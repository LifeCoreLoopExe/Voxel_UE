// Copyright blackw 2015-2020


#include "ContainerComponent.h"
#include "SandboxObject.h"
#include "Net/UnrealNetwork.h"
#include "SandboxLevelController.h"
#include "SandboxPlayerController.h"
#include <algorithm>

// Получает объект из стека контейнера по его ID
const ASandboxObject* FContainerStack::GetObject() const {
	return ASandboxLevelController::GetDefaultSandboxObject(SandboxClassId);
}

// Конструктор компонента контейнера
// Отключает тик компонента, так как он не нужен для логики контейнера
UContainerComponent::UContainerComponent() {
	//bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = false;
}

// Функция вызывается при начале игры
void UContainerComponent::BeginPlay() {
	Super::BeginPlay();
}

// Функция тика компонента (отключена, так как не используется)
void UContainerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) {
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

/*
bool UContainerComponent::IsOwnerAdmin() {
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetOwner());
	if (SandboxCharacter) {
		return SandboxCharacter->bIsAdmin;
	}

	return true;
}
*/

// Проверяет, пуст ли контейнер
// Контейнер считается пустым, если в нем нет предметов или все стеки пустые
bool UContainerComponent::IsEmpty() const {
	if (Content.Num() == 0) {
		return true;
	}

	for (int Idx = 0; Idx < Content.Num(); Idx++) {
		FContainerStack Stack = Content[Idx];
		if (Stack.Amount > 0 ) {
			return false;
		}
	}

	return true;
}

// Напрямую устанавливает стек предметов в определенный слот
// Используется для синхронизации содержимого контейнера
bool UContainerComponent::SetStackDirectly(const FContainerStack& Stack, const int SlotId) {
	// Увеличиваем размер массива, если слот больше текущего размера
	if (SlotId >= Content.Num()) {
		Content.SetNum(SlotId + 1);
	}

	FContainerStack* StackPtr = &Content[SlotId];
	if (Stack.Amount > 0) {
		StackPtr->Amount = Stack.Amount;
		StackPtr->SandboxClassId = Stack.SandboxClassId;
	} else {
		StackPtr->Clear();
	}

	MakeStats();
	bUpdated = true;
	return true;
}

// Добавляет объект в контейнер
// Пытается сложить с существующим стеком или найти пустой слот
bool UContainerComponent::AddObject(ASandboxObject* Obj) {
	if (Obj == nullptr) {
		return false;
	}

	uint32 MaxStackSize = Obj->GetMaxStackSize();

	// Ищем существующий стек того же типа или первый пустой слот
	int FirstEmptySlot = -1;
	bool bIsAdded = false;
	for (int Idx = 0; Idx < Content.Num(); Idx++) {
		FContainerStack* Stack = &Content[Idx];

		//TODO check inventory max volume and mass
		if (Stack->Amount > 0) {
			if (Stack->SandboxClassId == Obj->GetSandboxClassId() && MaxStackSize > 1 && (uint64)Stack->Amount < MaxStackSize) {
				Stack->Amount++;
				bIsAdded = true;
				break;
			}
		} else {
			if (FirstEmptySlot < 0) {
				FirstEmptySlot = Idx;
				if (MaxStackSize == 1) {
					break;
				}
			}
		}
	}

	if (!bIsAdded) {
		if (FirstEmptySlot >= 0) {
			FContainerStack* Stack = &Content[FirstEmptySlot];
			Stack->Amount = 1;
			Stack->SandboxClassId = Obj->GetSandboxClassId();
		} else {
			//UE_LOG(LogTemp, Warning, TEXT("AddObject -> Content.Num() -> MaxCapacity: %d %d"), Content.Num(), MaxCapacity);
			if (Content.Num() < MaxCapacity) {
				FContainerStack NewStack;
				NewStack.Amount = 1;
				NewStack.SandboxClassId = Obj->GetSandboxClassId();
				Content.Add(NewStack);
			} else {
				return false;
			}
		}
	}
	
	MakeStats();
	bUpdated = true;
	return true;
}

// Получает указатель на стек предметов в указанном слоте
// Возвращает nullptr если слот невалидный
FContainerStack* UContainerComponent::GetSlot(const int Slot) {
	if (!Content.IsValidIndex(Slot)) {
		return nullptr;
	}

	return &Content[Slot];
}

const FContainerStack* UContainerComponent::GetSlot(const int Slot) const {
	if (!Content.IsValidIndex(Slot)) {
		return nullptr;
	}

	return &Content[Slot];
}

/*
ASandboxObject* UContainerComponent::GetAvailableSlotObject(const int Slot) {
	if (!Content.IsValidIndex(Slot)) {
		return nullptr;
	}

	FContainerStack* Stack = &Content[Slot];
	if (Stack->Amount > 0) {
		TSubclassOf<ASandboxObject>	ObjectClass = Stack->ObjectClass;
		if (ObjectClass) {
			return (ASandboxObject*)(ObjectClass->GetDefaultObject());
		}
	}

	return nullptr;
}
*/

// Уменьшает количество предметов в указанном слоте на заданное число
// Возвращает true если в стеке остались предметы, false если стек стал пустым
bool UContainerComponent::DecreaseObjectsInContainer(int Slot, int Num) {
	FContainerStack* Stack = GetSlot(Slot);

	if (Stack == NULL) { 
		return false;
	}

	if (Stack->Amount > 0) {
		Stack->Amount -= Num;
		if (Stack->Amount == 0) { 
			Stack->Clear();
		}
	}

	MakeStats();
	bUpdated = true;
	return Stack->Amount > 0;
}

// Изменяет количество предметов в стеке на указанное число
// Может как увеличивать, так и уменьшать количество
void UContainerComponent::ChangeAmount(int Slot, int Num) {
	FContainerStack* Stack = GetSlot(Slot);

	if (Stack) {
		if (Stack->Amount > 0) {
			Stack->Amount += Num;
			if (Stack->Amount == 0) {
				Stack->Clear();
			}

			MakeStats();
			bUpdated = true;
		}
	}
}

// Проверяет, пуст ли указанный слот
bool UContainerComponent::IsSlotEmpty(int SlotId) const {
	const FContainerStack* Stack = GetSlot(SlotId);
	if (Stack) {
		return Stack->IsEmpty();
	}

	return true;
}

// Настройка сетевой репликации для массива Content
void UContainerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UContainerComponent, Content);
}

