// Copyright blackw 2015-2020

#include "ContainerComponent.h" // Подключение заголовочного файла ContainerComponent
#include "SandboxObject.h" // Подключение заголовочного файла SandboxObject
#include "Net/UnrealNetwork.h" // Подключение заголовочного файла для сетевых функций Unreal
#include "SandboxLevelController.h" // Подключение заголовочного файла SandboxLevelController
#include "SandboxPlayerController.h" // Подключение заголовочного файла SandboxPlayerController
#include <algorithm> // Подключение стандартной библиотеки алгоритмов C++

const ASandboxObject* FContainerStack::GetObject() const {
    return ASandboxLevelController::GetDefaultSandboxObject(SandboxClassId); // Возвращает объект песочницы по идентификатору класса
}

UContainerComponent::UContainerComponent() {
    //bWantsBeginPlay = true; // Отключение автоматического вызова BeginPlay
    PrimaryComponentTick.bCanEverTick = false; // Отключение возможности тика компонента
}

void UContainerComponent::BeginPlay() {
    Super::BeginPlay(); // Вызов родительского метода BeginPlay
}

void UContainerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction); // Вызов родительского метода TickComponent
}

/*
bool UContainerComponent::IsOwnerAdmin() {
    ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetOwner()); // Получение владельца компонента и приведение его к типу ASandboxCharacter
    if (SandboxCharacter) {
        return SandboxCharacter->bIsAdmin; // Проверка, является ли владелец админом
    }

    return true;
}
*/

bool UContainerComponent::IsEmpty() const {
    if (Content.Num() == 0) { // Проверка, пуст ли контейнер
        return true;
    }

    for (int Idx = 0; Idx < Content.Num(); Idx++) { // Проход по всем элементам контейнера
        FContainerStack Stack = Content[Idx];
        if (Stack.Amount > 0) { // Проверка, есть ли хотя бы один элемент с количеством больше 0
            return false;
        }
    }

    return true;
}

bool UContainerComponent::SetStackDirectly(const FContainerStack& Stack, const int SlotId) {
    if (SlotId >= Content.Num()) { // Проверка, достаточно ли места в контейнере
        Content.SetNum(SlotId + 1); // Увеличение размера контейнера, если необходимо
    }

    FContainerStack* StackPtr = &Content[SlotId]; // Получение указателя на стек в указанном слоте
    if (Stack.Amount > 0) { // Проверка, есть ли элементы в стеке
        StackPtr->Amount = Stack.Amount; // Установка количества элементов
        StackPtr->SandboxClassId = Stack.SandboxClassId; // Установка идентификатора класса
    } else {
        StackPtr->Clear(); // Очистка стека, если количество элементов равно 0
    }

    MakeStats(); // Обновление статистики контейнера
    bUpdated = true; // Установка флага обновления
    return true;
}

