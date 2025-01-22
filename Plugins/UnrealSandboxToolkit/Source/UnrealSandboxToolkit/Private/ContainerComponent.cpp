// Copyright blackw 2015-2020

#include "ContainerComponent.h"
#include "SandboxObject.h"
#include "Net/UnrealNetwork.h"
#include "SandboxLevelController.h"
#include "SandboxPlayerController.h"
#include <algorithm>

// Возвращает объект песочницы по идентификатору класса песочницы
const ASandboxObject* FContainerStack::GetObject() const {
	return ASandboxLevelController::GetDefaultSandboxObject(SandboxClassId);
}

// Конструктор компонента контейнера
UContainerComponent::UContainerComponent() {
	//bWantsBeginPlay = true; // Включает начало игры для компонента
	PrimaryComponentTick.bCanEverTick = false; // Отключает возможность тика компонента
}

// Вызывается при начале игры
void UContainerComponent::BeginPlay() {
	Super::BeginPlay(); // Вызывает базовый класс
}

// Обновляет компонент
void UContainerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) {
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction ); // Вызывает базовый класс
}

/*
bool UContainerComponent::IsOwnerAdmin() {
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetOwner()); // Получает владельца как персонажа песочницы
	if (SandboxCharacter) {
		return SandboxCharacter->bIsAdmin; // Возвращает статус администратора
	}

	return true; // Если владелец не является персонажем, считается администратором
}
*/

// Проверяет, пуст ли контейнер
bool UContainerComponent::IsEmpty() const {
	if (Content.Num() == 0) {
		return true; // Контейнер пуст
	}

	for (int Idx = 0; Idx < Content.Num(); Idx++) {
		FContainerStack Stack = Content[Idx]; // Получает стек контейнера
		if (Stack.Amount > 0 ) {
			return false; // Контейнер не пуст
		}
	}

	return true; // Контейнер пуст
}

// Устанавливает стек напрямую по идентификатору слота
bool UContainerComponent::SetStackDirectly(const FContainerStack& Stack, const int SlotId) {
	if (SlotId >= Content.Num()) {
		Content.SetNum(SlotId + 1); // Увеличивает размер массива контента
	}

	FContainerStack* StackPtr = &Content[SlotId]; // Указатель на стек
	if (Stack.Amount > 0) {
		StackPtr->Amount = Stack.Amount; // Устанавливает количество
		StackPtr->SandboxClassId = Stack.SandboxClassId; // Устанавливает идентификатор класса песочницы
	} else {
		StackPtr->Clear(); // Очищает стек
	}

	MakeStats(); // Обновляет статистику
	bUpdated = true; // Устанавливает флаг обновления
	return true; // Успешно
}

// Добавляет объект в контейнер
bool UContainerComponent::AddObject(ASandboxObject* Obj) {
	if (Obj == nullptr) {
		return false; // Объект не может быть добавлен
	}

	uint32 MaxStackSize = Obj->GetMaxStackSize(); // Максимальный размер стека

	int FirstEmptySlot = -1; // Первый пустой слот
	bool bIsAdded = false; // Флаг добавления объекта
	for (int Idx = 0; Idx < Content.Num(); Idx++) {
		FContainerStack* Stack = &Content[Idx]; // Получает стек по индексу

		//TODO проверить максимальный объем и массу инвентаря
		if (Stack->Amount > 0) {
			if (Stack->SandboxClassId == Obj->GetSandboxClassId() && MaxStackSize > 1 && (uint64)Stack->Amount < MaxStackSize) {
				Stack->Amount++; // Увеличивает количество в стеке
				bIsAdded = true; // Объект добавлен
				break; // Выход из цикла
			}
		} else {
			if (FirstEmptySlot < 0) {
				FirstEmptySlot = Idx; // Запоминает первый пустой слот
				if (MaxStackSize == 1) {
					break; // Если максимальный размер стека 1, выходим
				}
			}
		}
	}

	if (!bIsAdded) {
		if (FirstEmptySlot >= 0) {
			FContainerStack* Stack = &Content[FirstEmptySlot]; // Получает стек по пустому слоту
			Stack->Amount = 1; // Устанавливает количество в 1
			Stack->SandboxClassId = Obj->GetSandboxClassId(); // Устанавливает идентификатор класса песочницы
		} else {
			//UE_LOG(LogTemp, Warning, TEXT("AddObject -> Content.Num() -> MaxCapacity: %d %d"), Content.Num(), MaxCapacity);
			if (Content.Num() < MaxCapacity) {
				FContainerStack NewStack; // Новый стек
				NewStack.Amount = 1; // Устанавливает количество в 1
				NewStack.SandboxClassId = Obj->GetSandboxClassId(); // Устанавливает идентификатор класса песочницы
				Content.Add(NewStack); // Добавляет новый стек в контейнер
			} else {
				return false; // Контейнер переполнен
			}
		}
	}
	
	MakeStats(); // Обновляет статистику
	bUpdated = true; // Устанавливает флаг обновления
	return true; // Успешно
}