// Копирует содержимое текущего контейнера в целевой
void UContainerComponent::CopyTo(UContainerComponent* Target) {
	Target->Content = this->Content;
	bUpdated = true;
	MakeStats();
}

// Возвращает массив ID всех объектов в контейнере
TArray<uint64> UContainerComponent::GetAllObjects() const {
	TArray<uint64> Result;
	for (int Idx = 0; Idx < Content.Num(); Idx++) {
		const FContainerStack* Stack = &Content[Idx];
		if (Stack) {
			const ASandboxObject* Obj = Stack->GetObject();
			if (Obj) {
				Result.Add(Obj->GetSandboxClassId());
			}
		}
	}

	return Result;
}

// Проверяет, являются ли два стека одинаковыми предметами
bool IsSameObject(const FContainerStack* StackSourcePtr, const FContainerStack* StackTargetPtr) {
	if (StackSourcePtr && StackTargetPtr) {
		const auto* SourceObj = StackSourcePtr->GetObject();
		const auto* TargetObj = StackTargetPtr->GetObject();
		if (SourceObj && TargetObj) {
			return SourceObj->GetSandboxClassId() == TargetObj->GetSandboxClassId();
		}
	}

	return false;
}

void NetObjectTransfer(ASandboxPlayerController* LocalController, const ASandboxObject* RemoteObj, const UContainerComponent* RemoteContainer, const FContainerStack Stack, int32 SlotId) {
	const FString TargetContainerName = RemoteContainer->GetName();
	LocalController->TransferContainerStack(RemoteObj->GetSandboxNetUid(), TargetContainerName, Stack, SlotId);
}

void NetControllerTransfer(ASandboxPlayerController* LocalController, const UContainerComponent* RemoteContainer, const FContainerStack Stack, int32 SlotId) {
	const FString TargetContainerName = RemoteContainer->GetName();
	LocalController->TransferInventoryStack(TargetContainerName, Stack, SlotId);
}

