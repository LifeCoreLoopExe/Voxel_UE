// Заполните информацию об авторских правах на странице описания настроек проекта.

#pragma once

// Подключение заголовочных файлов для работы с UMG (Unreal Motion Graphics)
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

// Подключение заголовочного файла для работы с пользовательскими виджетами
#include "Blueprint/UserWidget.h"
#include "VitalSysWidget.generated.h"

// Предварительное объявление класса UVitalSystemComponent
class UVitalSystemComponent;

/**
 *
 */
// Объявление класса USandboxVitalSysWidget, который наследуется от UUserWidget
UCLASS()
class UNREALSANDBOXTOOLKIT_API USandboxVitalSysWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// Функция для получения текущего значения здоровья
	UFUNCTION(BlueprintCallable, Category = "SandboxVitalSys")
	float GetHealth();

	// Функция для получения максимального значения здоровья
	UFUNCTION(BlueprintCallable, Category = "SandboxVitalSys")
	float GetMaxHealth();

	// Функция для получения здоровья в процентах
	UFUNCTION(BlueprintCallable, Category = "Sandbox Widget")
	float GetHealthInPercent();

	// Функция для получения выносливости в процентах
	UFUNCTION(BlueprintCallable, Category = "Sandbox Widget")
	float GetStaminaInPercent();

private:

	// Функция для получения компонента системы жизненных показателей
	UVitalSystemComponent* GetVitalSystemComponent();
};
