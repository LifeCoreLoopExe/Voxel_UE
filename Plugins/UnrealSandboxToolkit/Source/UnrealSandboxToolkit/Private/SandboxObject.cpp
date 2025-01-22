// Заполните свое уведомление об авторских правах на странице "Описание" в настройках проекта.

#include "SandboxObject.h"
#include "SandboxLevelController.h"
#include "Net/UnrealNetwork.h"

// Конструктор для ASandboxObject
ASandboxObject::ASandboxObject() {
    PrimaryActorTick.bCanEverTick = true; // Разрешение тиков для этого актора
    SandboxRootMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SandboxRootMesh")); // Создание компонента статической сетки
    MaxStackSize = 100; // Максимальный размер стека
    bStackable = true; // Возможность складывания в стек
    RootComponent = SandboxRootMesh; // Установка корневого компонента
    bReplicates = true; // Включение репликации для этого актора
    SandboxNetUid = 0; // Инициализация сетевого идентификатора
    bCanPlaceSandboxObject = true; // Возможность размещения объекта песочницы
    SandboxRootMesh->SetLinearDamping(1.f); // Установка линейного демпфирования
    SandboxRootMesh->SetAngularDamping(10.f); // Установка углового демпфирования
    SandboxRootMesh->BodyInstance.bGenerateWakeEvents = true; // Генерация событий пробуждения
}

// Имя объекта песочницы по умолчанию
static const FString DefaultSandboxObjectName = FString(TEXT("Sandbox object"));

// Вызывается, когда игра начинается или когда объект создается
void ASandboxObject::BeginPlay() {
    Super::BeginPlay(); // Вызов функции BeginPlay родительского класса
    SandboxRootMesh->OnComponentSleep.AddDynamic(this, &ASandboxObject::OnSleep); // Добавление обработчика события сна компонента
    //SandboxRootMesh->OnTakeRadialDamage.AddDynamic(this, &ASandboxObject::OnTakeRadialDamage);
    //SandboxRootMesh->OnTakeAnyDamage.AddDynamic(this, &ASandboxObject::OnTakeRadialDamage);
    //SandboxRootMesh->OnTakeAnyDamage.AddDynamic(this, &ASandboxObject::OnTakeRadialDamage);
}

// Обработка события сна компонента
void ASandboxObject::OnSleep(UPrimitiveComponent* SleepingComponent, FName BoneName) {
    //UE_LOG(LogTemp, Warning, TEXT("OnSleep"));
    SandboxRootMesh->SetSimulatePhysics(false); // Отключение симуляции физики
}

// Обработка получения урона
float ASandboxObject::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) {
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser); // Вызов функции TakeDamage родительского класса

    if (DamageEnablePhysThreshold > 0 && ActualDamage > DamageEnablePhysThreshold) {
        EnablePhysics(); // Включение физики
    }

    return ActualDamage;
}

// Получение имени объекта песочницы
FString ASandboxObject::GetSandboxName() {
    return DefaultSandboxObjectName; // Возврат имени по умолчанию
}

// Получение сетевого идентификатора объекта песочницы
FString ASandboxObject::GetSandboxNetUid() const {
    return SandboxNetUid; // Возврат сетевого идентификатора
}

// Получение идентификатора класса объекта песочницы
uint64 ASandboxObject::GetSandboxClassId() const {
    return SandboxClassId; // Возврат идентификатора класса
}

// Получение идентификатора типа объекта песочницы
int ASandboxObject::GetSandboxTypeId() const {
    return 0; // Возврат идентификатора типа
}

// Получение максимального размера стека
int ASandboxObject::GetMaxStackSize() {
    if (!bStackable) {
        return 1; // Возврат 1, если объект не может быть сложен в стек
    }

    return MaxStackSize; // Возврат максимального размера стека
}

// Получение текстуры иконки объекта песочницы
UTexture2D* ASandboxObject::GetSandboxIconTexture() {
    return nullptr; // Возврат nullptr, так как текстура не установлена
}

// Обновление объекта в активном инвентаре
void ASandboxObject::TickInInventoryActive(float DeltaTime, UWorld* World, const FHitResult& HitResult) {
    // Реализация обновления
}

// Действие в активном инвентаре
void ASandboxObject::ActionInInventoryActive(UWorld* World, const FHitResult& HitResult) {
    // Реализация действия
}

// Проверка, может ли объект быть взят
bool ASandboxObject::CanTake(const AActor* Actor) const {
    if (SandboxRootMesh->IsSimulatingPhysics()) {
        return false; // Возврат false, если симуляция физики включена
    }

    TArray<UContainerComponent*> ContainerComponents;
    GetComponents<UContainerComponent>(ContainerComponents); // Получение всех компонентов контейнера

    for (const auto& ContainerComponent : ContainerComponents) {
        if (!ContainerComponent->IsEmpty()) {
            return false; // Возврат false, если контейнер не пуст
        }
    }

    return true; // Возврат true, если объект может быть взят
}

