#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GenMaterialUtils.generated.h"

/** Utility functions for creating and querying material nodes via MCP */
UCLASS()
class GENERATIVEAISUPPORTEDITOR_API UGenMaterialUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /**
     * Add a material expression node to a material.
     * @param MaterialPath Path to the material asset e.g. "/Game/Materials/M_Test"
     * @param NodeClass Name of the material expression class e.g. "MaterialExpressionMultiply"
     * @param NodeX X position in the material graph
     * @param NodeY Y position in the material graph
     * @param PropertiesJson JSON object of editable property values
     * @return JSON string with success flag and the created node GUID
     */
    UFUNCTION(BlueprintCallable, Category="Generative AI|Material Utils")
    static FString AddMaterialNode(const FString& MaterialPath, const FString& NodeClass, float NodeX, float NodeY, const FString& PropertiesJson);

    /**
     * Get a list of all available material expression classes and their editable properties.
     * @return JSON array of objects {"class":"MaterialExpressionAdd","properties":["A","B"]}
     */
    UFUNCTION(BlueprintCallable, Category="Generative AI|Material Utils")
    static FString GetAllMaterialNodes();
};

