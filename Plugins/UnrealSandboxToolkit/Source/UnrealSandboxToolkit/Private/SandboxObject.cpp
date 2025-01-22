#include "SandboxObject.h" // Подключение заголовочного файла SandboxObject.h
#include "SandboxLevelController.h" // Подключение заголовочного файла SandboxLevelController.h
#include "Net/UnrealNetwork.h" // Подключение заголовочного файла UnrealNetwork для сетевого взаимодействия

ASandboxObject::ASandboxObject() {
	PrimaryActorTick.bCanEverTick = true; // Разрешение тикания актора
	SandboxRootMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SandboxRootMesh")); // Создание корневого компонента меша
	MaxStackSize = 100; // Максимальный размер стека
	bStackable = true; // Разрешение на стекание объектов
	RootComponent = SandboxRootMesh; // Установка корневого компонента
	bReplicates = true; // Разрешение на репликацию объекта в сети
	SandboxNetUid = 0; // Идентификатор сетевого объекта
	bCanPlaceSandboxObject = true; // Разрешение на размещение объекта песочницы
	SandboxRootMesh->SetLinearDamping(1.f); // Установка линейного демпфирования
	SandboxRootMesh->SetAngularDamping(10.f); // Установка углового демпфирования
	SandboxRootMesh->BodyInstance.bGenerateWakeEvents = true; // Разрешение на генерацию событий пробуждения
}

static const FString DefaultSandboxObjectName = FString(TEXT("Sandbox object")); // Имя объекта песочницы по умолчанию

void ASandboxObject::BeginPlay() {
	Super::BeginPlay(); // Вызов функции BeginPlay родительского класса
	SandboxRootMesh->OnComponentSleep.AddDynamic(this, &ASandboxObject::OnSleep); // Подписка на событие сна компонента
	//SandboxRootMesh->OnTakeRadialDamage.AddDynamic(this, &ASandboxObject::OnTakeRadialDamage); // Подписка на событие радиального урона (закомментировано)
	//SandboxRootMesh->OnTakeAnyDamage.AddDynamic(this, &ASandboxObject::OnTakeRadialDamage); // Подписка на событие любого урона (закомментировано)
	//SandboxRootMesh->OnTakeAnyDamage.AddDynamic(this, &ASandboxObject::OnTakeRadialDamage); // Повторная подписка на событие любого урона (закомментировано)
}

void ASandboxObject::OnSleep(UPrimitiveComponent* SleepingComponent, FName BoneName) {
	//UE_LOG(LogTemp, Warning, TEXT("OnSleep")); // Логирование события сна (закомментировано)
	SandboxRootMesh->SetSimulatePhysics(false); // Остановка симуляции физики для корневого компонента
}

float ASandboxObject::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) {
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser); // Вызов функции получения урона родительского класса
	
	if (DamageEnablePhysThreshold > 0 && ActualDamage > DamageEnablePhysThreshold) { // Проверка на включение физики при превышении порога урона
		EnablePhysics(); // Включение физики
	}

	return ActualDamage; // Возврат фактического урона
}

//void ASandboxObject::OnTakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) {
//} // Обработка радиального урона (закомментировано)

FString ASandboxObject::GetSandboxName() {
	return DefaultSandboxObjectName; // Возврат имени объекта песочницы
}

FString ASandboxObject::GetSandboxNetUid() const {
	return SandboxNetUid; // Возврат сетевого идентификатора объекта песочницы
}

uint64 ASandboxObject::GetSandboxClassId() const {
	return SandboxClassId; // Возврат идентификатора класса объекта песочницы
}

int ASandboxObject::GetSandboxTypeId() const {
	return 0; // Возврат типа объекта песочницы (по умолчанию 0)
}

int ASandboxObject::GetMaxStackSize() {
	if (!bStackable) { // Проверка, может ли объект стекаться
		return 1; // Если нет, возвращаем 1
	}

	return MaxStackSize; // Возврат максимального размера стека
}

UTexture2D* ASandboxObject::GetSandboxIconTexture() {
	return NULL; // Возврат текстуры иконки объекта песочницы (по умолчанию NULL)
}

void ASandboxObject::TickInInventoryActive(float DeltaTime, UWorld* World, const FHitResult& HitResult) {
	// Функция обновления, когда объект активен в инвентаре (пока не реализована)
}

void ASandboxObject::ActionInInventoryActive(UWorld* World, const FHitResult& HitResult) {
	// Функция действия, когда объект активен в инвентаре (пока не реализована)
}

bool ASandboxObject::CanTake(const AActor* Actor) const {
	if (SandboxRootMesh->IsSimulatingPhysics()) { // Проверка, симулирует ли физику корневой компонент
		return false; // Если да, нельзя взять объект
	}
	
	TArray<UContainerComponent*> ContainerComponents; // Массив для хранения компонентов контейнера
	GetComponents<UContainerComponent>(ContainerComponents); // Получение всех компонентов контейнера

	for (const auto& ContainerComponent : ContainerComponents) { // Перебор всех компонентов контейнера
		if (!ContainerComponent->IsEmpty()) { // Если компонент не пустой
			return false; // Нельзя взять объект
		}
	}
	
	return true; // Объект можно взять
}

