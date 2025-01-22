// Copyright blackw 2015-2020

#include "ObjectContainerCellWidget.h"
#include "ContainerComponent.h"
#include "SandboxObject.h"
#include "SandboxPlayerController.h"

// Функция для получения цвета границы слота в зависимости от его идентификатора
FLinearColor USandboxObjectContainerCellWidget::SlotBorderColor(int32 SlotId) {
	if (ContainerName == TEXT("Inventory")) { 	//TODO исправить
		ASandboxPlayerController* PlayerController = Cast<ASandboxPlayerController>(GetOwningPlayer());
		if (PlayerController) {
			if (PlayerController->CurrentInventorySlot == SlotId) {
				return FLinearColor(0.1, 0.4, 1, 1);
		}
	}
}
	return FLinearColor(0, 0, 0, 0.5);
}

// Функция для получения текста количества объектов в слоте
FString USandboxObjectContainerCellWidget::SlotGetAmountText(int32 SlotId) {
	UContainerComponent* Container = GetContainer();
	if (Container != NULL) {
		const FContainerStack* Stack = Container->GetSlot(SlotId);
		if (Stack != NULL) {
			if (Stack->GetObject() != nullptr) {
				const ASandboxObject* DefaultObject = Cast<ASandboxObject>(Stack->GetObject());
				if (DefaultObject != nullptr) {
					if (!DefaultObject->bStackable) {
						return TEXT("");
					}
				}

				if (Stack->Amount > 0) {
					return FString::Printf(TEXT("%d"), Stack->Amount);
				}
			}
		}
	}

	return TEXT("");
}

// Функция для проверки является ли ячейка внешней
bool USandboxObjectContainerCellWidget::IsExternal() {
	return CellBinding == EContainerCellBinding::ExternalObject;
}

// Функция для получения контейнера
UContainerComponent* USandboxObjectContainerCellWidget::GetContainer() {
	if (IsExternal()) {
		ASandboxPlayerController* SandboxPC = Cast<ASandboxPlayerController>(GetOwningPlayer());
		if (SandboxPC) {
			return SandboxPC->GetOpenedContainer();
		}
	} else {
		APawn* Pawn = GetOwningPlayer()->GetPawn();
		if (Pawn) {
			TArray<UContainerComponent*> Components;
			Pawn->GetComponents<UContainerComponent>(Components);

			for (UContainerComponent* Container : Components) {
				if (Container->GetName().Equals(ContainerName.ToString())) {
					return Container;
				}
			}
		}
	}

	return nullptr;
}

// Функция для получения текстуры слота
UTexture2D* USandboxObjectContainerCellWidget::GetSlotTexture(int32 SlotId) {
	
	UContainerComponent* Container = GetContainer();
	if (Container != nullptr) {
		const FContainerStack* Stack = Container->GetSlot(SlotId);
		if (Stack != nullptr) {
			if (Stack->Amount > 0) {
				if (Stack->GetObject() != nullptr) {
					const ASandboxObject* DefaultObject = Cast<ASandboxObject>(Stack->GetObject());
					if (DefaultObject != nullptr) {
						return DefaultObject->IconTexture;
					}
				}
			}
		}
	}
	
	return nullptr;
}

// Функция для выбора слота
void USandboxObjectContainerCellWidget::SelectSlot(int32 SlotId) {
	UE_LOG(LogTemp, Log, TEXT("SelectSlot: %d"), SlotId);
}

// Функция для наведения на слот
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

		LocalController->OnContainerSlotHover(Sid, *Container->GetName());
	} 

}

// Функция для сброса слота
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

// Внутренняя функция для сброса слота
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

// Функция для проверки, пуст ли слот
bool USandboxObjectContainerCellWidget::SlotIsEmpty(int32 SlotId) {
	return false; // Здесь всегда возвращается false
}

// Функция для обработки основного действия с ячейкой
void USandboxObjectContainerCellWidget::HandleSlotMainAction(int32 SlotId) {
	ASandboxPlayerController* SandboxPC = Cast<ASandboxPlayerController>(GetOwningPlayer());
	if (SandboxPC) {
		SandboxPC->OnContainerMainAction(SlotId, ContainerName);
	}
}

// Функция для получения открытого объекта
AActor * USandboxObjectContainerCellWidget::GetOpenedObject() {
	if (IsExternal()) {
		ASandboxPlayerController* SandboxPC = Cast<ASandboxPlayerController>(GetOwningPlayer());
		if (SandboxPC != nullptr) {
			return SandboxPC->GetOpenedObject();
		}
	} else {
		return GetOwningPlayer()->GetPawn();
	}

	return nullptr;
}

// Функция для получения открытого контейнера
UContainerComponent * USandboxObjectContainerCellWidget::GetOpenedContainer() {
	return GetContainer();
}

