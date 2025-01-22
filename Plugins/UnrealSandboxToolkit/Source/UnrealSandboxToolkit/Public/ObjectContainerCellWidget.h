// Fill out your copyright notice in the Description page of Project Settings. // Заполните уведомление об авторских правах на странице описания в настройках проекта.

#pragma once // Указание компилятору включить этот файл только один раз

#include "Runtime/UMG/Public/UMG.h" // Подключение заголовочного файла UMG для работы с пользовательским интерфейсом
#include "Runtime/UMG/Public/UMGStyle.h" // Подключение заголовочного файла для стилей UMG
#include "Runtime/UMG/Public/Slate/SObjectWidget.h" // Подключение заголовочного файла для работы с виджетами Slate
#include "Runtime/UMG/Public/IUMGModule.h" // Подключение интерфейса модуля UMG
#include "Runtime/UMG/Public/Blueprint/UserWidget.h" // Подключение заголовочного файла для пользовательских виджетов Blueprints

#include "Blueprint/UserWidget.h" // Подключение заголовочного файла для пользовательских виджетов Blueprints (дублирование)
#include "ObjectContainerCellWidget.generated.h" // Генерация заголовочного файла для виджета контейнера объектов

class UContainerComponent; // Объявление класса UContainerComponent

UENUM(BlueprintType) // Определение перечисления, доступного в Blueprints
enum class EContainerCellBinding : uint8 { // Определение перечисления EContainerCellBinding с типом uint8
	Player = 0				UMETA(DisplayName = "Player"), // Игрок, отображаемое имя "Player"
	ExternalObject = 1		UMETA(DisplayName = "External Object"), // Внешний объект, отображаемое имя "External Object"
};


/**
 * 
 */
UCLASS() // Определение класса, доступного в Blueprints
class UNREALSANDBOXTOOLKIT_API USandboxObjectContainerCellWidget : public UUserWidget // Определение класса USandboxObjectContainerCellWidget, наследующего от UUserWidget
{
	GENERATED_BODY() // Генерация тела класса
	
public:	

	UPROPERTY(EditAnywhere, Category = "SandboxInventory") // Свойство, редактируемое в редакторе, в категории "SandboxInventory"
	FName ContainerName; // Имя контейнера

	UPROPERTY(EditAnywhere, Category = "SandboxInventory") // Свойство, редактируемое в редакторе, в категории "SandboxInventory"
	EContainerCellBinding CellBinding; // Привязка ячейки контейнера

	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Метод, доступный из Blueprints, в категории "SandboxInventory"
	FLinearColor SlotBorderColor(int32 SlotId); // Метод для получения цвета границы слота по его идентификатору

	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Метод, доступный из Blueprints, в категории "SandboxInventory"
	void SelectSlot(int32 SlotId); // Метод для выбора слота по его идентификатору

	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Метод, доступный из Blueprints, в категории "SandboxInventory"
	void HoverSlot(int32 SlotId); // Метод для обработки наведения курсора на слот по его идентификатору

	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Метод, доступный из Blueprints, в категории "SandboxInventory"
	UTexture2D* GetSlotTexture(int32 SlotId); // Метод для получения текстуры слота по его идентификатору

	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Метод, доступный из Blueprints, в категории "SandboxInventory"
	FString SlotGetAmountText(int32 SlotId); // Метод для получения текста количества объектов в слоте по его идентификатору

	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Метод, доступный из Blueprints, в категории "SandboxInventory"
	bool SlotDrop(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne = false); // Метод для передачи объекта из одного слота в другой

	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Метод, доступный из Blueprints, в категории "SandboxInventory"
	bool SlotIsEmpty(int32 SlotId); // Метод для проверки, пуст ли указанный слот

	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Метод, доступный из Blueprints, в категории "SandboxInventory"
	void HandleSlotMainAction(int32 SlotId); // Метод для обработки основного действия со слотом по его идентификатору

	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Метод, доступный из Blueprints, в категории "SandboxInventory"
	AActor* GetOpenedObject(); // Метод для получения открытого объекта (например, объекта в текущем слоте)
	
	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Метод, доступный из Blueprints, в категории "SandboxInventory"
	UContainerComponent* GetOpenedContainer(); // Метод для получения открытого контейнера

protected:
	UContainerComponent* GetContainer(); // Защищенный метод для получения контейнера

	bool IsExternal(); // Защищенный метод для проверки является ли объект внешним

private:

	bool SlotDropInternal(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne); // Приватный метод для внутренней реализации передачи объектов между слотами
};
