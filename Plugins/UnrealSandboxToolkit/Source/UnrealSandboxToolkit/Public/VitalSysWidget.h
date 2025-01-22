// Fill out your copyright notice in the Description page of Project Settings. // Заполните уведомление об авторских правах на странице описания в настройках проекта.

#pragma once // Указание компилятору включить этот файл только один раз

#include "Runtime/UMG/Public/UMG.h" // Подключение заголовочного файла UMG для работы с пользовательским интерфейсом
#include "Runtime/UMG/Public/UMGStyle.h" // Подключение заголовочного файла для стилей UMG
#include "Runtime/UMG/Public/Slate/SObjectWidget.h" // Подключение заголовочного файла для работы с виджетами Slate
#include "Runtime/UMG/Public/IUMGModule.h" // Подключение интерфейса модуля UMG
#include "Runtime/UMG/Public/Blueprint/UserWidget.h" // Подключение заголовочного файла для пользовательских виджетов Blueprints

#include "Blueprint/UserWidget.h" // Подключение заголовочного файла для пользовательских виджетов Blueprints (дублирование)
#include "VitalSysWidget.generated.h" // Генерация заголовочного файла для класса USandboxVitalSysWidget


class UVitalSystemComponent; // Объявление класса UVitalSystemComponent

/**
 * 
 */
UCLASS() // Определение класса USandboxVitalSysWidget, доступного в Blueprints
class UNREALSANDBOXTOOLKIT_API USandboxVitalSysWidget : public UUserWidget // Определение класса USandboxVitalSysWidget, наследующего от UUserWidget
{
	GENERATED_BODY() // Генерация тела класса
	
public:	

	UFUNCTION(BlueprintCallable, Category = "SandboxVitalSys") // Метод GetHealth, доступный из Blueprints в категории "SandboxVitalSys"
	float GetHealth(); // Метод для получения текущего здоровья

	UFUNCTION(BlueprintCallable, Category = "SandboxVitalSys") // Метод GetMaxHealth, доступный из Blueprints в категории "SandboxVitalSys"
	float GetMaxHealth(); // Метод для получения максимального здоровья

	UFUNCTION(BlueprintCallable, Category = "Sandbox Widget") // Метод GetHealthInPercent, доступный из Blueprints в категории "Sandbox Widget"
	float GetHealthInPercent(); // Метод для получения здоровья в процентах

	UFUNCTION(BlueprintCallable, Category = "Sandbox Widget") // Метод GetStaminaInPercent, доступный из Blueprints в категории "Sandbox Widget"
	float GetStaminaInPercent(); // Метод для получения выносливости в процентах

private:

	UVitalSystemComponent* GetVitalSystemComponent(); // Приватный метод для получения компонента жизненной системы
};
