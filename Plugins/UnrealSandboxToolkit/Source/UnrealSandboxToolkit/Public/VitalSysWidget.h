// Fill out your copyright notice in the Description page of Project Settings.

#pragma once // Включает файл только один раз

#include "Runtime/UMG/Public/UMG.h" // Включает основные функции UMG
#include "Runtime/UMG/Public/UMGStyle.h" // Включает стили UMG
#include "Runtime/UMG/Public/Slate/SObjectWidget.h" // Включает виджеты Slate
#include "Runtime/UMG/Public/IUMGModule.h" // Включает модуль UMG
#include "Runtime/UMG/Public/Blueprint/UserWidget.h" // Включает пользовательские виджеты Blueprint

#include "Blueprint/UserWidget.h" // Включает пользовательские виджеты
#include "VitalSysWidget.generated.h" // Включает сгенерированный заголовочный файл для VitalSysWidget

class UVitalSystemComponent; // Предварительное объявление класса UVitalSystemComponent

UCLASS() // Объявление класса, доступного для рефлексии UObject
class UNREALSANDBOXTOOLKIT_API USandboxVitalSysWidget : public UUserWidget { // Объявление класса USandboxVitalSysWidget, наследующегося от UUserWidget
    GENERATED_BODY() // Макрос для генерации тела класса

public:
    UFUNCTION(BlueprintCallable, Category = "SandboxVitalSys") // Функция, доступная для вызова из Blueprint
    float GetHealth(); // Метод для получения текущего здоровья

    UFUNCTION(BlueprintCallable, Category = "SandboxVitalSys") // Функция, доступная для вызова из Blueprint
    float GetMaxHealth(); // Метод для получения максимального здоровья

    UFUNCTION(BlueprintCallable, Category = "Sandbox Widget") // Функция, доступная для вызова из Blueprint
    float GetHealthInPercent(); // Метод для получения здоровья в процентах

    UFUNCTION(BlueprintCallable, Category = "Sandbox Widget") // Функция, доступная для вызова из Blueprint
    float GetStaminaInPercent(); // Метод для получения выносливости в процентах

private:
    UVitalSystemComponent* GetVitalSystemComponent(); // Метод для получения компонента системы жизненных показателей
};

