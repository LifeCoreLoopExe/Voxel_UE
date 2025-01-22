#include "SandboxObject.h" // Подключение заголовочного файла SandboxObject
#include "SandboxLevelController.h" // Подключение заголовочного файла SandboxLevelController
#include "Net/UnrealNetwork.h" // Подключение заголовочного файла для сетевых функций Unreal

// Конструктор класса ASandboxObject
ASandboxObject::ASandboxObject() {
    PrimaryActorTick.bCanEverTick = true; // Разрешение тика для актора
    SandboxRootMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SandboxRootMesh")); // Создание компонента статической сетки
    MaxStackSize = 100; // Максимальный размер стека
    bStackable = true; // Возможность складывания
    RootComponent = SandboxRootMesh; // Установка корневого компонента
    bReplicates = true; // Включение репликации
    SandboxNetUid = 0; // Уникальный сетевой идентификатор
    bCanPlaceSandboxObject = true; // Возможность размещения объекта
    SandboxRootMesh->SetLinearDamping(1.f); // Установка линейного демпфирования
    SandboxRootMesh->SetAngularDamping(10.f); // Установка углового демпфирования
    SandboxRootMesh->BodyInstance.bGenerateWakeEvents = true; // Включение генерации событий пробуждения
}

static const FString DefaultSandboxObjectName = FString(TEXT("Sandbox object")); // Имя объекта по умолчанию

// Метод, вызываемый при начале игры
void ASandboxObject::BeginPlay() {
    Super::BeginPlay(); // Вызов родительского метода
    SandboxRootMesh->OnComponentSleep.AddDynamic(this, &ASandboxObject::OnSleep); // Добавление обработчика события сна компонента
    //SandboxRootMesh->OnTakeRadialDamage.AddDynamic(this, &ASandboxObject::OnTakeRadialDamage); // Закомментированный обработчик радиального урона
    //SandboxRootMesh->OnTakeAnyDamage.AddDynamic(this, &ASandboxObject::OnTakeRadialDamage); // Закомментированный обработчик любого урона
    //SandboxRootMesh->OnTakeAnyDamage.AddDynamic(this, &ASandboxObject::OnTakeRadialDamage); // Закомментированный обработчик любого урона
}

// Метод, вызываемый при сне компонента
void ASandboxObject::OnSleep(UPrimitiveComponent* SleepingComponent, FName BoneName) {
    //UE_LOG(LogTemp, Warning, TEXT("OnSleep")); // Закомментированный лог
    SandboxRootMesh->SetSimulatePhysics(false); // Отключение симуляции физики
}

// Метод для получения урона
float ASandboxObject::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) {
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser); // Вызов родительского метода

    if (DamageEnablePhysThreshold > 0 && ActualDamage > DamageEnablePhysThreshold) { // Проверка порога урона
        EnablePhysics(); // Включение физики
    }

    return ActualDamage; // Возврат фактического урона
}

// Закомментированный метод для получения радиального урона
//void ASandboxObject::OnTakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) {
//}

// Метод для получения имени объекта
FString ASandboxObject::GetSandboxName() {
    return DefaultSandboxObjectName; // Возврат имени по умолчанию
}

// Метод для получения сетевого идентификатора
FString ASandboxObject::GetSandboxNetUid() const {
    return SandboxNetUid; // Возврат сетевого идентификатора
}

// Метод для получения идентификатора класса
uint64 ASandboxObject::GetSandboxClassId() const {
    return SandboxClassId; // Возврат идентификатора класса
}

// Метод для получения типа объекта
int ASandboxObject::GetSandboxTypeId() const {
    return 0; // Возврат типа объекта
}

// Метод для получения максимального размера стека
int ASandboxObject::GetMaxStackSize() {
    if (!bStackable) { // Проверка возможности складывания
        return 1; // Возврат 1, если объект не складывается
    }

    return MaxStackSize; // Возврат максимального размера стека
}

// Метод для получения иконки объекта
UTexture2D* ASandboxObject::GetSandboxIconTexture() {
    return NULL; // Возврат NULL
}

// Метод для тика в активном инвентаре
void ASandboxObject::TickInInventoryActive(float DeltaTime, UWorld* World, const FHitResult& HitResult) {
    // Пустой метод
}

// Метод для действия в активном инвентаре
void ASandboxObject::ActionInInventoryActive(UWorld* World, const FHitResult& HitResult) {
    // Пустой метод
}

// Метод для проверки возможности взятия объекта
bool ASandboxObject::CanTake(const AActor* Actor) const {
    if (SandboxRootMesh->IsSimulatingPhysics()) { // Проверка симуляции физики
        return false; // Возврат false, если физика симулируется
    }

    TArray<UContainerComponent*> ContainerComponents; // Массив компонентов контейнера
    GetComponents<UContainerComponent>(ContainerComponents); // Получение компонентов контейнера

    for (const auto& ContainerComponent : ContainerComponents) { // Проход по компонентам контейнера
        if (!ContainerComponent->IsEmpty()) { // Проверка пустоты контейнера
            return false; // Возврат false, если контейнер не пуст
        }
    }

    return true; // Возврат true
}

