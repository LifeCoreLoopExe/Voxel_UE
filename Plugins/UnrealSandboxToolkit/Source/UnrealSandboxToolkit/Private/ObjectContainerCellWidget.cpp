// Copyright blackw 2015-2020

#include "ObjectContainerCellWidget.h" // Включает заголовочный файл для ObjectContainerCellWidget
#include "ContainerComponent.h" // Включает заголовочный файл для ContainerComponent
#include "SandboxObject.h" // Включает заголовочный файл для SandboxObject
#include "SandboxPlayerController.h" // Включает заголовочный файл для SandboxPlayerController

FLinearColor USandboxObjectContainerCellWidget::SlotBorderColor(int32 SlotId) {
    if (ContainerName == TEXT("Inventory")) { // TODO: fix
        ASandboxPlayerController* PlayerController = Cast<ASandboxPlayerController>(GetOwningPlayer());
        if (PlayerController) {
            if (PlayerController->CurrentInventorySlot == SlotId) {
                return FLinearColor(0.1, 0.4, 1, 1); // Возвращает цвет границы слота
            }
        }
    }
    return FLinearColor(0, 0, 0, 0.5); // Возвращает цвет границы слота по умолчанию
}

FString USandboxObjectContainerCellWidget::SlotGetAmountText(int32 SlotId) {
    UContainerComponent* Container = GetContainer();
    if (Container != nullptr) {
        const FContainerStack* Stack = Container->GetSlot(SlotId);
        if (Stack != nullptr) {
            if (Stack->GetObject() != nullptr) {
                const ASandboxObject* DefaultObject = Cast<ASandboxObject>(Stack->GetObject());
                if (DefaultObject != nullptr) {
                    if (!DefaultObject->bStackable) {
                        return TEXT(""); // Возвращает пустую строку, если объект не складывается
                    }
                }

                if (Stack->Amount > 0) {
                    return FString::Printf(TEXT("%d"), Stack->Amount); // Возвращает количество объектов в слоте
                }
            }
        }
    }

    return TEXT(""); // Возвращает пустую строку, если слот пуст
}

bool USandboxObjectContainerCellWidget::IsExternal() {
    return CellBinding == EContainerCellBinding::ExternalObject; // Возвращает true, если привязка ячейки контейнера внешняя
}

UContainerComponent* USandboxObjectContainerCellWidget::GetContainer() {
    if (IsExternal()) {
        ASandboxPlayerController* SandboxPC = Cast<ASandboxPlayerController>(GetOwningPlayer());
        if (SandboxPC) {
            return SandboxPC->GetOpenedContainer(); // Возвращает открытый контейнер
        }
    } else {
        APawn* Pawn = GetOwningPlayer()->GetPawn();
        if (Pawn) {
            TArray<UContainerComponent*> Components;
            Pawn->GetComponents<UContainerComponent>(Components);

            for (UContainerComponent* Container : Components) {
                if (Container->GetName().Equals(ContainerName.ToString())) {
                    return Container; // Возвращает контейнер по имени
                }
            }
        }
    }

    return nullptr; // Возвращает nullptr, если контейнер не найден
}

UTexture2D* USandboxObjectContainerCellWidget::GetSlotTexture(int32 SlotId) {
    UContainerComponent* Container = GetContainer();
    if (Container != nullptr) {
        const FContainerStack* Stack = Container->GetSlot(SlotId);
        if (Stack != nullptr) {
            if (Stack->Amount > 0) {
                if (Stack->GetObject() != nullptr) {
                    const ASandboxObject* DefaultObject = Cast<ASandboxObject>(Stack->GetObject());
                    if (DefaultObject != nullptr) {
                        return DefaultObject->IconTexture; // Возвращает текстуру иконки объекта
                    }
                }
            }
        }
    }

    return nullptr; // Возвращает nullptr, если текстура не найдена
}

void USandboxObjectContainerCellWidget::SelectSlot(int32 SlotId) {
    UE_LOG(LogTemp, Log, TEXT("SelectSlot: %d"), SlotId); // Логирует выбор слота
}

void USandboxObjectContainerCellWidget::HoverSlot(int32 SlotId) {
    ASandboxPlayerController* LocalController = Cast<ASandboxPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (LocalController) {
        int32 Sid = -1;

        UContainerComponent* Container = GetContainer();
        if (Container != nullptr) {
            const FContainerStack* Stack = Container->GetSlot(SlotId);
            if (Stack != nullptr && Stack->Amount > 0) {
                Sid = SlotId;
            }
        }

        LocalController->OnContainerSlotHover(Sid, *Container->GetName()); // Обрабатывает наведение на слот
    }
}

bool USandboxObjectContainerCellWidget::SlotDrop(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne) {
    bool bResult = SlotDropInternal(SlotDropId, SlotTargetId, SourceActor, SourceContainer, bOnlyOne);
    if (bResult) {
        ASandboxPlayerController* LocalController = Cast<ASandboxPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
        if (LocalController && LocalController->GetNetMode() != NM_Client) {
            const FString TargetContainerName = GetContainer()->GetName();
            const FString SourceContainerName = SourceContainer->GetName();
            LocalController->OnContainerDropSuccess(SlotTargetId, *SourceContainerName, *TargetContainerName); // Обрабатывает успешное перемещение слота
        }
    }

    return bResult;
}

bool USandboxObjectContainerCellWidget::SlotDropInternal(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne) {
    UE_LOG(LogTemp, Log, TEXT("UI cell drop: drop id -> %d ---> target id -> %d"), SlotDropId, SlotTargetId);

    if (SourceContainer == nullptr) {
        return false;
    }

    if (SourceActor == nullptr) {
        return false;
    }

    UContainerComponent* TargetContainer = GetContainer();
    if (!TargetContainer) {
        return false;
    }

    if (SlotDropId == SlotTargetId && TargetContainer == SourceContainer) {
        return false;
    }

    return TargetContainer->SlotTransfer(SlotDropId, SlotTargetId, SourceActor, SourceContainer, bOnlyOne); // Перемещает слот
}

bool USandboxObjectContainerCellWidget::SlotIsEmpty(int32 SlotId) {
    return false; // Возвращает false (заглушка)
}

void USandboxObjectContainerCellWidget::HandleSlotMainAction(int32 SlotId) {
    ASandboxPlayerController* SandboxPC = Cast<ASandboxPlayerController>(GetOwningPlayer());
    if (SandboxPC) {
        SandboxPC->OnContainerMainAction(SlotId, ContainerName); // Обрабатывает основное действие слота
    }
}

AActor* USandboxObjectContainerCellWidget::GetOpenedObject() {
    if (IsExternal()) {
        ASandboxPlayerController* SandboxPC = Cast<ASandboxPlayerController>(GetOwningPlayer());
        if (SandboxPC != nullptr) {
            return SandboxPC->GetOpenedObject(); // Возвращает открытый объект
        }
    } else {
        return GetOwningPlayer()->GetPawn(); // Возвращает аватара игрока
    }

    return nullptr; // Возвращает nullptr, если объект не найден
}

UContainerComponent* USandboxObjectContainerCellWidget::GetOpenedContainer() {
    return GetContainer(); // Возвращает открытый контейнер
}