// Получает стек по идентификатору слота
FContainerStack* UContainerComponent::GetSlot(const int Slot) {
	if (!Content.IsValidIndex(Slot)) {
		return nullptr; // Если индекс не валиден, возвращает nullptr
	}

	return &Content[Slot]; // Возвращает стек
}

// Получает стек по идентификатору слота (константный метод)
const FContainerStack* UContainerComponent::GetSlot(const int Slot) const {
	if (!Content.IsValidIndex(Slot)) {
		return nullptr; // Если индекс не валиден, возвращает nullptr
	}

	return &Content[Slot]; // Возвращает стек
}

/*
ASandboxObject* UContainerComponent::GetAvailableSlotObject(const int Slot) {
	if (!Content.IsValidIndex(Slot)) {
		return nullptr; // Если индекс не валиден, возвращает nullptr
	}

	FContainerStack* Stack = &Content[Slot]; // Получает стек по индексу
	if (Stack->Amount > 0) {
		TSubclassOf<ASandboxObject>	ObjectClass = Stack->ObjectClass; // Получает класс объекта
		if (ObjectClass) {
			return (ASandboxObject*)(ObjectClass->GetDefaultObject()); // Возвращает объект по классу
		}
	}

	return nullptr; // Если объект не найден, возвращает nullptr
}
*/

// Уменьшает количество объектов в контейнере
bool UContainerComponent::DecreaseObjectsInContainer(int Slot, int Num) {
	FContainerStack* Stack = GetSlot(Slot); // Получает стек по слоту

	if (Stack == NULL) { 
		return false; // Если стек не найден, возвращает false
	}

	if (Stack->Amount > 0) {
		Stack->Amount -= Num; // Уменьшает количество
		if (Stack->Amount == 0) { 
			Stack->Clear(); // Очищает стек, если он пуст
		}
	}

	MakeStats(); // Обновляет статистику
	bUpdated = true; // Устанавливает флаг обновления
	return Stack->Amount > 0; // Возвращает, осталось ли количество объектов больше 0
}

// Изменяет количество объектов в слоте
void UContainerComponent::ChangeAmount(int Slot, int Num) {
	FContainerStack* Stack = GetSlot(Slot); // Получает стек по слоту

	//TODO проверить размер стека
	if (Stack) {
		if (Stack->Amount > 0) {
			Stack->Amount += Num; // Увеличивает или уменьшает количество
			if (Stack->Amount == 0) {
				Stack->Clear(); // Очищает стек, если он пуст
			}

			MakeStats(); // Обновляет статистику
			bUpdated = true; // Устанавливает флаг обновления
		}
	}
}

// Проверяет, пуст ли слот
bool UContainerComponent::IsSlotEmpty(int SlotId) const {
	const FContainerStack* Stack = GetSlot(SlotId); // Получает стек по слоту
	if (Stack) {
		return Stack->IsEmpty(); // Возвращает статус пустоты стека
	}

	return true; // Если стек не найден, считается пустым
}

// Получает свойства, которые нужно реплицировать
void UContainerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps); // Вызывает базовый класс
	DOREPLIFETIME(UContainerComponent, Content); // Указывает, что контент должен реплицироваться
}

// Копирует содержимое в другой контейнер
void UContainerComponent::CopyTo(UContainerComponent* Target) {
	Target->Content = this->Content; // Копирует контент
	bUpdated = true; // Устанавливает флаг обновления
	MakeStats(); // Обновляет статистику
}

// Получает все объекты в контейнере
TArray<uint64> UContainerComponent::GetAllObjects() const {
	TArray<uint64> Result; // Массив для хранения результатов
	for (int Idx = 0; Idx < Content.Num(); Idx++) {
		const FContainerStack* Stack = &Content[Idx]; // Получает стек по индексу
		if (Stack) {
			const ASandboxObject* Obj = Stack->GetObject(); // Получает объект
			if (Obj) {
				Result.Add(Obj->GetSandboxClassId()); // Добавляет идентификатор класса объекта в результат
			}
		}
	}

	return Result; // Возвращает результат
}

