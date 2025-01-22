// Copyright blackw 2015-2020

#include "ContainerComponent.h"
#include "SandboxObject.h"
#include "Net/UnrealNetwork.h"
#include "SandboxLevelController.h"
#include "SandboxPlayerController.h"
#include <algorithm>

// Функция для получения объекта, связанного с контейнером
const ASandboxObject* FContainerStack::GetObject() const {
    return ASandboxLevelController::GetDefaultSandboxObject(SandboxClassId); // Получить объект по умолчанию для данного класса
}

// Конструктор для UContainerComponent
UContainerComponent::UContainerComponent() {
    //bWantsBeginPlay = true; // Раскомментируйте, если хотите, чтобы компонент получал события BeginPlay
    PrimaryComponentTick.bCanEverTick = false; // Отключить тики для этого компонента
}

// Вызывается, когда игра начинается или когда объект создается
void UContainerComponent::BeginPlay() {
    Super::BeginPlay(); // Вызвать функцию BeginPlay родительского класса
}

// Вызывается каждый кадр, если компонент настроен на тики
void UContainerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction); // Вызвать функцию TickComponent родительского класса
}



// Проверка, пуст ли контейнер
bool UContainerComponent::IsEmpty() const {
    if (Content.Num() == 0) { // Если в контейнере нет предметов
        return true;
    }

    for (int Idx = 0; Idx < Content.Num(); Idx++) { // Перебрать все предметы
        FContainerStack Stack = Content[Idx];
        if (Stack.Amount > 0) { // Если у какого-либо предмета количество больше нуля
            return false;
        }
    }

    return true; // Если у всех предметов количество равно нулю
}

// Установить стек непосредственно в контейнере
bool UContainerComponent::SetStackDirectly(const FContainerStack& Stack, const int SlotId) {
    if (SlotId >= Content.Num()) { // Если индекс слота больше текущего размера
        Content.SetNum(SlotId + 1); // Изменить размер массива содержимого
    }

    FContainerStack* StackPtr = &Content[SlotId]; // Получить указатель на стек
    if (Stack.Amount > 0) { // Если в стеке есть предметы
        StackPtr->Amount = Stack.Amount; // Установить количество
        StackPtr->SandboxClassId = Stack.SandboxClassId; // Установить идентификатор класса
    } else {
        StackPtr->Clear(); // Очистить стек, если в нем нет предметов
    }

    MakeStats(); // Обновить статистику инвентаря
    bUpdated = true; // Отметить контейнер как обновленный
    return true;
}

// Добавить объект в контейнер
bool UContainerComponent::AddObject(ASandboxObject* Obj) {
    if (Obj == nullptr) { // Если объект равен null
        return false;
    }

    uint32 MaxStackSize = Obj->GetMaxStackSize(); // Получить максимальный размер стека для объекта

    int FirstEmptySlot = -1;
    bool bIsAdded = false;
    for (int Idx = 0; Idx < Content.Num(); Idx++) { // Перебрать все предметы
        FContainerStack* Stack = &Content[Idx];

        //TODO проверить максимальный объем и массу инвентаря
        if (Stack->Amount > 0) { // Если в стеке есть предметы
            if (Stack->SandboxClassId == Obj->GetSandboxClassId() && MaxStackSize > 1 && (uint64)Stack->Amount < MaxStackSize) { // Если стек может содержать больше предметов
                Stack->Amount++; // Увеличить количество
                bIsAdded = true;
                break;
            }
        } else { // Если стек пуст
            if (FirstEmptySlot < 0) { // Если пустой слот еще не найден
                FirstEmptySlot = Idx; // Отметить этот слот как первый пустой слот
                if (MaxStackSize == 1) { // Если максимальный размер стека равен 1
                    break;
                }
            }
        }
    }

    if (!bIsAdded) { // Если объект не был добавлен в существующий стек
        if (FirstEmptySlot >= 0) { // Если найден пустой слот
            FContainerStack* Stack = &Content[FirstEmptySlot]; // Получить указатель на стек
            Stack->Amount = 1; // Установить количество равным 1
            Stack->SandboxClassId = Obj->GetSandboxClassId(); // Установить идентификатор класса
        } else {
            //UE_LOG(LogTemp, Warning, TEXT("AddObject -> Content.Num() -> MaxCapacity: %d %d"), Content.Num(), MaxCapacity);
            if (Content.Num() < MaxCapacity) { // Если контейнер имеет свободное место
                FContainerStack NewStack; // Создать новый стек
                NewStack.Amount = 1; // Установить количество равным 1
                NewStack.SandboxClassId = Obj->GetSandboxClassId(); // Установить идентификатор класса
                Content.Add(NewStack); // Добавить новый стек в содержимое
            } else {
                return false; // Если контейнер заполнен
            }
        }
    }

    MakeStats(); // Обновить статистику инвентаря
    bUpdated = true; // Отметить контейнер как обновленный
    return true;
}

