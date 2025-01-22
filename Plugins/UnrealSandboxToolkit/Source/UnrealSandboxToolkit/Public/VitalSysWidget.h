// Этот файл является заголовочным файлом для виджета системы жизненных показателей

#pragma once  // Защита от повторного включения файла

// Подключаем все необходимые заголовочные файлы для работы с UMG (Unreal Motion Graphics)
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#include "Blueprint/UserWidget.h"
#include "VitalSysWidget.generated.h"

// Предварительное объявление класса, который будет использоваться позже
class UVitalSystemComponent;

/**
 * Класс USandboxVitalSysWidget - это виджет для отображения жизненных показателей персонажа
 * Наследуется от UUserWidget, что позволяет создавать UI элементы в Blueprint
 */
UCLASS()  // Макрос, указывающий Unreal Engine, что это класс UE
class UNREALSANDBOXTOOLKIT_API USandboxVitalSysWidget : public UUserWidget
{
	GENERATED_BODY()  // Макрос, генерирующий стандартный код UE
	
public:	
	// Функции, которые можно вызывать из Blueprint:
	
	// Получить текущее значение здоровья
	UFUNCTION(BlueprintCallable, Category = "SandboxVitalSys")
	float GetHealth();

	// Получить максимальное значение здоровья
	UFUNCTION(BlueprintCallable, Category = "SandboxVitalSys")
	float GetMaxHealth();

	// Получить здоровье в процентах (от 0 до 100)
	UFUNCTION(BlueprintCallable, Category = "Sandbox Widget")
	float GetHealthInPercent();

	// Получить выносливость в процентах (от 0 до 100)
	UFUNCTION(BlueprintCallable, Category = "Sandbox Widget")
	float GetStaminaInPercent();

private:
	// Приватная функция для получения компонента системы жизненных показателей
	UVitalSystemComponent* GetVitalSystemComponent();
};