// Сравнивает два объекта
bool IsSameObject(const FContainerStack* StackSourcePtr, const FContainerStack* StackTargetPtr) {
	if (StackSourcePtr && StackTargetPtr) {
		const auto* SourceObj = StackSourcePtr->GetObject(); // Получает объект из исходного стека
		const auto* TargetObj = StackTargetPtr->GetObject(); // Получает объект из целевого стека
		if (SourceObj && TargetObj) {
			return SourceObj->GetSandboxClassId() == TargetObj->GetSandboxClassId(); // Сравнивает идентификаторы классов
		}
	}

	return false; // Если один из объектов не найден, возвращает false
}

// Передает объект по сети
void NetObjectTransfer(ASandboxPlayerController* LocalController, const ASandboxObject* RemoteObj, const UContainerComponent* RemoteContainer, const FContainerStack Stack, int32 SlotId) {
	const FString TargetContainerName = RemoteContainer->GetName(); // Получает имя целевого контейнера
	LocalController->TransferContainerStack(RemoteObj->GetSandboxNetUid(), TargetContainerName, Stack, SlotId); // Передает стек
}

// Передает контроллер по сети
void NetControllerTransfer(ASandboxPlayerController* LocalController, const UContainerComponent* RemoteContainer, const FContainerStack Stack, int32 SlotId) {
	const FString TargetContainerName = RemoteContainer->GetName(); // Получает имя целевого контейнера
	LocalController->TransferInventoryStack(TargetContainerName, Stack, SlotId); // Передает инвентарный стек
}