// Метод для изменения состояния при изменении ландшафта
void ASandboxObject::OnTerrainChange() {
    SandboxRootMesh->SetSimulatePhysics(true); // Включение симуляции физики
}

// Метод для получения контейнера по имени
UContainerComponent* ASandboxObject::GetContainer(const FString& Name) {
    return GetFirstComponentByName<UContainerComponent>(Name); // Получение первого компонента по имени
}

// Метод для проверки интерактивности объекта
bool ASandboxObject::IsInteractive(const APawn* Source) {
    return false; // Возврат false
}

// Метод для основного взаимодействия с объектом
void ASandboxObject::MainInteraction(const APawn* Source) {
    // Пустой метод
}

// Метод для установки свойства
void ASandboxObject::SetProperty(FString Key, FString Value) {
    PropertyMap.Add(Key, Value); // Добавление свойства в карту
}

// Метод для получения свойства
FString ASandboxObject::GetProperty(FString Key) const {
    return PropertyMap.FindRef(Key); // Получение свойства из карты
}

// Метод для удаления свойства
void ASandboxObject::RemoveProperty(FString Key) {
    PropertyMap.Remove(Key); // Удаление свойства из карты
}

// Метод для загрузки свойств после загрузки
void ASandboxObject::PostLoadProperties() {
    // Пустой метод
}

// Метод для размещения объекта в мире
void ASandboxObject::OnPlaceToWorld() {
    // Пустой метод
}

// Метод для расчета позиции размещения объекта в мире
bool ASandboxObject::PlaceToWorldClcPosition(const UWorld* World, const FVector& SourcePos, const FRotator& SourceRotation, const FHitResult& Res, FVector& Location, FRotator& Rotation, bool bFinal) const {
    Location = Res.Location; // Установка позиции
    Rotation.Pitch = 0; // Установка угла наклона
    Rotation.Roll = 0; // Установка угла крена
    Rotation.Yaw = SourceRotation.Yaw; // Установка угла рысканья
    return true; // Возврат true
}

// Метод для получения реплицируемых свойств
void ASandboxObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps); // Вызов родительского метода
    DOREPLIFETIME(ASandboxObject, SandboxNetUid); // Репликация сетевого идентификатора
}

// Метод для получения маркерной сетки
const UStaticMeshComponent* ASandboxObject::GetMarkerMesh() const {
    return SandboxRootMesh; // Возврат корневой сетки
}

// Метод для включения физики
void ASandboxObject::EnablePhysics() {
    if (SandboxRootMesh) { // Проверка наличия корневой сетки
        SandboxRootMesh->SetSimulatePhysics(true); // Включение симуляции физики
    }
}

// Метод для получения типа объекта скелетного модуля
int ASandboxSkeletalModule::GetSandboxTypeId() const {
    return 500; // Возврат типа объекта
}

// Метод для получения позиции ног
void ASandboxSkeletalModule::GetFootPose(FRotator& LeftFootRotator, FRotator& RightFootRotator) {
    LeftFootRotator = FootRotator; // Установка позиции левой ноги
    RightFootRotator = FRotator(-FootRotator.Pitch, -FootRotator.Yaw, FootRotator.Roll); // Установка позиции правой ноги
}

// Метод для получения параметра воздействия
float ASandboxSkeletalModule::GetAffectParam(const FString& ParamName) const {
    if (AffectParamMap.Contains(ParamName)) { // Проверка наличия параметра в карте
        return AffectParamMap[ParamName]; // Возврат параметра
    }

    return 1; // Возврат 1 по умолчанию
}

// Шаблонный метод для получения первого компонента по имени
template<class T>
T* GetFirstComponentByName(AActor* Actor, FString ComponentName) {
    TArray<T*> Components; // Массив компонентов
    Actor->GetComponents<T>(Components); // Получение компонентов
    for (T* Component : Components) { // Проход по компонентам
        if (Component->GetName() == ComponentName) // Проверка имени компонента
            return Component; // Возврат компонента
    }

    return nullptr; // Возврат nullptr, если компонент не найден
}

// Метод для получения содержимого контейнера
TArray<ASandboxObject*> ASandboxObjectUtils::GetContainerContent(AActor* AnyActor, const FString& Name) {
    TArray<ASandboxObject*> Result; // Результат

    UContainerComponent* Container = GetFirstComponentByName<UContainerComponent>(AnyActor, Name); // Получение контейнера по имени
    if (Container) { // Проверка наличия контейнера
        TArray<uint64> ObjList = Container->GetAllObjects(); // Получение всех объектов контейнера
        for (uint64 ClassId : ObjList) { // Проход по объектам
            ASandboxObject* Obj = ASandboxLevelController::GetDefaultSandboxObject(ClassId); // Получение объекта по идентификатору класса
            if (Obj) { // Проверка наличия объекта
                Result.Add(Obj); // Добавление объекта в результат
            }
        }
    }

    return Result; // Возврат результата
}

// Конструктор класса USandboxDamageType
USandboxDamageType::USandboxDamageType(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    // Пустой конструктор
}

Эти комментарии объясняют, что делает каждая строка кода.
