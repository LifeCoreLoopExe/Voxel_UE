
#include "SandboxObject.h"
#include "SandboxLevelController.h"
#include "Net/UnrealNetwork.h"

ASandboxObject::ASandboxObject() {
    PrimaryActorTick.bCanEverTick = true; // Разрешает тикирование актера
    SandboxRootMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SandboxRootMesh")); // Создает компонент статической меши
    MaxStackSize = 100; // Устанавливает максимальный размер стека
    bStackable = true; // Устанавливает возможность складывания
    RootComponent = SandboxRootMesh; // Устанавливает корневой компонент
    bReplicates = true; // Включает репликацию
    SandboxNetUid = 0; // Устанавливает сетевой уникальный идентификатор
    bCanPlaceSandboxObject = true; // Устанавливает возможность размещения объекта Sandbox
    SandboxRootMesh->SetLinearDamping(1.f); // Устанавливает линейное демпфирование
    SandboxRootMesh->SetAngularDamping(10.f); // Устанавливает угловое демпфирование
    SandboxRootMesh->BodyInstance.bGenerateWakeEvents = true; // Включает генерацию событий пробуждения
}

static const FString DefaultSandboxObjectName = FString(TEXT("Sandbox object")); // Устанавливает имя объекта Sandbox по умолчанию

void ASandboxObject::BeginPlay() {
    Super::BeginPlay(); // Вызывает метод BeginPlay родительского класса
    SandboxRootMesh->OnComponentSleep.AddDynamic(this, &ASandboxObject::OnSleep); // Добавляет обработчик события сна компонента
    //SandboxRootMesh->OnTakeRadialDamage.AddDynamic(this, &ASandboxObject::OnTakeRadialDamage);
    //SandboxRootMesh->OnTakeAnyDamage.AddDynamic(this, &ASandboxObject::OnTakeRadialDamage);
    //SandboxRootMesh->OnTakeAnyDamage.AddDynamic(this, &ASandboxObject::OnTakeRadialDamage);
}

void ASandboxObject::OnSleep(UPrimitiveComponent* SleepingComponent, FName BoneName) {
    //UE_LOG(LogTemp, Warning, TEXT("OnSleep"));
    SandboxRootMesh->SetSimulatePhysics(false); // Отключает физическую симуляцию
}

float ASandboxObject::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) {
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser); // Вызывает метод TakeDamage родительского класса

    if (DamageEnablePhysThreshold > 0 && ActualDamage > DamageEnablePhysThreshold) {
        EnablePhysics(); // Включает физическую симуляцию
    }

    return ActualDamage;
}

//void ASandboxObject::OnTakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) {
//}

FString ASandboxObject::GetSandboxName() {
    return DefaultSandboxObjectName; // Возвращает имя объекта Sandbox
}

FString ASandboxObject::GetSandboxNetUid() const {
    return SandboxNetUid; // Возвращает сетевой уникальный идентификатор
}

uint64 ASandboxObject::GetSandboxClassId() const {
    return SandboxClassId; // Возвращает идентификатор класса Sandbox
}

int ASandboxObject::GetSandboxTypeId() const {
    return 0; // Возвращает идентификатор типа Sandbox (заглушка)
}

int ASandboxObject::GetMaxStackSize() {
    if (!bStackable) {
        return 1; // Возвращает 1, если объект не складывается
    }

    return MaxStackSize; // Возвращает максимальный размер стека
}

UTexture2D* ASandboxObject::GetSandboxIconTexture() {
    return nullptr; // Возвращает nullptr (заглушка)
}

void ASandboxObject::TickInInventoryActive(float DeltaTime, UWorld* World, const FHitResult& HitResult) {
    // Метод для тикирования объекта в активном инвентаре (заглушка)
}

void ASandboxObject::ActionInInventoryActive(UWorld* World, const FHitResult& HitResult) {
    // Метод для действия объекта в активном инвентаре (заглушка)
}

bool ASandboxObject::CanTake(const AActor* Actor) const {
    if (SandboxRootMesh->IsSimulatingPhysics()) {
        return false; // Возвращает false, если физическая симуляция активна
    }

    TArray<UContainerComponent*> ContainerComponents;
    GetComponents<UContainerComponent>(ContainerComponents);

    for (const auto& ContainerComponent : ContainerComponents) {
        if (!ContainerComponent->IsEmpty()) {
            return false; // Возвращает false, если контейнер не пуст
        }
    }

    return true; // Возвращает true, если объект можно взять
}