// Передает стек из одного слота в другой
bool UContainerComponent::SlotTransfer(int32 SlotSourceId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne) {
		UContainerComponent* TargetContainer = this; // Целевой контейнер
		const FContainerStack* StackSourcePtr = SourceContainer->GetSlot(SlotSourceId); // Получает стек из исходного контейнера
		const FContainerStack* StackTargetPtr = TargetContainer->GetSlot(SlotTargetId); // Получает стек из целевого контейнера

		bool bInternalTransfer = (TargetContainer == SourceContainer); // Проверяет, является ли передача внутренней

		FContainerStack NewSourceStack; // Новый стек для источника
		FContainerStack NewTargetStack; // Новый стек для цели

		if (StackTargetPtr) {
			NewTargetStack = *StackTargetPtr; // Копирует стек цели
		}

		if (StackSourcePtr) {
			NewSourceStack = *StackSourcePtr; // Копирует стек источника
		}

		bool bResult = false; // Результат операции

		APawn* LocalPawn = (APawn*)TargetContainer->GetOwner(); // Получает владельца контейнера
		if (LocalPawn) {
			ASandboxPlayerController* LocalController = Cast<ASandboxPlayerController>(LocalPawn->GetController()); // Получает контроллер
			if (LocalController) {
				const FString ContainerName = GetName(); // Получает имя контейнера
				const ASandboxObject* Obj = StackSourcePtr->GetObject(); // Получает объект из источника
				bool isValid = LocalController->OnContainerDropCheck(SlotTargetId, *ContainerName, Obj); // Проверяет возможность передачи
				if (!isValid) {
					return false; // Если передача невозможна, возвращает false
				}
			}
		}

		if (IsSameObject(StackSourcePtr, StackTargetPtr)) { // Если объекты одинаковые
			const ASandboxObject* Obj = (ASandboxObject*)StackTargetPtr->GetObject(); // Получает объект из целевого стека
			if (StackTargetPtr->Amount < (int)Obj->MaxStackSize) { // Если целевой стек не полон
				uint32 ChangeAmount = (bOnlyOne) ? 1 : StackSourcePtr->Amount; // Определяет количество для изменения
				uint32 NewAmount = StackTargetPtr->Amount + ChangeAmount; // Новое количество в целевом стеке

				if (NewAmount <= Obj->MaxStackSize) { // Если новое количество не превышает максимум
					NewTargetStack.Amount = NewAmount; // Устанавливает новое количество в целевом стеке
					NewSourceStack.Amount -= ChangeAmount; // Уменьшает количество в источнике
				} else {
					int D = NewAmount - Obj->MaxStackSize; // Вычисляет избыток
					NewTargetStack.Amount = Obj->MaxStackSize; // Устанавливает максимум в целевом стеке
					NewSourceStack.Amount = D; // Устанавливает избыток в источнике
				}

				bResult = true; // Операция успешна
			}
		} else {
			if (bOnlyOne) { // Если передаем только один объект
				if (!SourceContainer->IsSlotEmpty(SlotSourceId)) { // Если исходный слот не пуст
					if (TargetContainer->IsSlotEmpty(SlotTargetId)) { // Если целевой слот пуст
						NewTargetStack = NewSourceStack; // Копирует стек источника
						NewTargetStack.Amount = 1; // Устанавливает количество в 1
						NewSourceStack.Amount--; // Уменьшает количество в источнике
						bResult = true; // Операция успешна
					}
				}
			} else {
				std::swap(NewTargetStack, NewSourceStack); // Меняет местами стеки
				bResult = true; // Операция успешна
			}
		}

		if (bResult) {
			if (GetNetMode() == NM_Client) { // Если клиент
				ASandboxPlayerController* LocalController = Cast<ASandboxPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); // Получает контроллер игрока
				if (LocalController) {
					if (bInternalTransfer) {
						//TargetContainer->SetStackDirectly(NewTargetStack, SlotTargetId); // Устанавливает стек напрямую
						//SourceContainer->SetStackDirectly(NewSourceStack, SlotSourceId); // Устанавливает стек напрямую
					} 

					ASandboxObject* TargetObj = Cast<ASandboxObject>(TargetContainer->GetOwner()); // Получает объект цели
					if (TargetObj) {
						NetObjectTransfer(LocalController, TargetObj, TargetContainer, NewTargetStack, SlotTargetId); // Передает объект по сети
					}

					ASandboxObject* SourceObj = Cast<ASandboxObject>(SourceContainer->GetOwner()); // Получает объект источника
					if (SourceObj) {
						NetObjectTransfer(LocalController, SourceObj, SourceContainer, NewSourceStack, SlotSourceId); // Передает объект по сети
					}

					APawn* TargetPawn = Cast<APawn>(TargetContainer->GetOwner()); // Получает пехоту цели
					if (TargetPawn) {
						NetControllerTransfer(LocalController, TargetContainer, NewTargetStack, SlotTargetId); // Передает контроллер по сети
					}

					APawn* SourcePawn = Cast<APawn>(SourceContainer->GetOwner()); // Получает пехоту источника
					if (SourcePawn) {
						NetControllerTransfer(LocalController, SourceContainer, NewSourceStack, SlotSourceId); // Передает контроллер по сети
					}
				}
			} else {
				// только сервер
				TargetContainer->SetStackDirectly(NewTargetStack, SlotTargetId); // Устанавливает стек напрямую в целевом контейнере
				SourceContainer->SetStackDirectly(NewSourceStack, SlotSourceId); // Устанавливает стек напрямую в источнике
			}
		}

		bUpdated = bResult; // Устанавливает флаг обновления
		MakeStats(); // Обновляет статистику
		return bResult; // Возвращает результат операции
}

// Вызывается при репликации содержимого
void UContainerComponent::OnRep_Content() {
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Content: %s"), *GetName()); // Логирует событие репликации
	bUpdated = true; // Устанавливает флаг обновления
	MakeStats(); // Обновляет статистику
}

// Проверяет, обновлен ли компонент
bool UContainerComponent::IsUpdated() {
	return bUpdated; // Возвращает статус обновления
}

// Сбрасывает флаг обновления
void UContainerComponent::ResetUpdatedFlag() {
	bUpdated = false; // Устанавливает флаг обновления в false
}

// Обновляет статистику инвентаря
void UContainerComponent::MakeStats() {
	InventoryStats.Empty(); // Очищает статистику

	for (const auto& Stack : Content) {
		if (Stack.Amount > 0 && Stack.SandboxClassId > 0) {
			auto T = InventoryStats.FindOrAdd(Stack.SandboxClassId) + Stack.Amount; // Обновляет статистику по классу
			InventoryStats[Stack.SandboxClassId] = T; // Устанавливает новое значение
		}
	}
}

// Возвращает статистику инвентаря
const TMap<uint64, uint32>& UContainerComponent::GetStats() const {
	return InventoryStats; // Возвращает статистику
}

// Возвращает содержимое контейнера
const TArray<FContainerStack>& UContainerComponent::GetContent() {
	return Content; // Возвращает массив контента
}
