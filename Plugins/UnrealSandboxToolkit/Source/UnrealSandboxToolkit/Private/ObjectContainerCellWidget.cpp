// Copyright blackw 2015-2020

#include "ObjectContainerCellWidget.h"
#include "ContainerComponent.h"
#include "SandboxObject.h"
#include "SandboxPlayerController.h"

// Цвет рамки слота (как подсветка выбранного кармана)
FLinearColor USandboxObjectContainerCellWidget::SlotBorderColor(int32 SlotId) {
    if (ContainerName == TEXT("Inventory")) { // Если это инвентарь игрока
        ASandboxPlayerController* PlayerController = Cast<ASandboxPlayerController>(GetOwningPlayer());
        if (PlayerController) {
            // Если это текущий выбранный слот - красим в синий
            if (PlayerController->CurrentInventorySlot == SlotId) {
                return FLinearColor(0.1, 0.4, 1, 1); // Синий цвет выделения
            }
        }
    }
    return FLinearColor(0, 0, 0, 0.5); // Стандартный полупрозрачный черный
}

// Текст количества предметов (например "5" для 5 яблок)
FString USandboxObjectContainerCellWidget::SlotGetAmountText(int32 SlotId) {
    UContainerComponent* Container = GetContainer();
    if (Container) {
        const FContainerStack* Stack = Container->GetSlot(SlotId);
        if (Stack && Stack->Amount > 0) {
            // Если предмет нельзя стакать (например меч) - не показываем цифру
            if (Stack->GetObject() && !Stack->GetObject()->bStackable) {
                return TEXT(""); 
            }
            return FString::Printf(TEXT("%d"), Stack->Amount); // Превращаем число в текст
        }
    }
    return TEXT(""); // Пустая строка если слот пуст
}

// Это внешний контейнер? (типа сундука, а не инвентаря)
bool USandboxObjectContainerCellWidget::IsExternal() {
    return CellBinding == EContainerCellBinding::ExternalObject; 
}

// Получить контейнер (как найти нужный ящик с вещами)
UContainerComponent* USandboxObjectContainerCellWidget::GetContainer() {
    if (IsExternal()) { // Если это сундук/ящик
        ASandboxPlayerController* SandboxPC = Cast<ASandboxPlayerController>(GetOwningPlayer());
        if (SandboxPC) {
            return SandboxPC->GetOpenedContainer(); // Берем открытый контейнер
        }
    } else { // Если это инвентарь игрока
        APawn* Pawn = GetOwningPlayer()->GetPawn();
        if (Pawn) {
            // Ищем компонент-контейнер у персонажа по имени
            TArray<UContainerComponent*> Components;
            Pawn->GetComponents<UContainerComponent>(Components);
            
            for (UContainerComponent* Container : Components) {
                if (Container->GetName().Equals(ContainerName.ToString())) {
                    return Container; // Нашли нужный контейнер
                }
            }
        }
    }
    return nullptr; // Ничего не нашли
}

// Получить иконку предмета в слоте (как миниатюра предмета)
UTexture2D* USandboxObjectContainerCellWidget::GetSlotTexture(int32 SlotId) {
    UContainerComponent* Container = GetContainer();
    if (Container) {
        const FContainerStack* Stack = Container->GetSlot(SlotId);
        if (Stack && Stack->Amount > 0) {
            const ASandboxObject* DefaultObject = Cast<ASandboxObject>(Stack->GetObject());
            if (DefaultObject) {
                return DefaultObject->IconTexture; // Берем текстуру из настроек предмета
            }
        }
    }
    return nullptr; // Нет текстуры
}

// Клик по слоту (как нажатие на ячейку мышкой)
void USandboxObjectContainerCellWidget::SelectSlot(int32 SlotId) {
    UE_LOG(LogTemp, Log, TEXT("SelectSlot: %d"), SlotId);
}

// Наведение курсора на слот (как подсветка при наведении)
void USandboxObjectContainerCellWidget::HoverSlot(int32 SlotId) {
    ASandboxPlayerController* LocalController = Cast<ASandboxPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (LocalController) {
        LocalController->OnContainerSlotHover(SlotId, *Container->GetName()); // Говорим контроллеру о наведении
    }
}

// Перенос предмета между слотами (как перетаскивание вещи)
bool USandboxObjectContainerCellWidget::SlotDrop(...) {
    // Пытаемся перенести
    bool bResult = SlotDropInternal(...); 
    
    // Если успешно и мы на сервере
    if (bResult && LocalController->GetNetMode() != NM_Client) {
        // Сообщаем об успешном переносе
        LocalController->OnContainerDropSuccess(...);
    }
    return bResult;
}

// Внутренняя логика переноса предмета
bool USandboxObjectContainerCellWidget::SlotDropInternal(...) {
    // Нельзя переносить в тот же слот того же контейнера
    if (SlotDropId == SlotTargetId && TargetContainer == SourceContainer) {
        return false;
    }
    
    // Вызываем основную логику переноса из контейнера
    return TargetContainer->SlotTransfer(...);
}

// Основное действие со слотом (например использование предмета)
void USandboxObjectContainerCellWidget::HandleSlotMainAction(int32 SlotId) {
    ASandboxPlayerController* SandboxPC = Cast<ASandboxPlayerController>(GetOwningPlayer());
    if (SandboxPC) {
        SandboxPC->OnContainerMainAction(SlotId, ContainerName); // Передаем действие контроллеру
    }
}

// Получить открытый объект (сундук, ящик и т.д.)
AActor* USandboxObjectContainerCellWidget::GetOpenedObject() {
    if (IsExternal()) {
        ASandboxPlayerController* SandboxPC = ...;
        return SandboxPC->GetOpenedObject(); // Возвращаем открытый сундук
    } else {
        return GetOwningPlayer()->GetPawn(); // Возвращаем самого игрока
    }
}