void ASandboxObject::OnTerrainChange() {
    SandboxRootMesh->SetSimulatePhysics(true); // Включает физическую симуляцию
}

UContainerComponent* ASandboxObject::GetContainer(const FString& Name) {
    return GetFirstComponentByName<UContainerComponent>(this, Name); // Возвращает контейнер по имени
}

bool ASandboxObject::IsInteractive(const APawn* Source) {
    return false; // Возвращает false (заглушка)
}

void ASandboxObject::MainInteraction(const APawn* Source) {
    // Метод для основного взаимодействия (заглушка)
}

void ASandboxObject::SetProperty(FString Key, FString Value) {
    PropertyMap.Add(Key, Value); // Устанавливает свойство
}

FString ASandboxObject::GetProperty(FString Key) const {
    return PropertyMap.FindRef(Key); // Возвращает значение свойства
}

void ASandboxObject::RemoveProperty(FString Key) {
    PropertyMap.Remove(Key); // Удаляет свойство
}

void ASandboxObject::PostLoadProperties() {
    // Метод для загрузки свойств после создания (заглушка)
}

void ASandboxObject::OnPlaceToWorld() {
    // Метод для размещения объекта в мире (заглушка)
}

bool ASandboxObject::PlaceToWorldClcPosition(const UWorld* World, const FVector& SourcePos, const FRotator& SourceRotation, const FHitResult& Res, FVector& Location, FRotator& Rotation, bool bFinal) const {
    Location = Res.Location; // Устанавливает положение
    Rotation.Pitch = 0; // Устанавливает угол тангажа
    Rotation.Roll = 0; // Устанавливает угол крена
    Rotation.Yaw = SourceRotation.Yaw; // Устанавливает угол рыскания
    return true; // Возвращает true
}

void ASandboxObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps); // Вызывает метод GetLifetimeReplicatedProps родительского класса
    DOREPLIFETIME(ASandboxObject, SandboxNetUid); // Реплицирует свойство SandboxNetUid
}

const UStaticMeshComponent* ASandboxObject::GetMarkerMesh() const {
    return SandboxRootMesh; // Возвращает компонент статической меши
}

void ASandboxObject::EnablePhysics() {
    if (SandboxRootMesh) {
        SandboxRootMesh->SetSimulatePhysics(true); // Включает физическую симуляцию
    }
}

int ASandboxSkeletalModule::GetSandboxTypeId() const {
    return 500; // Возвращает идентификатор типа Sandbox (заглушка)
}

void ASandboxSkeletalModule::GetFootPose(FRotator& LeftFootRotator, FRotator& RightFootRotator) {
    LeftFootRotator = FootRotator; // Устанавливает угол левой ноги
    RightFootRotator = FRotator(-FootRotator.Pitch, -FootRotator.Yaw, FootRotator.Roll); // Устанавливает угол правой ноги
}

float ASandboxSkeletalModule::GetAffectParam(const FString& ParamName) const {
    if (AffectParamMap.Contains(ParamName)) {
        return AffectParamMap[ParamName]; // Возвращает значение параметра
    }

    return 1; // Возвращает 1, если параметр не найден
}

template<class T>
T* GetFirstComponentByName(AActor* Actor, FString ComponentName) {
    TArray<T*> Components;
    Actor->GetComponents<T>(Components);
    for (T* Component : Components) {
        if (Component->GetName() == ComponentName)
            return Component; // Возвращает компонент по имени
    }

    return nullptr; // Возвращает nullptr, если компонент не найден
}

TArray<ASandboxObject*> ASandboxObjectUtils::GetContainerContent(AActor* AnyActor, const FString& Name) {
    TArray<ASandboxObject*> Result;

    UContainerComponent* Container = GetFirstComponentByName<UContainerComponent>(AnyActor, Name);
    if (Container) {
        TArray<uint64> ObjList = Container->GetAllObjects();
        for (uint64 ClassId : ObjList) {
            ASandboxObject* Obj = ASandboxLevelController::GetDefaultSandboxObject(ClassId);
            if (Obj) {
                Result.Add(Obj); // Добавляет объект в результат
            }
        }
    }

    return Result; // Возвращает массив объектов
}

USandboxDamageType::USandboxDamageType(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    // Конструктор USandboxDamageType (заглушка)
}
