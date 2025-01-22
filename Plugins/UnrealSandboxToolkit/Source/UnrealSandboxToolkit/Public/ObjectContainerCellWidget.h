// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Подключаем все необходимые заголовочные файлы для работы с UMG (Unreal Motion Graphics - система UI в Unreal Engine)
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#include "Blueprint/UserWidget.h"
#include "ObjectContainerCellWidget.generated.h"

// Предварительное объявление класса, который будет использоваться позже
class UContainerComponent;

// Создаем перечисление (enum) для определения типа привязки ячейки контейнера
// Это можно использовать в Blueprint и C++
UENUM(BlueprintType)
enum class EContainerCellBinding : uint8 {
	Player = 0				UMETA(DisplayName = "Player"),     // Ячейка принадлежит игроку
	ExternalObject = 1		UMETA(DisplayName = "External Object"), // Ячейка принадлежит внешнему объекту
};

// Основной класс виджета ячейки контейнера
// Наследуется от UUserWidget - базового класса для всех виджетов пользовательского интерфейса
UCLASS()
class UNREALSANDBOXTOOLKIT_API USandboxObjectContainerCellWidget : public UUserWidget
{
	GENERATED_BODY() // Макрос, необходимый для работы Unreal Engine с классом
	
public:	
	// Свойства, доступные для редактирования в редакторе
	UPROPERTY(EditAnywhere, Category = "SandboxInventory")
	FName ContainerName; // Имя контейнера

	UPROPERTY(EditAnywhere, Category = "SandboxInventory")
	EContainerCellBinding CellBinding; // Тип привязки ячейки (к игроку или внешнему объекту)

	// Функции, которые можно вызывать из Blueprint

	// Получить цвет границы слота
	UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
	FLinearColor SlotBorderColor(int32 SlotId);

	// Выбрать слот
	UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
	void SelectSlot(int32 SlotId);

	// Подсветить слот при наведении
	UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
	void HoverSlot(int32 SlotId);

	// Получить текстуру для отображения в слоте
	UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
	UTexture2D* GetSlotTexture(int32 SlotId);

	// Получить текст с количеством предметов в слоте
	UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
	FString SlotGetAmountText(int32 SlotId);

	// Функция для перетаскивания предметов между слотами
	UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
	bool SlotDrop(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne = false);

	// Проверить, пуст ли слот
	UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
	bool SlotIsEmpty(int32 SlotId);

	// Обработать основное действие со слотом
	UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
	void HandleSlotMainAction(int32 SlotId);

	// Получить открытый объект
	UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
	AActor* GetOpenedObject();
	
	// Получить открытый контейнер
	UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
	UContainerComponent* GetOpenedContainer();

protected:
	// Защищенные методы, доступные только внутри класса и его наследников
	UContainerComponent* GetContainer(); // Получить компонент контейнера
	bool IsExternal(); // Проверить, является ли контейнер внешним

private:
	// Внутренняя реализация функции перетаскивания предметов
	bool SlotDropInternal(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne);
};
