
#pragma once // Включает файл только один раз

#include "Runtime/UMG/Public/UMG.h" // Включает основные функции UMG
#include "Runtime/UMG/Public/UMGStyle.h" // Включает стили UMG
#include "Runtime/UMG/Public/Slate/SObjectWidget.h" // Включает виджеты Slate
#include "Runtime/UMG/Public/IUMGModule.h" // Включает модуль UMG
#include "Runtime/UMG/Public/Blueprint/UserWidget.h" // Включает пользовательские виджеты Blueprint

#include "Blueprint/UserWidget.h" // Включает пользовательские виджеты
#include "ObjectContainerCellWidget.generated.h" // Включает сгенерированный заголовочный файл для виджета

class UContainerComponent; // Предварительное объявление класса UContainerComponent

UENUM(BlueprintType) // Объявление перечисления, доступного для Blueprint
enum class EContainerCellBinding : uint8 {
    Player = 0 UMETA(DisplayName = "Player"), // Элемент перечисления для игрока
    ExternalObject = 1 UMETA(DisplayName = "External Object"), // Элемент перечисления для внешнего объекта
};

UCLASS() // Объявление класса, доступного для рефлексии UObject
class UNREALSANDBOXTOOLKIT_API USandboxObjectContainerCellWidget : public UUserWidget { // Объявление класса USandboxObjectContainerCellWidget, наследующегося от UUserWidget
    GENERATED_BODY() // Макрос для генерации тела класса

public:
    UPROPERTY(EditAnywhere, Category = "SandboxInventory") // Свойство, доступное для редактирования в редакторе
    FName ContainerName; // Имя контейнера

    UPROPERTY(EditAnywhere, Category = "SandboxInventory") // Свойство, доступное для редактирования в редакторе
    EContainerCellBinding CellBinding; // Привязка ячейки контейнера

    UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Функция, доступная для вызова из Blueprint
    FLinearColor SlotBorderColor(int32 SlotId); // Метод для получения цвета границы слота

    UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Функция, доступная для вызова из Blueprint
    void SelectSlot(int32 SlotId); // Метод для выбора слота

    UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Функция, доступная для вызова из Blueprint
    void HoverSlot(int32 SlotId); // Метод для наведения на слот

    UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Функция, доступная для вызова из Blueprint
    UTexture2D* GetSlotTexture(int32 SlotId); // Метод для получения текстуры слота

    UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Функция, доступная для вызова из Blueprint
    FString SlotGetAmountText(int32 SlotId); // Метод для получения текста количества в слоте

    UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Функция, доступная для вызова из Blueprint
    bool SlotDrop(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne = false); // Метод для переноса слота

    UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Функция, доступная для вызова из Blueprint
    bool SlotIsEmpty(int32 SlotId); // Метод для проверки, пуст ли слот

    UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Функция, доступная для вызова из Blueprint
    void HandleSlotMainAction(int32 SlotId); // Метод для обработки основного действия слота

    UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Функция, доступная для вызова из Blueprint
    AActor* GetOpenedObject(); // Метод для получения открытого объекта

    UFUNCTION(BlueprintCallable, Category = "SandboxInventory") // Функция, доступная для вызова из Blueprint
    UContainerComponent* GetOpenedContainer(); // Метод для получения открытого контейнера

protected:
    UContainerComponent* GetContainer(); // Метод для получения контейнера

    bool IsExternal(); // Метод для проверки, является ли контейнер внешним

private:
    bool SlotDropInternal(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne); // Внутренний метод для переноса слота
};
