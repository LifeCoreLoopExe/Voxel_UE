// Copyright blackw 2015-2020

#include "ObjectContainerCellWidget.h" // Подключаем заголовочный файл для виджета ячейки контейнера объектов
#include "ContainerComponent.h" // Подключаем заголовочный файл для компонента контейнера
#include "SandboxObject.h" // Подключаем заголовочный файл для объектов песочницы
#include "SandboxPlayerController.h" // Подключаем заголовочный файл для контроллера игрока песочницы

// Функция, возвращающая цвет границы слота в зависимости от его идентификатора
FLinearColor USandboxObjectContainerCellWidget::SlotBorderColor(int32 SlotId) {
	if (ContainerName == TEXT("Inventory")) { 	// Проверяем, является ли имя контейнера "Inventory"
		ASandboxPlayerController* PlayerController = Cast<ASandboxPlayerController>(GetOwningPlayer()); // Получаем контроллер игрока
		if (PlayerController) { // Если контроллер существует
			if (PlayerController->CurrentInventorySlot == SlotId) { // Проверяем, совпадает ли текущий слот с переданным идентификатором
				return FLinearColor(0.1, 0.4, 1, 1); // Возвращаем цвет границы для выбранного слота
			}
		}
	}
	return FLinearColor(0, 0, 0, 0.5); // Возвращаем цвет границы по умолчанию
}

// Функция, возвращающая текстовое представление количества объектов в слоте
FString USandboxObjectContainerCellWidget::SlotGetAmountText(int32 SlotId) {
	UContainerComponent* Container = GetContainer(); // Получаем контейнер
	if (Container != NULL) { // Если контейнер существует
		const FContainerStack* Stack = Container->GetSlot(SlotId); // Получаем стек объектов из слота
		if (Stack != NULL) { // Если стек существует
			if (Stack->GetObject() != nullptr) { // Если в стеке есть объект
				const ASandboxObject* DefaultObject = Cast<ASandboxObject>(Stack->GetObject()); // Приводим объект к типу ASandboxObject
				if (DefaultObject != nullptr) { // Если объект успешно приведен к типу ASandboxObject
					if (!DefaultObject->bStackable) { // Проверяем, является ли объект нестекуемым
						return TEXT(""); // Возвращаем пустую строку для нестекуемых объектов
					}
				}

				if (Stack->Amount > 0) { // Проверяем, есть ли объекты в стеке
					return FString::Printf(TEXT("%d"), Stack->Amount); // Возвращаем количество объектов в виде строки
				}
			}
		}
	}

	return TEXT(""); // Возвращаем пустую строку по умолчанию
}

// Функция, проверяющая, является ли ячейка внешней
bool USandboxObjectContainerCellWidget::IsExternal() {
	return CellBinding == EContainerCellBinding::ExternalObject; // Сравниваем привязку ячейки с внешним объектом
}

// Функция для получения контейнера, к которому относится ячейка
UContainerComponent* USandboxObjectContainerCellWidget::GetContainer() {
	if (IsExternal()) { // Если ячейка внешняя
		ASandboxPlayerController* SandboxPC = Cast<ASandboxPlayerController>(GetOwningPlayer()); // Получаем контроллер игрока
		if (SandboxPC) {
			return SandboxPC->GetOpenedContainer(); // Возвращаем открытый контейнер у контроллера игрока
		}
	} else { // Если ячейка не внешняя
		APawn* Pawn = GetOwningPlayer()->GetPawn(); // Получаем пешку игрока
		if (Pawn) {
			TArray<UContainerComponent*> Components; // Создаем массив для компонентов контейнера
			Pawn->GetComponents<UContainerComponent>(Components); // Получаем все компоненты контейнера у пешки

			for (UContainerComponent* Container : Components) { // Проходим по всем компонентам контейнера
				if (Container->GetName().Equals(ContainerName.ToString())) { // Проверяем имя компонента на совпадение с именем контейнера
					return Container; // Возвращаем найденный контейнер
				}
			}
		}
	}

	return nullptr; // Возвращаем nullptr, если контейнер не найден
}

