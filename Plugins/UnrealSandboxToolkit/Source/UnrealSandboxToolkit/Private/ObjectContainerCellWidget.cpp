// Copyright blackw 2015-2020

#include "ObjectContainerCellWidget.h" // Подключение заголовочного файла для виджета ячейки контейнера объектов
#include "ContainerComponent.h" // Подключение заголовочного файла для компонента контейнера
#include "SandboxObject.h" // Подключение заголовочного файла для объекта песочницы
#include "SandboxPlayerController.h" // Подключение заголовочного файла для контроллера игрока песочницы

FLinearColor USandboxObjectContainerCellWidget::SlotBorderColor(int32 SlotId) {
    if (ContainerName == TEXT("Inventory")) { // Если имя контейнера равно "Inventory"
        ASandboxPlayerController* PlayerController = Cast<ASandboxPlayerController>(GetOwningPlayer()); // Получение контроллера игрока
        if (PlayerController) { // Если контроллер игрока существует
            if (PlayerController->CurrentInventorySlot == SlotId) { // Если текущий слот инвентаря равен SlotId
                return FLinearColor(0.1, 0.4, 1, 1); // Возвращаем цвет границы слота
            }
        }
    }
    return FLinearColor(0, 0, 0, 0.5); // Возвращаем цвет границы слота по умолчанию
}

FString USandboxObjectContainerCellWidget::SlotGetAmountText(int32 SlotId) {
    UContainerComponent* Container = GetContainer(); // Получение компонента контейнера
    if (Container != NULL) { // Если контейнер существует
        const FContainerStack* Stack = Container->GetSlot(SlotId); // Получение стека объектов в слоте
        if (Stack != NULL) { // Если стек существует
            if (Stack->GetObject() != nullptr) { // Если объект в стеке существует
                const ASandboxObject* DefaultObject = Cast<ASandboxObject>(Stack->GetObject()); // Получение объекта песочницы
                if (DefaultObject != nullptr) { // Если объект песочницы существует
                    if (!DefaultObject->bStackable) { // Если объект не стекабельный
                        return TEXT(""); // Возвращаем пустую строку
                    }
                }

                if (Stack->Amount > 0) { // Если количество объектов в стеке больше нуля
                    return FString::Printf(TEXT("%d"), Stack->Amount); // Возвращаем количество объектов в виде строки
                }
            }
        }
    }

    return TEXT(""); // Возвращаем пустую строку по умолчанию
}

bool USandboxObjectContainerCellWidget::IsExternal() {
    return CellBinding == EContainerCellBinding::ExternalObject; // Возвращаем true, если ячейка привязана к внешнему объекту
}

UContainerComponent* USandboxObjectContainerCellWidget::GetContainer() {
    if (IsExternal()) { // Если ячейка привязана к внешнему объекту
        ASandboxPlayerController* SandboxPC = Cast<ASandboxPlayerController>(GetOwningPlayer()); // Получение контроллера игрока
        if (SandboxPC) { // Если контроллер игрока существует
            return SandboxPC->GetOpenedContainer(); // Возвращаем открытый контейнер
        }
    } else { // Иначе
        APawn* Pawn = GetOwningPlayer()->GetPawn(); // Получение персонажа игрока
        if (Pawn) { // Если персонаж существует
            TArray<UContainerComponent*> Components; // Массив компонентов контейнера
            Pawn->GetComponents<UContainerComponent>(Components); // Получение всех компонентов контейнера персонажа

            for (UContainerComponent* Container : Components) { // Перебор всех компонентов контейнера
                if (Container->GetName().Equals(ContainerName.ToString())) { // Если имя компонента равно имени контейнера
                    return Container; // Возвращаем компонент контейнера
                }
            }
        }
    }

    return nullptr; // Возвращаем nullptr, если контейнер не найден
}

UTexture2D* USandboxObjectContainerCellWidget::GetSlotTexture(int32 SlotId) {
    UContainerComponent* Container = GetContainer(); // Получение компонента контейнера
    if (Container != nullptr) { // Если контейнер существует
        const FContainerStack* Stack = Container->GetSlot(SlotId); // Получение стека объектов в слоте
        if (Stack != nullptr) { // Если стек существует
            if (Stack->Amount > 0) { // Если количество объектов в стеке больше нуля
                if (Stack->GetObject() != nullptr) { // Если объект в стеке существует
                    const ASandboxObject* DefaultObject = Cast<ASandboxObject>(Stack->GetObject()); // Получение объекта песочницы
                    if (DefaultObject != nullptr) { // Если объект песочницы существует
                        return DefaultObject->IconTexture; // Возвращаем текстуру иконки объекта
                    }
                }
            }
        }
    }

    return nullptr; // Возвращаем nullptr, если текстура не найдена
}