bool UContainerComponent::AddObject(ASandboxObject* Obj) {
    if (Obj == nullptr) { // Проверка, не является ли объект нулевым
        return false;
    }

    uint32 MaxStackSize = Obj->GetMaxStackSize(); // Получение максимального размера стека для объекта

    int FirstEmptySlot = -1; // Инициализация переменной для первого пустого слота
    bool bIsAdded = false; // Инициализация флага добавления
    for (int Idx = 0; Idx < Content.Num(); Idx++) { // Проход по всем элементам контейнера
        FContainerStack* Stack = &Content[Idx];

        //TODO check inventory max volume and mass // TODO: проверка максимального объема и массы инвентаря
        if (Stack->Amount > 0) { // Проверка, есть ли элементы в стеке
            if (Stack->SandboxClassId == Obj->GetSandboxClassId() && MaxStackSize > 1 && (uint64)Stack->Amount < MaxStackSize) { // Проверка, можно ли добавить объект в существующий стек
                Stack->Amount++; // Увеличение количества элементов в стеке
                bIsAdded = true; // Установка флага добавления
                break;
            }
        } else {
            if (FirstEmptySlot < 0) { // Проверка, найден ли первый пустой слот
                FirstEmptySlot = Idx; // Установка индекса первого пустого слота
                if (MaxStackSize == 1) { // Проверка, равен ли максимальный размер стека 1
                    break;
                }
            }
        }
    }

    if (!bIsAdded) { // Проверка, был ли объект добавлен
        if (FirstEmptySlot >= 0) { // Проверка, найден ли первый пустой слот
            FContainerStack* Stack = &Content[FirstEmptySlot]; // Получение указателя на стек в первом пустом слоте
            Stack->Amount = 1; // Установка количества элементов в стеке
            Stack->SandboxClassId = Obj->GetSandboxClassId(); // Установка идентификатора класса
        } else {
            //UE_LOG(LogTemp, Warning, TEXT("AddObject -> Content.Num() -> MaxCapacity: %d %d"), Content.Num(), MaxCapacity); // Логирование предупреждения
            if (Content.Num() < MaxCapacity) { // Проверка, достаточно ли места в контейнере
                FContainerStack NewStack; // Создание нового стека
                NewStack.Amount = 1; // Установка количества элементов в стеке
                NewStack.SandboxClassId = Obj->GetSandboxClassId(); // Установка идентификатора класса
                Content.Add(NewStack); // Добавление нового стека в контейнер
            } else {
                return false; // Возврат false, если нет места в контейнере
            }
        }
    }

    MakeStats(); // Обновление статистики контейнера
    bUpdated = true; // Установка флага обновления
    return true;
}

FContainerStack* UContainerComponent::GetSlot(const int Slot) {
    if (!Content.IsValidIndex(Slot)) { // Проверка, является ли индекс слота допустимым
        return nullptr;
    }

    return &Content[Slot]; // Возврат указателя на стек в указанном слоте
}

const FContainerStack* UContainerComponent::GetSlot(const int Slot) const {
    if (!Content.IsValidIndex(Slot)) { // Проверка, является ли индекс слота допустимым
        return nullptr;
    }

    return &Content[Slot]; // Возврат указателя на стек в указанном слоте
}

/*
ASandboxObject* UContainerComponent::GetAvailableSlotObject(const int Slot) {
    if (!Content.IsValidIndex(Slot)) { // Проверка, является ли индекс слота допустимым
        return nullptr;
    }

    FContainerStack* Stack = &Content[Slot];
    if (Stack->Amount > 0) { // Проверка, есть ли элементы в стеке
        TSubclassOf<ASandboxObject> ObjectClass = Stack->ObjectClass; // Получение класса объекта
        if (ObjectClass) { // Проверка, не является ли класс объекта нулевым
            return (ASandboxObject*)(ObjectClass->GetDefaultObject()); // Возврат объекта по умолчанию
        }
    }

    return nullptr;
}
*/

bool UContainerComponent::DecreaseObjectsInContainer(int Slot, int Num) {
    FContainerStack* Stack = GetSlot(Slot); // Получение указателя на стек в указанном слоте

    if (Stack == NULL) { // Проверка, не является ли указатель нулевым
        return false;
    }

    if (Stack->Amount > 0) { // Проверка, есть ли элементы в стеке
        Stack->Amount -= Num; // Уменьшение количества элементов в стеке
        if (Stack->Amount == 0) { // Проверка, равно ли количество элементов 0
            Stack->Clear(); // Очистка стека
        }
    }

    MakeStats(); // Обновление статистики контейнера
    bUpdated = true; // Установка флага обновления
    return Stack->Amount > 0; // Возврат true, если в стеке остались элементы
}