bool UContainerComponent::SlotTransfer(int32 SlotSourceId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne) {
		UContainerComponent* TargetContainer = this;
		const FContainerStack* StackSourcePtr = SourceContainer->GetSlot(SlotSourceId);
		const FContainerStack* StackTargetPtr = TargetContainer->GetSlot(SlotTargetId);

		bool bInternalTransfer = (TargetContainer == SourceContainer);

		FContainerStack NewSourceStack;
		FContainerStack NewTargetStack;

		if (StackTargetPtr) {
			NewTargetStack = *StackTargetPtr;
		}

		if (StackSourcePtr) {
			NewSourceStack = *StackSourcePtr;
		}

		bool bResult = false;

		APawn* LocalPawn = (APawn*)TargetContainer->GetOwner();
		if (LocalPawn) {
			ASandboxPlayerController* LocalController = Cast<ASandboxPlayerController>(LocalPawn->GetController());
			if (LocalController) {
				const FString ContainerName = GetName();
				const ASandboxObject* Obj = StackSourcePtr->GetObject();
				bool isValid = LocalController->OnContainerDropCheck(SlotTargetId, *ContainerName, Obj);
				if (!isValid) {
					return false;
				}
			}
		}

		if (IsSameObject(StackSourcePtr, StackTargetPtr)) {
			const ASandboxObject* Obj = (ASandboxObject*)StackTargetPtr->GetObject();
			if (StackTargetPtr->Amount < (int)Obj->MaxStackSize) {
				uint32 ChangeAmount = (bOnlyOne) ? 1 : StackSourcePtr->Amount;
				uint32 NewAmount = StackTargetPtr->Amount + ChangeAmount;

				if (NewAmount <= Obj->MaxStackSize) {
					NewTargetStack.Amount = NewAmount;
					NewSourceStack.Amount -= ChangeAmount;
				} else {
					int D = NewAmount - Obj->MaxStackSize;
					NewTargetStack.Amount = Obj->MaxStackSize;
					NewSourceStack.Amount = D;
				}

				bResult = true;
			}
		} else {
			if (bOnlyOne) {
				if (!SourceContainer->IsSlotEmpty(SlotSourceId)) {
					if (TargetContainer->IsSlotEmpty(SlotTargetId)) {
						NewTargetStack = NewSourceStack;
						NewTargetStack.Amount = 1;
						NewSourceStack.Amount--;
						bResult = true;
					}
				}
			} else {
				std::swap(NewTargetStack, NewSourceStack);
				bResult = true;
			}
		}

		if (bResult) {
			if (GetNetMode() == NM_Client) {
				ASandboxPlayerController* LocalController = Cast<ASandboxPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
				if (LocalController) {
					if (bInternalTransfer) {
						//TargetContainer->SetStackDirectly(NewTargetStack, SlotTargetId);
						//SourceContainer->SetStackDirectly(NewSourceStack, SlotSourceId);
					} 

					ASandboxObject* TargetObj = Cast<ASandboxObject>(TargetContainer->GetOwner());
					if (TargetObj) {
						NetObjectTransfer(LocalController, TargetObj, TargetContainer, NewTargetStack, SlotTargetId);
					}

					ASandboxObject* SourceObj = Cast<ASandboxObject>(SourceContainer->GetOwner());
					if (SourceObj) {
						NetObjectTransfer(LocalController, SourceObj, SourceContainer, NewSourceStack, SlotSourceId);
					}

					APawn* TargetPawn = Cast<APawn>(TargetContainer->GetOwner());
					if (TargetPawn) {
						NetControllerTransfer(LocalController, TargetContainer, NewTargetStack, SlotTargetId);
					}

					APawn* SourcePawn = Cast<APawn>(SourceContainer->GetOwner());
					if (SourcePawn) {
						NetControllerTransfer(LocalController, SourceContainer, NewSourceStack, SlotSourceId);
					}
				}
			} else {
				// server only
				TargetContainer->SetStackDirectly(NewTargetStack, SlotTargetId);
				SourceContainer->SetStackDirectly(NewSourceStack, SlotSourceId);
			}
		}

		bUpdated = bResult;
		MakeStats();
		return bResult;
}

// Вызывается при обновлении содержимого контейнера по сети
void UContainerComponent::OnRep_Content() {
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Content: %s"), *GetName());
	bUpdated = true;
	MakeStats();
}

// Проверяет, было ли обновлено содержимое контейнера
bool UContainerComponent::IsUpdated() {
	return bUpdated;
}

// Сбрасывает флаг обновления
void UContainerComponent::ResetUpdatedFlag() {
	bUpdated = false;
}

// MakeStats() пересчитывает статистику по всем предметам в контейнере
void UContainerComponent::MakeStats() {
	InventoryStats.Empty();

	for (const auto& Stack : Content) {
		if (Stack.Amount > 0 && Stack.SandboxClassId > 0) {
			auto T = InventoryStats.FindOrAdd(Stack.SandboxClassId) + Stack.Amount;
			InventoryStats[Stack.SandboxClassId] = T;
		}
	}
}

// Возвращает статистику по всем предметам в контейнере
const TMap<uint64, uint32>& UContainerComponent::GetStats() const {
	return InventoryStats;
}

// Возвращает прямой доступ к содержимому контейнера
const TArray<FContainerStack>& UContainerComponent::GetContent() {
	return Content;
}