void USandboxObjectContainerCellWidget::SelectSlot(int32 SlotId) {
    UE_LOG(LogTemp, Log, TEXT("SelectSlot: %d"), SlotId); // Логирование выбора слота
}

void USandboxObjectContainerCellWidget::HoverSlot(int32 SlotId) {
    ASandboxPlayerController* LocalController = Cast<ASandboxPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); // Получение контроллера игрока
    if (LocalController) { // Если контроллер игрока существует
        int32 Sid = -1; // Инициализация переменной для хранения идентификатора слота

        UContainerComponent* Container = GetContainer(); // Получение компонента контейнера
        if (Container != nullptr) { // Если контейнер существует
            const FContainerStack* Stack = Container->GetSlot(SlotId); // Получение стека объектов в слоте
            if (Stack != nullptr && Stack->Amount > 0) { // Если стек существует и количество объектов в стеке больше нуля
                Sid = SlotId; // Установка идентификатора слота
            }
        }

        LocalController->OnContainerSlotHover(Sid, *Container->GetName()); // Вызов функции при наведении на слот контейнера
    }
}

bool USandboxObjectContainerCellWidget::SlotDrop(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne) {
    bool bResult = SlotDropInternal(SlotDropId, SlotTargetId, SourceActor, SourceContainer, bOnlyOne); // Вызов внутренней функции для перемещения объекта
    if (bResult) { // Если перемещение успешно
        ASandboxPlayerController* LocalController = Cast<ASandboxPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); // Получение контроллера игрока
        if (LocalController && LocalController->GetNetMode() != NM_Client) { // Если контроллер игрока существует и не является клиентом
            const FString TargetContainerName = GetContainer()->GetName(); // Получение имени целевого контейнера
            const FString SourceContainerName = SourceContainer->GetName(); // Получение имени исходного контейнера
            LocalController->OnContainerDropSuccess(SlotTargetId, *SourceContainerName, *TargetContainerName); // Вызов функции при успешном перемещении объекта
        }
    }

    return bResult; // Возвращаем результат перемещения
}

bool USandboxObjectContainerCellWidget::SlotDropInternal(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne) {
    UE_LOG(LogTemp, Log, TEXT("UI cell drop: drop id -> %d ---> target id -> %d"), SlotDropId, SlotTargetId); // Логирование перемещения объекта

    if (SourceContainer == nullptr) { // Если исходный контейнер не существует
        return false; // Возвращаем false
    }

    if (SourceActor == nullptr) { // Если исходный актор не существует
        return false; // Возвращаем false
    }

    UContainerComponent* TargetContainer = GetContainer(); // Получение целевого контейнера
    if (!TargetContainer) { // Если целевой контейнер не существует
        return false; // Возвращаем false
    }

    if (SlotDropId == SlotTargetId && TargetContainer == SourceContainer) { // Если идентификаторы слотов и контейнеры совпадают
        return false; // Возвращаем false
    }

    return TargetContainer->SlotTransfer(SlotDropId, SlotTargetId, SourceActor, SourceContainer, bOnlyOne); // Вызов функции для перемещения объекта между слотами
}

bool USandboxObjectContainerCellWidget::SlotIsEmpty(int32 SlotId) {
    return false; // Возвращаем false (слот не пуст)
}

void USandboxObjectContainerCellWidget::HandleSlotMainAction(int32 SlotId) {
    ASandboxPlayerController* SandboxPC = Cast<ASandboxPlayerController>(GetOwningPlayer()); // Получение контроллера игрока
    if (SandboxPC) { // Если контроллер игрока существует
        SandboxPC->OnContainerMainAction(SlotId, ContainerName); // Вызов функции для основного действия с слотом
    }
}

AActor * USandboxObjectContainerCellWidget::GetOpenedObject() {
    if (IsExternal()) { // Если ячейка привязана к внешнему объекту
        ASandboxPlayerController* SandboxPC = Cast<ASandboxPlayerController>(GetOwningPlayer()); // Получение контроллера игрока
        if (SandboxPC != nullptr) { // Если контроллер игрока существует
            return SandboxPC->GetOpenedObject(); // Возвращаем открытый объект
        }
    } else { // Иначе
        return GetOwningPlayer()->GetPawn(); // Возвращаем персонажа игрока
    }

    return nullptr; // Возвращаем nullptr, если объект не найден
}

UContainerComponent * USandboxObjectContainerCellWidget::GetOpenedContainer() {
    return GetContainer(); // Возвращаем компонент контейнера