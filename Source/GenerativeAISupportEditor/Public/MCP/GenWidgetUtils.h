// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GenWidgetUtils.generated.h"

class UWidgetTree;
class UWidget;
/**
 * 
 */
UCLASS()
class GENERATIVEAISUPPORTEDITOR_API  UGenWidgetUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Adds a new widget to a User Widget Blueprint's hierarchy.
     * Assumes the parent is a CanvasPanel for positioning unless specified otherwise.
     * @param UserWidgetPath Path to the User Widget Blueprint (e.g., "/Game/UI/WBP_MainMenu").
     * @param WidgetClassName Class name of the widget to add (e.g., "TextBlock", "Button", "Image", "CanvasPanel").
     * @param WidgetName Name for the new widget variable in the Blueprint (e.g., "TitleText", "StartButton").
     * @param ParentWidgetName Optional name of the parent widget to attach to.
     *                         When empty, attaches to the root panel if it is a
     *                         PanelWidget, otherwise the first CanvasPanel is
     *                         searched. If no parent is found and the new widget
     *                         is a panel with an empty tree, it becomes the root.
     * @return JSON string indicating success or failure, including the actual name assigned.
     */
    UFUNCTION(BlueprintCallable, Category = "MCP | UI Generation")
    static FString AddWidgetToUserWidget(const FString& UserWidgetPath, const FString& WidgetClassName, const FString& WidgetName, const FString& ParentWidgetName = TEXT(""));

    /**
     * Edits a property of a specific widget within a User Widget Blueprint.
     * Handles common properties and layout properties (e.g., "Slot.Size", "Slot.Position").
     * @param UserWidgetPath Path to the User Widget Blueprint.
     * @param WidgetName Name of the widget inside the User Widget to modify.
     * @param PropertyName Name of the property (e.g., "Text", "ColorAndOpacity", "Slot.Size", "Slot.Anchors").
     * @param ValueString String representation of the new value (e.g., "\"Hello World\"", "(R=1,G=0,B=0,A=1)", "(X=100,Y=50)", "(Minimum=(X=0.5,Y=0.5),Maximum=(X=0.5,Y=0.5))").
     * @return JSON string indicating success or failure.
     */
    UFUNCTION(BlueprintCallable, Category = "MCP | UI Generation")
    static FString EditWidgetProperty(const FString& UserWidgetPath, const FString& WidgetName, const FString& PropertyName, const FString& ValueString);

private:
    // Helper function to find a widget by name in the tree
    static UWidget* FindWidgetByName(UWidgetTree* WidgetTree, const FName& Name);
    // Helper to get the actual object (widget or slot) and property
    static bool FindPropertyAndObject(UWidget* TargetWidget, const FString& PropertyName, UObject*& OutObject, FProperty*& OutProperty);
    // Helper to save and recompile widget blueprint
    static bool SaveAndRecompileWidgetBlueprint(UBlueprint* WidgetBP);
};