void UContainerComponent::ChangeAmount(int Slot, int Num) {
    FContainerStack* Stack = GetSlot(Slot); // Получение указателя на стек в указанном слоте

    //TODO check stack size // TODO: проверка размера стека
    if (Stack) { // Проверка, не является ли указатель нулевым
        if (Stack->Amount > 0) { // Проверка, есть ли элементы в стеке
            Stack->Amount += Num; // Изменение количества элементов в стеке
            if (Stack->Amount == 0) { // Проверка, равно ли количество элементов 0
                Stack->Clear(); // Очистка стека
            }

            MakeStats(); // Обновление статистики контейнера
            bUpdated = true; // Установка флага обновления
        }
    }
}

bool UContainerComponent::IsSlotEmpty(int SlotId) const {
    const FContainerStack* Stack = GetSlot(SlotId); // Получение указателя на стек в указанном слоте
    if (Stack) { // Проверка, не является ли указатель нулевым
        return Stack->IsEmpty(); // Проверка, пуст ли стек
    }

    return true;
}

void UContainerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps); // Вызов родительского метода GetLifetimeReplicatedProps
    DOREPLIFETIME(UContainerComponent, Content); // Репликация содержимого контейнера
}

void UContainerComponent::CopyTo(UContainerComponent* Target) {
    Target->Content = this->Content; // Копирование содержимого контейнера в целевой контейнер
    bUpdated = true; // Установка флага обновления
    MakeStats(); // Обновление статистики контейнера
}

TArray<uint64> UContainerComponent::GetAllObjects() const {
    TArray<uint64> Result; // Инициализация массива для результата
    for (int Idx = 0; Idx < Content.Num(); Idx++) { // Проход по всем элементам контейнера
        const FContainerStack* Stack = &Content[Idx];
        if (Stack) { // Проверка, не является ли указатель нулевым
            const ASandboxObject* Obj = Stack->GetObject(); // Получение объекта песочницы
            if (Obj) { // Проверка, не является ли объект нулевым
                Result.Add(Obj->GetSandboxClassId()); // Добавление идентификатора класса объекта в результат
            }
        }
    }

    return Result;
}

bool IsSameObject(const FContainerStack* StackSourcePtr, const FContainerStack* StackTargetPtr) {
    if (StackSourcePtr && StackTargetPtr) { // Проверка, не являются ли указатели нулевыми
        const auto* SourceObj = StackSourcePtr->GetObject(); // Получение объекта песочницы из исходного стека
        const auto* TargetObj = StackTargetPtr->GetObject(); // Получение объекта песочницы из целевого стека
        if (SourceObj && TargetObj) { // Проверка, не являются ли объекты нулевыми
            return SourceObj->GetSandboxClassId() == TargetObj->GetSandboxClassId(); // Проверка, равны ли идентификаторы классов объектов
        }

    }

    return false;
}

void NetObjectTransfer(ASandboxPlayerController* LocalController, const ASandboxObject* RemoteObj, const UContainerComponent* RemoteContainer, const FContainerStack Stack, int32 SlotId) {
    const FString TargetContainerName = RemoteContainer->GetName(); // Получение имени целевого контейнера
    LocalController->TransferContainerStack(RemoteObj->GetSandboxNetUid(), TargetContainerName, Stack, SlotId); // Передача стека контейнера
}

void NetControllerTransfer(ASandboxPlayerController* LocalController, const UContainerComponent* RemoteContainer, const FContainerStack Stack, int32 SlotId) {
    const FString TargetContainerName = RemoteContainer->GetName(); // Получение имени целевого контейнера
    LocalController->TransferInventoryStack(TargetContainerName, Stack, SlotId); // Передача стека инвентаря
}