// Функция для получения текстуры слота по его идентификатору
UTexture2D* USandboxObjectContainerCellWidget::GetSlotTexture(int32 SlotId) {
	
	UContainerComponent* Container = GetContainer(); // Получаем контейнер
	if (Container != nullptr) { // Если контейнер существует
		const FContainerStack* Stack = Container->GetSlot(SlotId); // Получаем стек из слота по его идентификатору
		if (Stack != nullptr) { // Если стек существует
			if (Stack->Amount > 0) { // Проверяем, есть ли объекты в стеке
				if (Stack->GetObject() != nullptr) { // Если в стеке есть объект
					const ASandboxObject* DefaultObject = Cast<ASandboxObject>(Stack->GetObject()); // Приводим объект к типу ASandboxObject
					if (DefaultObject != nullptr) { 
						return DefaultObject->IconTexture; // Возвращаем текстуру объекта как иконку слота 
					}
				}
			}
		}
	}
	
	return nullptr; // Возвращаем nullptr, если текстура не найдена 
}

// Функция для выбора слота по его идентификатору 
void USandboxObjectContainerCellWidget::SelectSlot(int32 SlotId) {
	UE_LOG(LogTemp, Log, TEXT("SelectSlot: %d"), SlotId); // Логируем выбор слота 
}

// Функция для обработки наведения на слот 
void USandboxObjectContainerCellWidget::HoverSlot(int32 SlotId) {

	ASandboxPlayerController* LocalController = Cast<ASandboxPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); // Получаем локальный контроллер игрока 
	if (LocalController) {
		int32 Sid = -1; 

		UContainerComponent* Container = GetContainer(); 
		if (Container != nullptr) {
			const FContainerStack* Stack = Container->GetSlot(SlotId); 
			if (Stack != nullptr && Stack->Amount > 0) { 
				Sid = SlotId; 
			}
		}

        LocalController->OnContainerSlotHover(Sid, *Container->GetName());  // Вызываем обработчик наведения на слот у контроллера игрока 
    } 
}

// Функция для обработки сброса объекта из одного слота в другой 
bool USandboxObjectContainerCellWidget::SlotDrop(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne) {
	bool bResult = SlotDropInternal(SlotDropId, SlotTargetId, SourceActor, SourceContainer, bOnlyOne); 
	if (bResult) {
        ASandboxPlayerController* LocalController = Cast<ASandboxPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); 
        if (LocalController && LocalController->GetNetMode() != NM_Client) { 
            const FString TargetContainerName = GetContainer()->GetName(); 
            const FString SourceContainerName = SourceContainer->GetName(); 
            LocalController->OnContainerDropSuccess(SlotTargetId, *SourceContainerName, *TargetContainerName); 
        }
    }

    return bResult; 
}

// Внутренняя функция обработки сброса объекта из одного слота в другой 
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

    return TargetContainer->SlotTransfer(SlotDropId, SlotTargetId, SourceActor, SourceContainer, bOnlyOne); 
}

// Функция проверки пустоты слота по его идентификатору  
bool USandboxObjectContainerCellWidget::SlotIsEmpty(int32 SlotId) {
    return false;  // По умолчанию возвращает false; реализация может быть изменена  
}

// Функция обработки основного действия по слоту  
void USandboxObjectContainerCellWidget::HandleSlotMainAction(int32 SlotId) {
    ASandboxPlayerController* SandboxPC = Cast<ASandboxPlayerController>(GetOwningPlayer());  
    if (SandboxPC) {  
        SandboxPC->OnContainerMainAction(SlotId, ContainerName);  // Вызываем основное действие у контроллера игрока  
    }
}

// Функция получения открытого объекта из ячейки  
AActor * USandboxObjectContainerCellWidget::GetOpenedObject() {
    if (IsExternal()) {  
        ASandboxPlayerController* SandboxPC = Cast<ASandboxPlayerController>(GetOwningPlayer());  
        if (SandboxPC != nullptr) {  
            return SandboxPC->GetOpenedObject();  // Возвращаем открытый объект у контроллера игрока  
        }
    } else {  
        return GetOwningPlayer()->GetPawn();  // Возвращаем пешку игрока  
    }

    return nullptr;  
}

// Функция получения открытого контейнера из ячейки  
UContainerComponent * USandboxObjectContainerCellWidget::GetOpenedContainer() {
    return GetContainer();  // Возвращаем текущий контейнер  
}
