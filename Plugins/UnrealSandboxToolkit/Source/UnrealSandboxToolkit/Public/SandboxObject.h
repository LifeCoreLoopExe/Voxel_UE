#pragma once

// Подключаем необходимые заголовочные файлы движка Unreal Engine
#include "Engine.h"
#include "GameFramework/Actor.h"
#include "ContainerComponent.h"
#include "Engine/DamageEvents.h"
#include "SandboxObject.generated.h"

// Определяем порог урона, при котором включается физика объекта
#define DAMAGE_ENABLE_PHYS_THRESHOLD 1.f

// Основной класс игрового объекта песочницы
// UCLASS указывает, что класс можно использовать в Blueprint и он доступен для создания
UCLASS(BlueprintType, Blueprintable)
class UNREALSANDBOXTOOLKIT_API ASandboxObject : public AActor {
	GENERATED_BODY()
	
public:	
	// Конструктор класса
	ASandboxObject();

	// Компонент статической модели - основная 3D модель объекта
	UPROPERTY(Category = StaticMeshActor, VisibleAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true"))
	UStaticMeshComponent* SandboxRootMesh;

	// Текстура иконки для отображения в инвентаре
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	UTexture2D* IconTexture;

	// Уникальный ID класса объекта
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	uint64 SandboxClassId;

	// Сетевой ID объекта (реплицируется по сети)
	UPROPERTY(Replicated)
	FString SandboxNetUid;

	// Можно ли складывать объекты в стопку
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	bool bStackable;

	// Максимальный размер стопки
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	uint32 MaxStackSize;

	// Карта пользовательских свойств объекта
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	TMap<FString, FString> PropertyMap;

	// Можно ли размещать объект в мире
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	bool bCanPlaceSandboxObject;

	// Порог урона для включения физики
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float DamageEnablePhysThreshold = DAMAGE_ENABLE_PHYS_THRESHOLD;

protected:
	// Вызывается при старте игры
	virtual void BeginPlay() override;

	// Обработчик события, когда объект перестает двигаться
	UFUNCTION()
	void OnSleep(UPrimitiveComponent* SleepingComponent, FName BoneName);

public:
	// Функция получения урона объектом
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	// Геттеры для различных свойств объекта
	FString GetSandboxNetUid() const;
	uint64 GetSandboxClassId() const;
	virtual FString GetSandboxName();
	virtual int GetSandboxTypeId() const;
	virtual int GetMaxStackSize();
	virtual UTexture2D* GetSandboxIconTexture();

	// Функции для работы с объектом в инвентаре
	virtual void TickInInventoryActive(float DeltaTime, UWorld* World, const FHitResult& HitResult);
	virtual void ActionInInventoryActive(UWorld* World, const FHitResult& HitResult);

	// Функции взаимодействия с объектом
	virtual bool IsInteractive(const APawn* Source = nullptr);
	virtual void MainInteraction(const APawn* Source = nullptr);
	virtual bool CanTake(const AActor* Actor = nullptr) const;

	// Вызывается при изменении террейна
	virtual void OnTerrainChange();

	// Включает физику объекта
	void EnablePhysics();
    
	// Получает компонент контейнера по имени
	UContainerComponent* GetContainer(const FString& Name);

	// Шаблонная функция для поиска компонента по имени
	template<class T>
	T* GetFirstComponentByName(FString ComponentName) {
		TArray<T*> Components;
		GetComponents<T>(Components);
		for (T* Component : Components) {
			if (Component->GetName() == ComponentName)
				return Component;
		}
		return nullptr;
	}

	// Функции для работы со свойствами объекта
	void SetProperty(FString Key, FString Value);
	FString GetProperty(FString Key) const;
	void RemoveProperty(FString Key);

	// Функции жизненного цикла объекта
	virtual void PostLoadProperties();
	virtual void OnPlaceToWorld();
	virtual bool PlaceToWorldClcPosition(const UWorld* World, const FVector& SourcePos, const FRotator& SourceRotation, const FHitResult& Res, FVector& Location, FRotator& Rotation, bool bFinal = false) const;
	virtual const UStaticMeshComponent* GetMarkerMesh() const;
};

// Интерфейс для носимых предметов (пока пустой)
class IWearable {
public:

};

// Класс для скелетных модулей (например, для одежды персонажа)
UCLASS()
class UNREALSANDBOXTOOLKIT_API ASandboxSkeletalModule : public ASandboxObject, public IWearable {
	GENERATED_BODY()

public:
	// Имя привязки скелетной модели
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	FName SkMeshBindName;

	// Скелетная модель
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	USkeletalMesh* SkeletalMesh;

	// Настройки для изменения позы ног
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	bool bModifyFootPose;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	FRotator FootRotator;

	// Карты для морфинга (изменения формы) модели
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	TMap<FString, float> MorphMap;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	TMap<FString, float> ParentMorphMap;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	TMap<FString, float> AffectParamMap;

	// Функции для работы с параметрами модуля
	virtual float GetAffectParam(const FString& ParamName) const;
	int GetSandboxTypeId() const override;
	void GetFootPose(FRotator& LeftFootRotator, FRotator& RightFootRotator);
};

// Утилитный класс для работы с объектами песочницы
class UNREALSANDBOXTOOLKIT_API ASandboxObjectUtils {
public:
	// Получает содержимое контейнера по имени
	static TArray<ASandboxObject*> GetContainerContent(AActor* AnyActor, const FString& Name);
};

// Класс типа урона для песочницы
UCLASS()
class UNREALSANDBOXTOOLKIT_API USandboxDamageType : public UDamageType {
	GENERATED_UCLASS_BODY()

public:
	// Множители разных типов урона
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float FireDamageFactor = 1.f;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float ExplosionDamageFactor = 1.f;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float HitDamageFactor = 1.f;
};