bool UContainerComponent::SlotTransfer(int32 SlotSourceId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne) {
    UContainerComponent* TargetContainer = this; // Установка целевого контейнера
    const FContainerStack* StackSourcePtr = SourceContainer->GetSlot(SlotSourceId); // Получение указателя на стек в исходном слоте
    const FContainerStack* StackTargetPtr = TargetContainer->GetSlot(SlotTargetId); // Получение указателя на стек в целевом слоте

    bool bInternalTransfer = (TargetContainer == SourceContainer); // Проверка, является ли передача внутренней

    FContainerStack NewSourceStack; // Инициализация нового исходного стека
    FContainerStack NewTargetStack; // Инициализация нового целевого стека

    if (StackTargetPtr) { // Проверка, не является ли указатель нулевым
        NewTargetStack = *StackTargetPtr; // Копирование целевого стека
    }

    if (StackSourcePtr) { // Проверка, не является ли указатель нулевым
        NewSourceStack = *StackSourcePtr; // Копирование исходного стека
    }

    bool bResult = false; // Инициализация флага результата

    APawn* LocalPawn = (APawn*)TargetContainer->GetOwner(); // Получение владельца целевого контейнера и приведение его к типу APawn
    if (LocalPawn) { // Проверка, не является ли владелец нулевым
        ASandboxPlayerController* LocalController = Cast<ASandboxPlayerController>(LocalPawn->GetController()); // Получение контроллера владельца
        if (LocalController) { // Проверка, не является ли контроллер нулевым
            const FString ContainerName = GetName(); // Получение имени контейнера
            const ASandboxObject* Obj = StackSourcePtr->GetObject(); // Получение объекта песочницы из исходного стека
            bool isValid = LocalController->OnContainerDropCheck(SlotTargetId, *ContainerName, Obj); // Проверка, допустима ли передача
            if (!isValid) { // Проверка, допустима ли передача
                return false;
            }
        }
    }

    if (IsSameObject(StackSourcePtr, StackTargetPtr)) { // Проверка, являются ли объекты в стеках одинаковыми
        const ASandboxObject* Obj = (ASandboxObject*)StackTargetPtr->GetObject(); // Получение объекта песочницы из целевого стека
        if (StackTargetPtr->Amount < (int)Obj->MaxStackSize) { // Проверка, можно ли добавить объект в целевой стек
            uint32 ChangeAmount = (bOnlyOne) ? 1 : StackSourcePtr->Amount; // Определение количества объектов для передачи
            uint32 NewAmount = StackTargetPtr->Amount + ChangeAmount; // Определение нового количества объектов в целевом стеке

            if (NewAmount <= Obj->MaxStackSize) { // Проверка, не превышает ли новое количество объектов максимальный размер стека
                NewTargetStack.Amount = NewAmount; // Установка нового количества объектов в целевом стеке
                NewSourceStack.Amount -= ChangeAmount; // Уменьшение количества объектов в исходном стеке
            } else {
                int D = NewAmount - Obj->MaxStackSize; // Определение количества объектов, превышающих максимальный размер стека
                NewTargetStack.Amount = Obj->MaxStackSize; // Установка максимального количества объектов в целевом стеке
                NewSourceStack.Amount = D; // Установка количества объектов, превышающих максимальный размер стека, в исходном стеке
            }

            bResult = true; // Установка флага результата
        }
    } else {
        if (bOnlyOne) { // Проверка, передается ли только один объект
            if (!SourceContainer->IsSlotEmpty(SlotSourceId)) { // Проверка, не пуст ли исходный слот
                if (TargetContainer->IsSlotEmpty(SlotTargetId)) { // Проверка, пуст ли целевой слот
                    NewTargetStack = NewSourceStack; // Копирование исходного стека в целевой стек
                    NewTargetStack.Amount = 1; // Установка количества объектов в целевом стеке
                    NewSourceStack.Amount--; // Уменьшение количества объектов в исходном стеке
                    bResult = true; // Установка флага результата
                }
            }
        } else {
            std::swap(NewTargetStack, NewSourceStack); // Обмен исходного и целевого стеков
            bResult = true; // Установка флага результата
        }
    }

    if (bResult) { // Проверка, был ли результат успешным
        if (GetNetMode() == NM_Client) { // Проверка, является ли режим сети клиентским
            ASandboxPlayerController* LocalController = Cast<ASandboxPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); // Получение локального контроллера
            if (LocalController) { // Проверка, не является ли контроллер нулевым
                if (bInternalTransfer) { // Проверка, является ли передача внутренней
                    //TargetContainer->SetStackDirectly(NewTargetStack, SlotTargetId); // Установка целевого стека напрямую
                    //SourceContainer->SetStackDirectly(NewSourceStack, SlotSourceId); // Установка исходного стека напрямую
                }

                ASandboxObject* TargetObj = Cast<ASandboxObject>(TargetContainer->GetOwner()); // Получение объекта песочницы из целевого контейнера
                if (TargetObj) { // Проверка, не является ли объект нулевым
                    NetObjectTransfer(LocalController, TargetObj, TargetContainer, NewTargetStack, SlotTargetId); // Передача объекта песочницы
                }

                ASandboxObject* SourceObj = Cast<ASandboxObject>(SourceContainer->GetOwner()); // Получение объекта песочницы из исходного контейнера
                if (SourceObj) { // Проверка, не является ли объект нулевым
                    NetObjectTransfer(LocalController, SourceObj, SourceContainer, NewSourceStack, SlotSourceId); // Передача объекта песочницы
                }

                APawn* TargetPawn = Cast<APawn>(TargetContainer->GetOwner()); // Получение владельца целевого контейнера и приведение его к типу APawn
                if (TargetPawn) { // Проверка, не является ли владелец нулевым
                    NetControllerTransfer(LocalController, TargetContainer, NewTargetStack, SlotTargetId); // Передача контроллера
                }

                APawn* SourcePawn = Cast<APawn>(SourceContainer->GetOwner()); // Получение владельца исходного контейнера и приведение его к типу APawn
                if (SourcePawn) { // Проверка, не является ли владелец нулевым
                    NetControllerTransfer(LocalController, SourceContainer, NewSourceStack, SlotSourceId); // Передача контроллера
                }
            }
        } else {
            // server only // только сервер
            TargetContainer->SetStackDirectly(NewTargetStack, SlotTargetId); // Установка целевого стека напрямую
            SourceContainer->SetStackDirectly(NewSourceStack, SlotSourceId); // Установка исходного стека напрямую
        }
    }

    bUpdated = bResult; // Установка флага обновления
    MakeStats(); // Обновление статистики контейнера
    return bResult;
}