// Получить стек из контейнера
FContainerStack* UContainerComponent::GetSlot(const int Slot) {
    if (!Content.IsValidIndex(Slot)) { // Если слот недействителен
        return nullptr;
    }

    return &Content[Slot]; // Вернуть указатель на стек
}

// Получить стек из контейнера (константная версия)
const FContainerStack* UContainerComponent::GetSlot(const int Slot) const {
    if (!Content.IsValidIndex(Slot)) { // Если слот недействителен
        return nullptr;
    }

    return &Content[Slot]; // Вернуть указатель на стек
}
// Уменьшить количество объектов в слоте контейнера
bool UContainerComponent::DecreaseObjectsInContainer(int Slot, int Num) {
    FContainerStack* Stack = GetSlot(Slot); // Получить указатель на стек

    if (Stack == NULL) { // Если стек равен NULL
        return false;
    }

    if (Stack->Amount > 0) { // Если в стеке есть предметы
        Stack->Amount -= Num; // Уменьшить количество
        if (Stack->Amount == 0) { // Если количество равно нулю
            Stack->Clear(); // Очистить стек
        }
    }

    MakeStats(); // Обновить статистику инвентаря
    bUpdated = true; // Отметить контейнер как обновленный
    return Stack->Amount > 0; // Вернуть, есть ли еще предметы в стеке
}

// Изменить количество объектов в слоте контейнера
void UContainerComponent::ChangeAmount(int Slot, int Num) {
    FContainerStack* Stack = GetSlot(Slot); // Получить указатель на стек

    //TODO проверить размер стека
    if (Stack) { // Если стек действителен
        if (Stack->Amount > 0) { // Если в стеке есть предметы
            Stack->Amount += Num; // Изменить количество
            if (Stack->Amount == 0) { // Если количество равно нулю
                Stack->Clear(); // Очистить стек
            }

            MakeStats(); // Обновить статистику инвентаря
            bUpdated = true; // Отметить контейнер как обновленный
        }
    }
}

// Проверка, пуст ли слот
bool UContainerComponent::IsSlotEmpty(int SlotId) const {
    const FContainerStack* Stack = GetSlot(SlotId); // Получить указатель на стек
    if (Stack) { // Если стек действителен
        return Stack->IsEmpty(); // Вернуть, пуст ли стек
    }

    return true; // По умолчанию вернуть true, если стек недействителен
}

// Репликация свойств для синхронизации по сети
void UContainerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps); // Вызвать функцию родительского класса
    DOREPLIFETIME(UContainerComponent, Content); // Реплицировать массив содержимого
}

// Скопировать содержимое в другой контейнер
void UContainerComponent::CopyTo(UContainerComponent* Target) {
    Target->Content = this->Content; // Скопировать массив содержимого
    bUpdated = true; // Отметить контейнер как обновленный
    MakeStats(); // Обновить статистику инвентаря
}

// Получить все объекты в контейнере
TArray<uint64> UContainerComponent::GetAllObjects() const {
    TArray<uint64> Result; // Создать массив результатов
    for (int Idx = 0; Idx < Content.Num(); Idx++) { // Перебрать все предметы
        const FContainerStack* Stack = &Content[Idx];
        if (Stack) { // Если стек действителен
            const ASandboxObject* Obj = Stack->GetObject(); // Получить объект
            if (Obj) { // Если объект действителен
                Result.Add(Obj->GetSandboxClassId()); // Добавить идентификатор класса в результат
            }
        }
    }

    return Result; // Вернуть массив результатов
}

// Проверка, содержат ли два стека один и тот же объект
bool IsSameObject(const FContainerStack* StackSourcePtr, const FContainerStack* StackTargetPtr) {
    if (StackSourcePtr && StackTargetPtr) { // Если оба стека действительны
        const auto* SourceObj = StackSourcePtr->GetObject(); // Получить исходный объект
        const auto* TargetObj = StackTargetPtr->GetObject(); // Получить целевой объект
        if (SourceObj && TargetObj) { // Если оба объекта действительны
            return SourceObj->GetSandboxClassId() == TargetObj->GetSandboxClassId(); // Вернуть, совпадают ли идентификаторы классов
        }
    }

    return false; // По умолчанию вернуть false, если стеки или объекты недействительны
}