// Обработка изменения террейна
void ASandboxObject::OnTerrainChange() {
    SandboxRootMesh->SetSimulatePhysics(true); // Включение симуляции физики
}

// Получение контейнера по имени
UContainerComponent* ASandboxObject::GetContainer(const FString& Name) {
    return GetFirstComponentByName<UContainerComponent>(Name); // Получение первого компонента контейнера по имени
}

// Проверка, является ли объект интерактивным
bool ASandboxObject::IsInteractive(const APawn* Source) {
    return false; // Возврат false, так как объект не является интерактивным
}

// Основное взаимодействие с объектом
void ASandboxObject::MainInteraction(const APawn* Source) {
    // Реализация взаимодействия
}

// Установка свойства объекта
void ASandboxObject::SetProperty(FString Key, FString Value) {
    PropertyMap.Add(Key, Value); // Добавление свойства в карту свойств
}

// Получение свойства объекта
FString ASandboxObject::GetProperty(FString Key) const {
    return PropertyMap.FindRef(Key); // Получение свойства из карты свойств
}

// Удаление свойства объекта
void ASandboxObject::RemoveProperty(FString Key) {
    PropertyMap.Remove(Key); // Удаление свойства из карты свойств
}

// Загрузка свойств после их установки
void ASandboxObject::PostLoadProperties() {
    // Реализация загрузки свойств
}

// Обработка размещения объекта в мире
void ASandboxObject::OnPlaceToWorld() {
    // Реализация размещения
}

// Вычисление позиции для размещения объекта в мире
bool ASandboxObject::PlaceToWorldClcPosition(const UWorld* World, const FVector& SourcePos, const FRotator& SourceRotation, const FHitResult& Res, FVector& Location, FRotator& Rotation, bool bFinal) const {
    Location = Res.Location; // Установка местоположения
    Rotation.Pitch = 0; // Установка нулевого угла тангажа
    Rotation.Roll = 0; // Установка нулевого угла крена
    Rotation.Yaw = SourceRotation.Yaw; // Установка угла рысканья
    return true; // Возврат true
}

// Репликация свойств для сетевой синхронизации
void ASandboxObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps); // Вызов функции родительского класса
    DOREPLIFETIME(ASandboxObject, SandboxNetUid); // Репликация сетевого идентификатора
}

// Получение маркерной сетки
const UStaticMeshComponent* ASandboxObject::GetMarkerMesh() const {
    return SandboxRootMesh; // Возврат маркерной сетки
}

// Включение физики для объекта
void ASandboxObject::EnablePhysics() {
    if (SandboxRootMesh) {
        SandboxRootMesh->SetSimulatePhysics(true); // Включение симуляции физики
    }
}

// Получение идентификатора типа объекта песочницы для скелетного модуля
int ASandboxSkeletalModule::GetSandboxTypeId() const {
    return 500; // Возврат идентификатора типа
}

// Получение позиции стопы для скелетного модуля
void ASandboxSkeletalModule::GetFootPose(FRotator& LeftFootRotator, FRotator& RightFootRotator) {
    LeftFootRotator = FootRotator; // Установка позиции левой стопы
    RightFootRotator = FRotator(-FootRotator.Pitch, -FootRotator.Yaw, FootRotator.Roll); // Установка позиции правой стопы
}

// Получение параметра воздействия для скелетного модуля
float ASandboxSkeletalModule::GetAffectParam(const FString& ParamName) const {
    if (AffectParamMap.Contains(ParamName)) {
        return AffectParamMap[ParamName]; // Возврат параметра воздействия
    }

    return 1; // Возврат 1, если параметр не найден
}

// Получение первого компонента по имени
template<class T>
T* GetFirstComponentByName(AActor* Actor, FString ComponentName) {
    TArray<T*> Components;
    Actor->GetComponents<T>(Components); // Получение всех компонентов типа T
    for (T* Component : Components) {
        if (Component->GetName() == ComponentName) {
            return Component; // Возврат компонента, если имя совпадает
        }
    }

    return nullptr; // Возврат nullptr, если компонент не найден
}

// Получение содержимого контейнера
TArray<ASandboxObject*> ASandboxObjectUtils::GetContainerContent(AActor* AnyActor, const FString& Name) {
    TArray<ASandboxObject*> Result;

    UContainerComponent* Container = GetFirstComponentByName<UContainerComponent>(AnyActor, Name); // Получение первого компонента контейнера по имени
    if (Container) {
        TArray<uint64> ObjList = Container->GetAllObjects(); // Получение всех объектов в контейнере
        for (uint64 ClassId : ObjList) {
            ASandboxObject* Obj = ASandboxLevelController::GetDefaultSandboxObject(ClassId); // Получение объекта песочницы по умолчанию
            if (Obj) {
                Result.Add(Obj); // Добавление объекта в результат
            }
        }
    }

    return Result; // Возврат результата
}

// Конструктор для USandboxDamageType
USandboxDamageType::USandboxDamageType(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    // Реализация конструктора
}
