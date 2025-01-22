#pragma once // Защита от множественного включения этого заголовочного файла

#include "Engine.h" // Подключение основного заголовочного файла движка Unreal Engine
#include "GameFramework/Actor.h" // Подключение заголовочного файла для класса Actor
#include "ContainerComponent.h" // Подключение заголовочного файла для компонента контейнера
#include "Engine/DamageEvents.h" // Подключение заголовочного файла для событий урона
#include "SandboxObject.generated.h" // Генерация кода для этого заголовочного файла

#define DAMAGE_ENABLE_PHYS_THRESHOLD 1.f // Определение порога активации физики при получении урона

// Класс для объектов песочницы, наследующий от AActor
UCLASS(BlueprintType, Blueprintable) 
class UNREALSANDBOXTOOLKIT_API ASandboxObject : public AActor { 
	GENERATED_BODY() // Генерация тела класса
	
public:	
	ASandboxObject(); // Конструктор класса

	// Основной компонент для визуализации объекта
	UPROPERTY(Category = StaticMeshActor, VisibleAnywhere, BlueprintReadOnly, 
		meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true"))
	UStaticMeshComponent* SandboxRootMesh; // Корневой статический меш объекта

	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	UTexture2D* IconTexture; // Текстура иконки объекта

	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	uint64 SandboxClassId; // Идентификатор класса объекта

	UPROPERTY(Replicated) 
	FString SandboxNetUid; // Сетевая уникальная идентификация объекта

	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	bool bStackable; // Флаг, указывающий, можно ли складывать объекты

	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	uint32 MaxStackSize; // Максимальный размер стека объектов

	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	TMap<FString, FString> PropertyMap; // Карта свойств объекта

	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	bool bCanPlaceSandboxObject; // Флаг, указывающий, можно ли разместить объект в мире

	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	float DamageEnablePhysThreshold = DAMAGE_ENABLE_PHYS_THRESHOLD; // Порог активации физики при получении урона

protected:
	virtual void BeginPlay() override; // Переопределение метода BeginPlay для инициализации при старте игры

	UFUNCTION() 
	void OnSleep(UPrimitiveComponent* SleepingComponent, FName BoneName); // Метод обработки сна компонента

public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser); 
    // Метод для обработки получения урона

	FString GetSandboxNetUid() const; // Метод для получения сетевой уникальной идентификации объекта

	uint64 GetSandboxClassId() const; // Метод для получения идентификатора класса объекта

	virtual FString GetSandboxName(); // Метод для получения имени объекта песочницы

	virtual int GetSandboxTypeId() const; // Метод для получения типа объекта песочницы

	virtual int GetMaxStackSize(); // Метод для получения максимального размера стека объектов
	
	virtual UTexture2D* GetSandboxIconTexture(); // Метод для получения текстуры иконки объекта

	virtual void TickInInventoryActive(float DeltaTime, UWorld* World, const FHitResult& HitResult); 
    // Метод для обработки обновления состояния в инвентаре активного объекта 

	virtual void ActionInInventoryActive(UWorld* World, const FHitResult& HitResult); 
    // Метод для выполнения действия с активным объектом в инвентаре 

	virtual bool IsInteractive(const APawn* Source = nullptr); 
    // Метод проверки возможности взаимодействия с объектом 

	virtual void MainInteraction(const APawn* Source = nullptr); 
    // Метод выполнения основного взаимодействия с объектом 

	virtual bool CanTake(const AActor* Actor = nullptr) const; 
    // Метод проверки возможности взять объект 

	virtual void OnTerrainChange();  // Метод обработки изменения местности 

	void EnablePhysics();  // Метод активации физики объекта
    
	UContainerComponent* GetContainer(const FString& Name);  // Метод получения контейнера по имени 

	template<class T>
	T* GetFirstComponentByName(FString ComponentName) {  // Шаблонный метод для получения первого компонента по имени
		TArray<T*> Components;
		GetComponents<T>(Components);  // Получение всех компонентов указанного типа
		for (T* Component : Components) {
			if (Component->GetName() == ComponentName)  // Сравнение имени компонента с заданным именем
				return Component;
		}

		return nullptr;  // Возвращает nullptr если компонент не найден
	}

	void SetProperty(FString Key, FString Value);  // Метод установки свойства по ключу и значению

	FString GetProperty(FString Key) const;  // Метод получения свойства по ключу 

	void RemoveProperty(FString Key);  // Метод удаления свойства по ключу 

	virtual void PostLoadProperties();  // Виртуальный метод постобработки свойств после загрузки 

	virtual void OnPlaceToWorld();  // Виртуальный метод обработки размещения в мире 

	virtual bool PlaceToWorldClcPosition(const UWorld* World, const FVector& SourcePos,
		const FRotator& SourceRotation, const FHitResult& Res,
		FVector& Location, FRotator& Rotation, bool bFinal = false) const;  
    /* Виртуальный метод для вычисления позиции размещения в мире */

	virtual const UStaticMeshComponent* GetMarkerMesh() const;  /* Виртуальный метод для получения меша маркера */
};

// Интерфейс для носимых объектов
class IWearable {
public:
};

// Класс для скелетных модулей объектов песочницы
UCLASS() 
class UNREALSANDBOXTOOLKIT_API ASandboxSkeletalModule : public ASandboxObject, public IWearable { 
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	FName SkMeshBindName;  /* Имя связывания скелетной сетки */

	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	USkeletalMesh* SkeletalMesh;  /* Скелетная сетка */

	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	bool bModifyFootPose;  /* Флаг изменения позы ног */

	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	FRotator FootRotator;  /* Поворот ног */

	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	TMap<FString, float> MorphMap;  /* Карта морфов */

	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	TMap<FString, float> ParentMorphMap;  /* Карта родительских морфов */

	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	TMap<FString, float> AffectParamMap;  /* Карта параметров воздействия */

	virtual float GetAffectParam(const FString& ParamName) const;  /* Метод получения параметра воздействия по имени */

	int GetSandboxTypeId() const override;  /* Переопределение метода получения типа объекта песочницы */

	void GetFootPose(FRotator& LeftFootRotator, FRotator& RightFootRotator);  
    /* Метод получения позы ног */
};

// Утилиты для работы с объектами песочницы
class UNREALSANDBOXTOOLKIT_API ASandboxObjectUtils {
public:
	static TArray<ASandboxObject*> GetContainerContent(AActor* AnyActor, const FString& Name);  
    /* Статический метод получения содержимого контейнера */
};

// Класс типа урона объектов песочницы
UCLASS() 
class UNREALSANDBOXTOOLKIT_API USandboxDamageType : public UDamageType { 
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	float FireDamageFactor = 1.f;  /* Коэффициент урона от огня */

	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	float ExplosionDamageFactor = 1.f;  /* Коэффициент урона от взрыва */

	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	float HitDamageFactor = 1.f;  /* Коэффициент урона от удара */
};