// Передача стека между контейнерами по сети
void NetObjectTransfer(ASandboxPlayerController* LocalController, const ASandboxObject* RemoteObj, const UContainerComponent* RemoteContainer, const FContainerStack Stack, int32 SlotId) {
    const FString TargetContainerName = RemoteContainer->GetName(); // Получить имя целевого контейнера
    LocalController->TransferContainerStack(RemoteObj->GetSandboxNetUid(), TargetContainerName, Stack, SlotId); // Передать стек
}

// Передача стека между контроллерами по сети
void NetControllerTransfer(ASandboxPlayerController* LocalController, const UContainerComponent* RemoteContainer, const FContainerStack Stack, int32 SlotId) {
    const FString TargetContainerName = RemoteContainer->GetName(); // Получить имя целевого контейнера
    LocalController->TransferInventoryStack(TargetContainerName, Stack, SlotId); // Передать стек
}

// Передача стека между слотами внутри или между контейнерами
bool UContainerComponent::SlotTransfer(int32 SlotSourceId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne) {
    UContainerComponent* TargetContainer = this; // Установить целевой контейнер как текущий
    const FContainerStack* StackSourcePtr = SourceContainer->GetSlot(SlotSourceId); // Получить указатель на исходный стек
    const FContainerStack* StackTargetPtr = TargetContainer->GetSlot(SlotTargetId); // Получить указатель на целевой стек

    bool bInternalTransfer = (TargetContainer == SourceContainer); // Проверить, является ли передача внутренней

    FContainerStack NewSourceStack;
    FContainerStack NewTargetStack;

    if (StackTargetPtr) { // Если целевой стек действителен
        NewTargetStack = *StackTargetPtr; // Скопировать целевой стек
    }

    if (StackSourcePtr) { // Если исходный стек действителен
        NewSourceStack = *StackSourcePtr; // Скопировать исходный стек
    }

    bool bResult = false;

    APawn* LocalPawn = (APawn*)TargetContainer->GetOwner(); // Получить владельца целевого контейнера
    if (LocalPawn) { // Если владелец действителен
        ASandboxPlayerController* LocalController = Cast<ASandboxPlayerController>(LocalPawn->GetController()); // Получить контроллер
        if (LocalController) { // Если контроллер действителен
            const FString ContainerName = GetName(); // Получить имя контейнера
            const ASandboxObject* Obj = StackSourcePtr->GetObject(); // Получить исходный объект
            bool isValid = LocalController->OnContainerDropCheck(SlotTargetId, *ContainerName, Obj); // Проверить, действителен ли сброс
            if (!isValid) { // Если сброс недействителен
                return false;
            }
        }
    }

    if (IsSameObject(StackSourcePtr, StackTargetPtr)) { // Если исходный и целевой стеки содержат один и тот же объект
        const ASandboxObject* Obj = (ASandboxObject*)StackTargetPtr->GetObject(); // Получить целевой объект
        if (StackTargetPtr->Amount < (int)Obj->MaxStackSize) { // Если целевой стек может содержать больше предметов
            uint32 ChangeAmount = (bOnlyOne) ? 1 : StackSourcePtr->Amount; // Определить количество изменений
            uint32 NewAmount = StackTargetPtr->Amount + ChangeAmount; // Вычислить новое количество

            if (NewAmount <= Obj->MaxStackSize) { // Если новое количество в пределах максимального размера стека
                NewTargetStack.Amount = NewAmount; // Установить новое количество для целевого стека
                NewSourceStack.Amount -= ChangeAmount; // Уменьшить количество для исходного стека
            } else {
                int D = NewAmount - Obj->MaxStackSize; // Вычислить разницу
                NewTargetStack.Amount = Obj->MaxStackSize; // Установить целевой стек до максимального размера стека
                NewSourceStack.Amount = D; // Установить исходный стек до разницы
            }

            bResult = true; // Отметить результат как успешный
        }
    } else { // Если исходный и целевой стеки содержат разные объекты
        if (bOnlyOne) { // Если передается только один предмет
            if (!SourceContainer->IsSlotEmpty(SlotSourceId)) { // Если исходный слот не пуст
                if (TargetContainer->IsSlotEmpty(SlotTargetId)) { // Если целевой слот пуст
                    NewTargetStack = NewSourceStack; // Скопировать исходный стек в целевой стек
                    NewTargetStack.Amount = 1; // Установить количество равным 1
                    NewSourceStack.Amount--; // Уменьшить количество для исходного стека
                    bResult = true; // Отметить результат как успешный
                }
            }
        } else { // Если передается несколько предметов
            std::swap(NewTargetStack, NewSourceStack); // Поменять местами исходный и целевой стеки
            bResult = true; // Отметить результат как успешный
        }
    }

    if (bResult) { // Если передача была успешной
        if (GetNetMode() == NM_Client) { // Если игра запущена на клиенте
            ASandboxPlayerController* LocalController = Cast<ASandboxPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); // Получить локальный контроллер
            if (LocalController) { // Если локальный контроллер действителен
                if (bInternalTransfer) {
                    //TargetContainer->SetStackDirectly(NewTargetStack, SlotTargetId);
                    //SourceContainer->SetStackDirectly(NewSourceStack, SlotSourceId);
                }

                ASandboxObject* TargetObj = Cast<ASandboxObject>(TargetContainer->GetOwner()); // Получить целевой объект
                if (TargetObj) { // Если целевой объект действителен
                    NetObjectTransfer(LocalController, TargetObj, TargetContainer, NewTargetStack, SlotTargetId); // Передать целевой стек
                }

                ASandboxObject* SourceObj = Cast<ASandboxObject>(SourceContainer->GetOwner()); // Получить исходный объект
                if (SourceObj) { // Если исходный объект действителен
                    NetObjectTransfer(LocalController, SourceObj, SourceContainer, NewSourceStack, SlotSourceId); // Передать исходный стек
                }

                APawn* TargetPawn = Cast<APawn>(TargetContainer->GetOwner()); // Получить целевого персонажа
                if (TargetPawn) { // Если целевой персонаж действителен
                    NetControllerTransfer(LocalController, TargetContainer, NewTargetStack, SlotTargetId); // Передать целевой стек
                }

                APawn* SourcePawn = Cast<APawn>(SourceContainer->GetOwner()); // Получить исходного персонажа
                if (SourcePawn) { // Если исходный персонаж действителен
                    NetControllerTransfer(LocalController, SourceContainer, NewSourceStack, SlotSourceId); // Передать исходный стек
                }
            }
        } else { // Если игра запущена на сервере
            TargetContainer->SetStackDirectly(NewTargetStack, SlotTargetId); // Установить целевой стек непосредственно
            SourceContainer->SetStackDirectly(NewSourceStack, SlotSourceId); // Установить исходный стек непосредственно
        }
    }

    bUpdated = bResult; // Отметить контейнер как обновленный, если передача была успешной
    MakeStats(); // Обновить статистику инвентаря
    return bResult; // Вернуть результат
}

