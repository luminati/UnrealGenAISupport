#include "MCP/GenMaterialUtils.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpression.h"
#include "MaterialEditingLibrary.h"
#include "UObject/UObjectIterator.h"
#include "UObject/ConstructorHelpers.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

FString UGenMaterialUtils::AddMaterialNode(const FString& MaterialPath, const FString& NodeClass, float NodeX, float NodeY, const FString& PropertiesJson)
{
    UMaterial* Material = LoadObject<UMaterial>(nullptr, *MaterialPath);
    if (!Material)
    {
        return TEXT("{\"success\": false, \"error\": \"Material not found\"}");
    }

    UClass* ExpressionClass = FindObject<UClass>(ANY_PACKAGE, *NodeClass);
    if (!ExpressionClass || !ExpressionClass->IsChildOf(UMaterialExpression::StaticClass()))
    {
        return TEXT("{\"success\": false, \"error\": \"Invalid node class\"}");
    }

    UMaterialExpression* Expression = Cast<UMaterialExpression>(UMaterialEditingLibrary::CreateMaterialExpression(Material, ExpressionClass, NodeX, NodeY));
    if (!Expression)
    {
        return TEXT("{\"success\": false, \"error\": \"Failed to create node\"}");
    }

    if (!PropertiesJson.IsEmpty())
    {
        TSharedPtr<FJsonObject> JsonObj;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(PropertiesJson);
        if (FJsonSerializer::Deserialize(Reader, JsonObj) && JsonObj.IsValid())
        {
            for (const auto& Pair : JsonObj->Values)
            {
                FProperty* Prop = ExpressionClass->FindPropertyByName(*Pair.Key);
                if (!Prop) continue;
                void* PropPtr = Prop->ContainerPtrToValuePtr<void>(Expression);
                if (FNumericProperty* NumProp = CastField<FNumericProperty>(Prop))
                {
                    if (Pair.Value->Type == EJson::Number)
                    {
                        double Val = Pair.Value->AsNumber();
                        if (NumProp->IsInteger())
                            NumProp->SetIntPropertyValue(PropPtr, (int64)Val);
                        else
                            NumProp->SetFloatingPointPropertyValue(PropPtr, Val);
                    }
                }
                else if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Prop))
                {
                    if (Pair.Value->Type == EJson::Boolean)
                        BoolProp->SetPropertyValue(PropPtr, Pair.Value->AsBool());
                }
                else if (FStrProperty* StrProp = CastField<FStrProperty>(Prop))
                {
                    if (Pair.Value->Type == EJson::String)
                        StrProp->SetPropertyValue(PropPtr, Pair.Value->AsString());
                }
                else if (FStructProperty* StructProp = CastField<FStructProperty>(Prop))
                {
                    if (StructProp->Struct == TBaseStructure<FLinearColor>::Get() && Pair.Value->Type == EJson::Array)
                    {
                        const TArray<TSharedPtr<FJsonValue>>& Arr = Pair.Value->AsArray();
                        if (Arr.Num() >= 3)
                        {
                            FLinearColor Color;
                            Color.R = Arr[0]->AsNumber();
                            Color.G = Arr[1]->AsNumber();
                            Color.B = Arr[2]->AsNumber();
                            Color.A = Arr.Num() > 3 ? Arr[3]->AsNumber() : 1.0f;
                            *StructProp->ContainerPtrToValuePtr<FLinearColor>(Expression) = Color;
                        }
                    }
                    else if (StructProp->Struct == TBaseStructure<FVector2D>::Get() && Pair.Value->Type == EJson::Array)
                    {
                        const TArray<TSharedPtr<FJsonValue>>& Arr = Pair.Value->AsArray();
                        if (Arr.Num() >= 2)
                        {
                            FVector2D Vec;
                            Vec.X = Arr[0]->AsNumber();
                            Vec.Y = Arr[1]->AsNumber();
                            *StructProp->ContainerPtrToValuePtr<FVector2D>(Expression) = Vec;
                        }
                    }
                }
            }
        }
    }

    if (!Expression->MaterialExpressionGuid.IsValid())
    {
        Expression->MaterialExpressionGuid = FGuid::NewGuid();
    }

    Material->PostEditChange();
    UMaterialEditingLibrary::RecompileMaterial(Material);

    return FString::Printf(TEXT("{\"success\": true, \"node_guid\": \"%s\"}"), *Expression->MaterialExpressionGuid.ToString());
}

FString UGenMaterialUtils::GetAllMaterialNodes()
{
    TArray<TSharedPtr<FJsonValue>> NodeArray;
    for (TObjectIterator<UClass> It; It; ++It)
    {
        UClass* Cls = *It;
        if (!Cls->IsChildOf(UMaterialExpression::StaticClass()) || Cls->HasAnyClassFlags(CLASS_Abstract))
        {
            continue;
        }

        TSharedPtr<FJsonObject> NodeObj = MakeShareable(new FJsonObject);
        NodeObj->SetStringField(TEXT("class"), Cls->GetName());
        TArray<TSharedPtr<FJsonValue>> Props;
        for (TFieldIterator<FProperty> PropIt(Cls, EFieldIteratorFlags::IncludeSuper); PropIt; ++PropIt)
        {
            FProperty* Prop = *PropIt;
            if (Prop->HasAnyPropertyFlags(CPF_Edit))
            {
                Props.Add(MakeShareable(new FJsonValueString(Prop->GetName())));
            }
        }
        NodeObj->SetArrayField(TEXT("properties"), Props);
        NodeArray.Add(MakeShareable(new FJsonValueObject(NodeObj)));
    }

    FString OutJson;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson);
    FJsonSerializer::Serialize(NodeArray, Writer);
    return OutJson;
}