void ASandboxObject::OnTerrainChange() {
	SandboxRootMesh->SetSimulatePhysics(true); // Включение симуляции физики для корневого компонента при изменении местности
}

UContainerComponent* ASandboxObject::GetContainer(const FString& Name) { 
	return GetFirstComponentByName<UContainerComponent>(Name); // Получение контейнера по имени
}

bool ASandboxObject::IsInteractive(const APawn* Source) {
	return false; // Объект не интерактивен
}

void ASandboxObject::MainInteraction(const APawn* Source) {
	// Никаких действий (пока не реализовано)
}

void ASandboxObject::SetProperty(FString Key, FString Value) {
	PropertyMap.Add(Key, Value); // Установка свойства объекта
}

FString ASandboxObject::GetProperty(FString Key) const {
	return PropertyMap.FindRef(Key); // Получение значения свойства по ключу
}

void ASandboxObject::RemoveProperty(FString Key) {
	PropertyMap.Remove(Key); // Удаление свойства по ключу
}

void ASandboxObject::PostLoadProperties() {
	// Обработка после загрузки свойств (пока не реализовано)
}

void ASandboxObject::OnPlaceToWorld() {
	// Обработка при размещении в мире (пока не реализовано)
}

bool ASandboxObject::PlaceToWorldClcPosition(const UWorld* World, const FVector& SourcePos, const FRotator& SourceRotation, const FHitResult& Res, FVector& Location, FRotator& Rotation, bool bFinal) const {
	Location = Res.Location; // Установка местоположения
	Rotation.Pitch = 0; // Установка угла поворота по оси Pitch
	Rotation.Roll = 0; // Установка угла поворота по оси Roll
	Rotation.Yaw = SourceRotation.Yaw; // Установка угла поворота по оси Yaw из исходного значения
	return true; // Возврат успешного выполнения
}

void ASandboxObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps); // Вызов функции получения реплицируемых свойств родительского класса
	DOREPLIFETIME(ASandboxObject, SandboxNetUid); // Добавление свойства к репликации
}

const UStaticMeshComponent* ASandboxObject::GetMarkerMesh() const {
	return SandboxRootMesh; // Возврат корневого компонента меша
}

void ASandboxObject::EnablePhysics() {
	if (SandboxRootMesh) { // Проверка наличия корневого компонента
		SandboxRootMesh->SetSimulatePhysics(true); // Включение симуляции физики
	}
}

int ASandboxSkeletalModule::GetSandboxTypeId() const {
	return 500; // Возврат идентификатора типа для скелетного модуля
}

void ASandboxSkeletalModule::GetFootPose(FRotator& LeftFootRotator, FRotator& RightFootRotator) {
	LeftFootRotator = FootRotator; // Установка поворота левой ноги
	RightFootRotator = FRotator(-FootRotator.Pitch, -FootRotator.Yaw, FootRotator.Roll); // Установка поворота правой ноги с инвертированием
}

float ASandboxSkeletalModule::GetAffectParam(const FString& ParamName) const {
	if (AffectParamMap.Contains(ParamName)) { // Проверка наличия параметра в карте параметров
		return AffectParamMap[ParamName]; // Возврат значения параметра
	}

	return 1; // Возврат значения по умолчанию
}

template<class T>
T* GetFirstComponentByName(AActor* Actor, FString ComponentName) {
	TArray<T*> Components; // Массив для хранения компонентов
	Actor->GetComponents<T>(Components); // Получение всех компонентов указанного типа
	for (T* Component : Components) { // Перебор всех компонентов
		if (Component->GetName() == ComponentName) // Проверка имени компонента
			return Component; // Возврат найденного компонента
	}

	return nullptr; // Возврат nullptr, если компонент не найден
}

TArray<ASandboxObject*> ASandboxObjectUtils::GetContainerContent(AActor* AnyActor, const FString& Name) {
	TArray<ASandboxObject*> Result; // Массив для хранения объектов песочницы

	UContainerComponent* Container = GetFirstComponentByName<UContainerComponent>(AnyActor, Name); // Получение контейнера по имени
	if (Container) { // Проверка наличия контейнера
		TArray<uint64> ObjList = Container->GetAllObjects(); // Получение всех объектов из контейнера
		for (uint64 ClassId : ObjList) { // Перебор всех идентификаторов классов объектов
			ASandboxObject* Obj = ASandboxLevelController::GetDefaultSandboxObject(ClassId); // Получение объекта песочницы по идентификатору класса
			if (Obj) { // Проверка наличия объекта
				Result.Add(Obj); // Добавление объекта в результат
			}
		}
	}

	return Result; // Возврат массива объектов песочницы
}

USandboxDamageType::USandboxDamageType(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	// Конструктор типа урона песочницы (пока не реализовано)
}
