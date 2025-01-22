// Заполните информацию об авторских правах на странице настроек проекта.

#pragma once

// Включение необходимых заголовочных файлов для работы с UMG (Unreal Motion Graphics)
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

// Включение заголовочного файла для работы с пользовательскими виджетами
#include "Blueprint/UserWidget.h"
// Включение сгенерированного заголовочного файла для данного класса
#include "ObjectContainerCellWidget.generated.h"

// Предварительное объявление класса UContainerComponent
class UContainerComponent;

// Определение перечисления для типов привязки ячейки контейнера
UENUM(BlueprintType)
enum class EContainerCellBinding : uint8 {
    Player = 0        UMETA(DisplayName = "Player"),        // Игрок
    ExternalObject = 1 UMETA(DisplayName = "External Object"), // Внешний объект
};

/**
 * Класс виджета ячейки контейнера объектов для песочницы
 */
UCLASS()
class UNREALSANDBOXTOOLKIT_API USandboxObjectContainerCellWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    // Имя контейнера
    UPROPERTY(EditAnywhere, Category = "SandboxInventory")
    FName ContainerName;

    // Тип привязки ячейки контейнера
    UPROPERTY(EditAnywhere, Category = "SandboxInventory")
    EContainerCellBinding CellBinding;

    // Функция для получения цвета границы слота
    UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
    FLinearColor SlotBorderColor(int32 SlotId);

    // Функция для выбора слота
    UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
    void SelectSlot(int32 SlotId);

    // Функция для наведения на слот
    UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
    void HoverSlot(int32 SlotId);

    // Функция для получения текстуры слота
    UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
    UTexture2D* GetSlotTexture(int32 SlotId);

    // Функция для получения текста количества в слоте
    UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
    FString SlotGetAmountText(int32 SlotId);

    // Функция для перемещения предмета из одного слота в другой
    UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
    bool SlotDrop(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne = false);

    // Функция для проверки, пуст ли слот
    UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
    bool SlotIsEmpty(int32 SlotId);

    // Функция для обработки основного действия слота
    UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
    void HandleSlotMainAction(int32 SlotId);

    // Функция для получения открытого объекта
    UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
    AActor* GetOpenedObject();

    // Функция для получения открытого контейнера
    UFUNCTION(BlueprintCallable, Category = "SandboxInventory")
    UContainerComponent* GetOpenedContainer();

protected:
    // Функция для получения контейнера
    UContainerComponent* GetContainer();

    // Функция для проверки, является ли контейнер внешним
    bool IsExternal();

private:
    // Внутренняя функция для перемещения предмета из одного слота в другой
    bool SlotDropInternal(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne);
};
