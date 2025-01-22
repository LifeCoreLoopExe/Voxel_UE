#pragma once // Защита от множественного включения этого заголовочного файла

#include "Runtime/UMG/Public/UMG.h" // Подключение заголовочного файла для UMG (Unreal Motion Graphics)
#include "Runtime/UMG/Public/UMGStyle.h" // Подключение заголовочного файла для стилей UMG
#include "Runtime/UMG/Public/Slate/SObjectWidget.h" // Подключение заголовочного файла для виджетов Slate
#include "Runtime/UMG/Public/IUMGModule.h" // Подключение заголовочного файла для модуля UMG
#include "Runtime/UMG/Public/Blueprint/UserWidget.h" // Подключение заголовочного файла для пользовательских виджетов

#include "Blueprint/UserWidget.h" // Подключение заголовочного файла для пользовательских виджетов (дублирование, можно удалить)
#include "ObjectContainerCellWidget.generated.h" // Генерация кода для этого заголовочного файла

class UContainerComponent; // Предварительное объявление класса UContainerComponent

// Перечисление для привязки ячейки контейнера
UENUM(BlueprintType) 
enum class EContainerCellBinding : uint8 {
	Player = 0 UMETA(DisplayName = "Player"), // Привязка к игроку
	ExternalObject = 1 UMETA(DisplayName = "External Object"), // Привязка к внешнему объекту
};
UCLASS() 
class UNREALSANDBOXTOOLKIT_API USandboxObjectContainerCellWidget : public UUserWidget { 
	GENERATED_BODY() // Генерация тела класса

public:	

	UPROPERTY(EditAnywhere, Category = "SandboxInventory") 
	FName ContainerName; // Имя контейнера

	UPROPERTY(EditAnywhere, Category = "SandboxInventory") 
	EContainerCellBinding CellBinding; // Привязка ячейки контейнера

	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") 
	FLinearColor SlotBorderColor(int32 SlotId); // Метод для получения цвета границы слота по его идентификатору

	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") 
	void SelectSlot(int32 SlotId); // Метод для выбора слота по его идентификатору

	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") 
	void HoverSlot(int32 SlotId); // Метод для обработки наведения на слот по его идентификатору

	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") 
	UTexture2D* GetSlotTexture(int32 SlotId); // Метод для получения текстуры слота по его идентификатору

	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") 
	FString SlotGetAmountText(int32 SlotId); // Метод для получения текстового представления количества объектов в слоте

	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") 
	bool SlotDrop(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne = false); 
    // Метод для передачи объекта из одного слота в другой (или контейнер)

	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") 
	bool SlotIsEmpty(int32 SlotId); // Метод для проверки, пуст ли слот по его идентификатору

	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") 
	void HandleSlotMainAction(int32 SlotId); // Метод для обработки основного действия со слотом по его идентификатору

	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") 
	AActor* GetOpenedObject(); // Метод для получения открытого объекта из контейнера
	
	UFUNCTION(BlueprintCallable, Category = "SandboxInventory") 
	UContainerComponent* GetOpenedContainer(); // Метод для получения открытого контейнера

protected:
	UContainerComponent* GetContainer(); // Защищенный метод для получения компонента контейнера

	bool IsExternal(); // Метод для проверки, является ли объект внешним

private:
	bool SlotDropInternal(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne); 
    // Внутренний метод для передачи объекта между слотами или контейнерами
};