void UContainerComponent::OnRep_Content() {
    UE_LOG(LogTemp, Warning, TEXT("OnRep_Content: %s"), *GetName()); // Логирование предупреждения
    bUpdated = true; // Установка флага обновления
    MakeStats(); // Обновление статистики контейнера
}

bool UContainerComponent::IsUpdated() {
    return bUpdated; // Возврат флага обновления
}

void UContainerComponent::ResetUpdatedFlag() {
    bUpdated = false; // Сброс флага обновления
}

void UContainerComponent::MakeStats() {
    InventoryStats.Empty(); // Очистка статистики инвентаря

    for (const auto& Stack : Content) { // Проход по всем элементам контейнера
        if (Stack.Amount > 0 && Stack.SandboxClassId > 0) { // Проверка, есть ли элементы в стеке и не равен ли идентификатор класса 0
            auto T = InventoryStats.FindOrAdd(Stack.SandboxClassId) + Stack.Amount; // Нахождение или добавление идентификатора класса в статистику инвентаря и увеличение количества элементов
            InventoryStats[Stack.SandboxClassId] = T; // Установка количества элементов в статистике инвентаря
        }
    }
}

const TMap<uint64, uint32>& UContainerComponent::GetStats() const {
    return InventoryStats; // Возврат статистики инвентаря
}

const TArray<FContainerStack>& UContainerComponent::GetContent() {
    return Content; // Возврат содержимого контейнера
}