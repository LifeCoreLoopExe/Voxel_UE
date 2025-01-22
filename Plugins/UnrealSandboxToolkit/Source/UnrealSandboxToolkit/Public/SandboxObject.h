
#pragma once

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "ContainerComponent.h"
#include "Engine/DamageEvents.h"
#include "SandboxObject.generated.h"



#define DAMAGE_ENABLE_PHYS_THRESHOLD 1.f



// Объявление класса ASandboxObject, наследуемого от AActor
UCLASS(BlueprintType, Blueprintable)
class UNREALSANDBOXTOOLKIT_API ASandboxObject : public AActor {
	GENERATED_BODY()

public:
	// Конструктор класса
	ASandboxObject();

	// Компонент статической меши (закомментировано)
	//UPROPERTY(EditAnywhere, Category = "Sandbox")
	// Компонент статической меши, видимый и доступный для редактирования в Blueprint
	UPROPERTY(Category = StaticMeshActor, VisibleAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true"))
	UStaticMeshComponent* SandboxRootMesh;

	// Текстура иконки объекта
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	UTexture2D* IconTexture;

	// Идентификатор класса объекта
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	uint64 SandboxClassId;

	// Сетевой идентификатор объекта
	UPROPERTY(Replicated)
	FString SandboxNetUid;

	// Флаг, указывающий, можно ли складывать объекты в стек
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	bool bStackable;

	// Максимальный размер стека объектов
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	uint32 MaxStackSize;

	// Карта свойств объекта
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	TMap<FString, FString> PropertyMap;

	// Флаг, указывающий, можно ли размещать объект в песочнице
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	bool bCanPlaceSandboxObject;

	// Порог повреждения для включения физики
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float DamageEnablePhysThreshold = DAMAGE_ENABLE_PHYS_THRESHOLD;

protected:
	// Метод, вызываемый при начале игры
	virtual void BeginPlay() override;

	// Метод, вызываемый при переходе компонента в спящий режим
	UFUNCTION()
	void OnSleep(UPrimitiveComponent* SleepingComponent, FName BoneName);

	// Метод, вызываемый при получении радиального урона (закомментировано)
	//UFUNCTION()
	//void OnTakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

public:
	// Метод для получения урона
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	// Метод для получения сетевого идентификатора объекта
	FString GetSandboxNetUid() const;

	// Метод для получения идентификатора класса объекта
	uint64 GetSandboxClassId() const;

	// Метод для получения имени объекта
	virtual FString GetSandboxName();

	// Метод для получения типа объекта
	virtual int GetSandboxTypeId() const;

	// Метод для получения максимального размера стека объектов
	virtual int GetMaxStackSize();

	// Метод для получения текстуры иконки объекта
	virtual UTexture2D* GetSandboxIconTexture();

	// Метод для обновления объекта в инвентаре
	virtual void TickInInventoryActive(float DeltaTime, UWorld* World, const FHitResult& HitResult);

	// Метод для выполнения действия в инвентаре
	virtual void ActionInInventoryActive(UWorld* World, const FHitResult& HitResult);

	// Метод для проверки, является ли объект интерактивным
	virtual bool IsInteractive(const APawn* Source = nullptr);

	// Метод для выполнения основного взаимодействия с объектом
	virtual void MainInteraction(const APawn* Source = nullptr);

	// Метод для проверки, можно ли взять объект
	virtual bool CanTake(const AActor* Actor = nullptr) const;

	// Метод для обработки изменения территории
	virtual void OnTerrainChange();

	// Метод для включения физики объекта
	void EnablePhysics();

	// Метод для получения контейнера по имени
	UContainerComponent* GetContainer(const FString& Name);

	// Шаблонный метод для получения первого компонента по имени
	template<class T>
	T* GetFirstComponentByName(FString ComponentName) {
		// Список компонентов
		TArray<T*> Components;
		// Получение всех компонентов объекта
		GetComponents<T>(Components);
		// Поиск компонента по имени
		for (T* Component : Components) {
			if (Component->GetName() == ComponentName)
				return Component;
		}

		// Возвращение nullptr, если компонент не найден
		return nullptr;
	}

	// Метод для установки свойства объекта
	void SetProperty(FString Key, FString Value);

	// Метод для получения свойства объекта
	FString GetProperty(FString Key) const;

	// Метод для удаления свойства объекта
	void RemoveProperty(FString Key);

	// Метод для загрузки свойств объекта после его создания
	virtual void PostLoadProperties();

	// Метод для обработки размещения объекта в мире
	virtual void OnPlaceToWorld();

	// Метод для расчета позиции размещения объекта в мире
	virtual bool PlaceToWorldClcPosition(const UWorld* World, const FVector& SourcePos, const FRotator& SourceRotation, const FHitResult& Res, FVector& Location, FRotator& Rotation, bool bFinal = false) const;

	// Метод для получения маркерной меши объекта
	virtual const UStaticMeshComponent* GetMarkerMesh() const;
};

// TODO: Реализация интерфейса IWearable
class IWearable {

public:

};

// Объявление класса ASandboxSkeletalModule, наследуемого от ASandboxObject и реализующего интерфейс IWearable
UCLASS()
class UNREALSANDBOXTOOLKIT_API ASandboxSkeletalModule : public ASandboxObject, public IWearable {
	GENERATED_BODY()

public:
	// Имя привязки скелетной меши
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	FName SkMeshBindName;

	// Скелетная меш
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	USkeletalMesh* SkeletalMesh;

	// Флаг, указывающий, нужно ли изменять позу ног
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	bool bModifyFootPose;

	// Ротация ног
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	FRotator FootRotator;

	// Карта морфинга
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	TMap<FString, float> MorphMap;

	// Карта морфинга родительского объекта
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	TMap<FString, float> ParentMorphMap;

	// Карта параметров влияния
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	TMap<FString, float> AffectParamMap;

	// Метод для получения параметра влияния
	virtual float GetAffectParam(const FString& ParamName) const;

	// Метод для получения типа объекта
	int GetSandboxTypeId() const override;

	// Метод для получения позы ног
	void GetFootPose(FRotator& LeftFootRotator, FRotator& RightFootRotator);

};

// Класс для утилит работы с объектами песочницы
class UNREALSANDBOXTOOLKIT_API ASandboxObjectUtils {

public:
	// Метод для получения содержимого контейнера
	static TArray<ASandboxObject*> GetContainerContent(AActor* AnyActor, const FString& Name);
};

// Объявление класса USandboxDamageType, наследуемого от UDamageType
UCLASS()
class UNREALSANDBOXTOOLKIT_API USandboxDamageType : public UDamageType {
	GENERATED_UCLASS_BODY()

public:
	// Коэффициент урона от огня
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float FireDamageFactor = 1.f;

	// Коэффициент урона от взрыва
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float ExplosionDamageFactor = 1.f;

	// Коэффициент урона от удара
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float HitDamageFactor = 1.f;
};
