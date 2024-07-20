// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#include "../TerrainController.h"

#include <vector>
#include <tuple>

#include "Blueprint/UserWidget.h"
#include "SelectedObjectWidget.generated.h"

/**
 * 
 */
UCLASS()
class DARKLITHOSPHERE_API USelectedObjectWidget : public UUserWidget {
	GENERATED_BODY()
	
protected:
	
	UFUNCTION(BlueprintCallable, Category = "Sandbox widget")
	FString GetSandboxSelectedObjectText();

	UFUNCTION(BlueprintCallable, Category = "Sandbox widget")
	FString GetSandboxKeyText();

	UFUNCTION(BlueprintCallable, Category = "Sandbox widget")
	FString GetSandboxKeyText2();

	UFUNCTION(BlueprintCallable, Category = "Sandbox widget")
	FString GetSandboxKeyDescription();

	UFUNCTION(BlueprintCallable, Category = "Sandbox widget")
	FString GetSandboxKeyDescription2();


private:

	void MakeTextList();

	std::vector<std::tuple<FString, FString>> List;
	
};