// Вызывается, когда содержимое реплицируется
void UContainerComponent::OnRep_Content() {
    UE_LOG(LogTemp, Warning, TEXT("OnRep_Content: %s"), *GetName()); // Записать в лог репликацию содержимого
    bUpdated = true; // Отметить контейнер как обновленный
    MakeStats(); // Обновить статистику инвентаря
}

// Проверка, был ли контейнер обновлен
bool UContainerComponent::IsUpdated() {
    return bUpdated; // Вернуть флаг обновления
}

// Сбросить флаг обновления
void UContainerComponent::ResetUpdatedFlag() {
    bUpdated = false; // Сбросить флаг обновления
}

// Обновить статистику инвентаря
void UContainerComponent::MakeStats() {
    InventoryStats.Empty(); // Очистить статистику инвентаря

    for (const auto& Stack : Content) { // Перебрать все предметы
        if (Stack.Amount > 0 && Stack.SandboxClassId > 0) { // Если в стеке есть предметы и действителен идентификатор класса
            auto T = InventoryStats.FindOrAdd(Stack.SandboxClassId) + Stack.Amount; // Найти или добавить идентификатор класса и обновить количество
            InventoryStats[Stack.SandboxClassId] = T; // Установить обновленное количество
        }
    }
}

// Получить статистику инвентаря
const TMap<uint64, uint32>& UContainerComponent::GetStats() const {
    return InventoryStats; // Вернуть статистику инвентаря
}

// Получить содержимое контейнера
const TArray<FContainerStack>& UContainerComponent::GetContent() {
    return Content; // Вернуть массив содержимого
}
