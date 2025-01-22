// Fill out your copyright notice in the Description page of Project Settings.

#pragma once // Защита от множественного включения этого заголовочного файла

#include "Runtime/UMG/Public/UMG.h" // Подключение заголовочного файла для UMG (Unreal Motion Graphics)
#include "Runtime/UMG/Public/UMGStyle.h" // Подключение заголовочного файла для стилей UMG
#include "Runtime/UMG/Public/Slate/SObjectWidget.h" // Подключение заголовочного файла для виджетов Slate
#include "Runtime/UMG/Public/IUMGModule.h" // Подключение заголовочного файла для модуля UMG
#include "Runtime/UMG/Public/Blueprint/UserWidget.h" // Подключение заголовочного файла для пользовательских виджетов

#include "Blueprint/UserWidget.h" // Подключение заголовочного файла для пользовательских виджетов (дублирование, можно удалить)
#include "VitalSysWidget.generated.h" // Генерация кода для этого заголовочного файла

class UVitalSystemComponent; // Предварительное объявление класса UVitalSystemComponent

/**
 * Класс виджета системы жизней в песочнице
 */
UCLASS() 
class UNREALSANDBOXTOOLKIT_API USandboxVitalSysWidget : public UUserWidget { 
	GENERATED_BODY() // Генерация тела класса
	
public:	
	// Метод для получения текущего значения здоровья
	UFUNCTION(BlueprintCallable, Category = "SandboxVitalSys") 
	float GetHealth(); 

	// Метод для получения максимального значения здоровья
	UFUNCTION(BlueprintCallable, Category = "SandboxVitalSys") 
	float GetMaxHealth(); 

	// Метод для получения текущего здоровья в процентах относительно максимума
	UFUNCTION(BlueprintCallable, Category = "Sandbox Widget") 
	float GetHealthInPercent(); 

	// Метод для получения текущей выносливости в процентах относительно максимума
	UFUNCTION(BlueprintCallable, Category = "Sandbox Widget") 
	float GetStaminaInPercent(); 

private:
	// Метод для получения компонента системы жизней
	UVitalSystemComponent* GetVitalSystemComponent(); 
};
