#include "SandboxObject.h"
#include "SandboxLevelController.h"
#include "Net/UnrealNetwork.h"

// Основной конструктор класса SandboxObject
// Здесь настраиваются базовые параметры объекта
ASandboxObject::ASandboxObject() {
	PrimaryActorTick.bCanEverTick = true; // Включаем обновление каждый кадр
	SandboxRootMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SandboxRootMesh")); // Создаем корневой меш компонент
	MaxStackSize = 100; // Максимальный размер стека предметов
	bStackable = true; // Можно ли складывать предметы в стопку
	RootComponent = SandboxRootMesh; // Устанавливаем корневой компонент
	bReplicates = true; // Включаем сетевую репликацию
	SandboxNetUid = 0; // Уникальный сетевой идентификатор
	bCanPlaceSandboxObject = true; // Можно ли размещать объект
	SandboxRootMesh->SetLinearDamping(1.f); // Устанавливаем линейное затухание для физики
	SandboxRootMesh->SetAngularDamping(10.f); // Устанавливаем угловое затухание для физики
	SandboxRootMesh->BodyInstance.bGenerateWakeEvents = true; // Включаем события пробуждения физики
}

// Стандартное имя объекта по умолчанию
static const FString DefaultSandboxObjectName = FString(TEXT("Sandbox object"));

// Вызывается при начале игры
void ASandboxObject::BeginPlay() {
	Super::BeginPlay();
	// Подписываемся на событие засыпания физики
	SandboxRootMesh->OnComponentSleep.AddDynamic(this, &ASandboxObject::OnSleep);
}

// Обработчик события засыпания физики
void ASandboxObject::OnSleep(UPrimitiveComponent* SleepingComponent, FName BoneName) {
	SandboxRootMesh->SetSimulatePhysics(false); // Отключаем симуляцию физики
}

// Обработка получения урона
float ASandboxObject::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) {
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// Если урон превышает порог, включаем физику
	if (DamageEnablePhysThreshold > 0 && ActualDamage > DamageEnablePhysThreshold) {
		EnablePhysics();
	}

	return ActualDamage;
}

// Получение имени объекта
FString ASandboxObject::GetSandboxName() {
	return DefaultSandboxObjectName;
}

// Получение сетевого ID
FString ASandboxObject::GetSandboxNetUid() const {
	return SandboxNetUid;
}

// Получение ID класса
uint64 ASandboxObject::GetSandboxClassId() const {
	return SandboxClassId; 
}

// Получение типа объекта
int ASandboxObject::GetSandboxTypeId() const {
	return 0; 
}

// Получение максимального размера стека
int ASandboxObject::GetMaxStackSize() {
	if (!bStackable) {
		return 1;
	}
	return MaxStackSize;
}

// Получение текстуры иконки
UTexture2D* ASandboxObject::GetSandboxIconTexture() {
	return NULL; 
}

// Обработка тика в инвентаре
void ASandboxObject::TickInInventoryActive(float DeltaTime, UWorld* World, const FHitResult& HitResult) {
}

// Обработка действия в инвентаре
void ASandboxObject::ActionInInventoryActive(UWorld* World, const FHitResult& HitResult) {
}

// Проверка возможности подбора объекта
bool ASandboxObject::CanTake(const AActor* Actor) const {
	// Нельзя подобрать, если активна физика
	if (SandboxRootMesh->IsSimulatingPhysics()) {
		return false;
	}
	
	// Проверяем, пусты ли все контейнеры
	TArray<UContainerComponent*> ContainerComponents;
	GetComponents<UContainerComponent>(ContainerComponents);

	for (const auto& ContainerComponent : ContainerComponents) {
		if (!ContainerComponent->IsEmpty()) {
			return false;
		}
	}
	
	return true; 
}

// Обработка изменения террейна
void ASandboxObject::OnTerrainChange() {
	SandboxRootMesh->SetSimulatePhysics(true);
}

// Получение контейнера по имени
UContainerComponent* ASandboxObject::GetContainer(const FString& Name) { 
	return GetFirstComponentByName<UContainerComponent>(Name);
}

// Проверка возможности взаимодействия
bool ASandboxObject::IsInteractive(const APawn* Source) {
	return false;
}

// Основное взаимодействие
void ASandboxObject::MainInteraction(const APawn* Source) {
}

// Установка свойства
void ASandboxObject::SetProperty(FString Key, FString Value) {
	PropertyMap.Add(Key, Value);
}

// Получение свойства
FString ASandboxObject::GetProperty(FString Key) const {
	return PropertyMap.FindRef(Key);
}

// Удаление свойства
void ASandboxObject::RemoveProperty(FString Key) {
	PropertyMap.Remove(Key);
}

// Обработка после загрузки свойств
void ASandboxObject::PostLoadProperties() {
}

// Обработка размещения в мире
void ASandboxObject::OnPlaceToWorld() {
}

// Расчет позиции при размещении в мире
bool ASandboxObject::PlaceToWorldClcPosition(const UWorld* World, const FVector& SourcePos, const FRotator& SourceRotation, const FHitResult& Res, FVector& Location, FRotator& Rotation, bool bFinal) const {
	Location = Res.Location;
	Rotation.Pitch = 0;
	Rotation.Roll = 0;
	Rotation.Yaw = SourceRotation.Yaw;
	return true;
}

// Настройка сетевой репликации
void ASandboxObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASandboxObject, SandboxNetUid);
}

// Получение меша маркера
const UStaticMeshComponent* ASandboxObject::GetMarkerMesh() const {
	return SandboxRootMesh;
}

// Включение физики
void ASandboxObject::EnablePhysics() {
	if (SandboxRootMesh) {
		SandboxRootMesh->SetSimulatePhysics(true);
	}
}

// Получение типа скелетного модуля
int ASandboxSkeletalModule::GetSandboxTypeId() const {
	return 500;
}

// Получение позы ног
void ASandboxSkeletalModule::GetFootPose(FRotator& LeftFootRotator, FRotator& RightFootRotator) {
	LeftFootRotator = FootRotator;
	RightFootRotator = FRotator(-FootRotator.Pitch, -FootRotator.Yaw, FootRotator.Roll);
}

// Получение параметра воздействия
float ASandboxSkeletalModule::GetAffectParam(const FString& ParamName) const {
	if (AffectParamMap.Contains(ParamName)) {
		return AffectParamMap[ParamName];
	}
	return 1;
}

// Шаблонная функция для получения первого компонента по имени
template<class T>
T* GetFirstComponentByName(AActor* Actor, FString ComponentName) {
	TArray<T*> Components;
	Actor->GetComponents<T>(Components);
	for (T* Component : Components) {
		if (Component->GetName() == ComponentName)
			return Component;
	}
	return nullptr;
}

// Получение содержимого контейнера
TArray<ASandboxObject*> ASandboxObjectUtils::GetContainerContent(AActor* AnyActor, const FString& Name) {
	TArray<ASandboxObject*> Result;

	UContainerComponent* Container = GetFirstComponentByName<UContainerComponent>(AnyActor, Name);
	if (Container) {
		TArray<uint64> ObjList = Container->GetAllObjects();
		for (uint64 ClassId : ObjList) {
			ASandboxObject* Obj = ASandboxLevelController::GetDefaultSandboxObject(ClassId);
			if (Obj) {
				Result.Add(Obj);
			}
		}
	}

	return Result;
}

// Конструктор типа урона
USandboxDamageType::USandboxDamageType(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
}