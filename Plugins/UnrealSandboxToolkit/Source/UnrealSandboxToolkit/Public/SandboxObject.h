#pragma once // Указание компилятору включить этот файл только один раз

#include "Engine.h" // Подключение основного заголовочного файла движка Unreal Engine
#include "GameFramework/Actor.h" // Подключение заголовочного файла для класса AActor
#include "ContainerComponent.h" // Подключение заголовочного файла для компонента контейнера
#include "Engine/DamageEvents.h" // Подключение заголовочного файла для событий повреждений
#include "SandboxObject.generated.h" // Генерация заголовочного файла для класса ASandboxObject

#define DAMAGE_ENABLE_PHYS_THRESHOLD 1.f // Определение константы порога включения физики для повреждений

UCLASS(BlueprintType, Blueprintable) // Определение класса ASandboxObject, доступного в Blueprints
class UNREALSANDBOXTOOLKIT_API ASandboxObject : public AActor { // Определение класса ASandboxObject, наследующего от AActor
	GENERATED_BODY() // Генерация тела класса
	
public:	
	ASandboxObject(); // Конструктор класса ASandboxObject

	//UPROPERTY(EditAnywhere, Category = "Sandbox") 
	UPROPERTY(Category = StaticMeshActor, VisibleAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true")) // Свойство SandboxRootMesh, видимое в редакторе и доступное для чтения в Blueprints
	UStaticMeshComponent* SandboxRootMesh; // Корневой статический меш объекта песочницы

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство IconTexture, редактируемое в редакторе в категории "Sandbox"
	UTexture2D* IconTexture; // Текстура иконки объекта песочницы

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство SandboxClassId, редактируемое в редакторе в категории "Sandbox"
	uint64 SandboxClassId; // Идентификатор класса объекта песочницы

	UPROPERTY(Replicated) // Свойство SandboxNetUid с репликацией (синхронизация между клиентом и сервером)
	FString SandboxNetUid; // Сетевой уникальный идентификатор объекта песочницы

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство bStackable, редактируемое в редакторе в категории "Sandbox"
	bool bStackable; // Флаг указывающий, можно ли складывать объекты песочницы

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство MaxStackSize, редактируемое в редакторе в категории "Sandbox"
	uint32 MaxStackSize; // Максимальный размер стека объектов песочницы

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство PropertyMap, редактируемое в редакторе в категории "Sandbox"
	TMap<FString, FString> PropertyMap; // Карта свойств объекта песочницы (имя свойства -> значение)

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство bCanPlaceSandboxObject, редактируемое в редакторе в категории "Sandbox"
	bool bCanPlaceSandboxObject; // Флаг указывающий, можно ли разместить объект песочницы

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство DamageEnablePhysThreshold, редактируемое в редакторе в категории "Sandbox"
	float DamageEnablePhysThreshold = DAMAGE_ENABLE_PHYS_THRESHOLD; // Порог включения физики при повреждении

protected:
	virtual void BeginPlay() override; // Переопределение метода BeginPlay для инициализации при старте игры

	UFUNCTION() // Объявление функции OnSleep
	void OnSleep(UPrimitiveComponent* SleepingComponent, FName BoneName); // Метод обработки сна компонента

	//UFUNCTION() 
	//void OnTakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser); // Закомментированный метод обработки радиального урона

public:

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser); // Метод обработки получения урона

	FString GetSandboxNetUid() const; // Метод получения сетевого уникального идентификатора объекта песочницы

	uint64 GetSandboxClassId() const; // Метод получения идентификатора класса объекта песочницы

	virtual FString GetSandboxName(); // Виртуальный метод получения имени объекта песочницы

	virtual int GetSandboxTypeId() const; // Виртуальный метод получения идентификатора типа объекта песочницы

	virtual int GetMaxStackSize(); // Виртуальный метод получения максимального размера стека объектов песочницы
	
	virtual UTexture2D* GetSandboxIconTexture(); // Виртуальный метод получения текстуры иконки объекта песочницы

	virtual void TickInInventoryActive(float DeltaTime, UWorld* World, const FHitResult& HitResult); // Виртуальный метод обновления состояния объекта в активном инвентаре

	virtual void ActionInInventoryActive(UWorld* World, const FHitResult& HitResult); // Виртуальный метод выполнения действия в активном инвентаре

	virtual bool IsInteractive(const APawn* Source = nullptr); // Виртуальный метод проверки взаимодействия с объектом

	virtual void MainInteraction(const APawn* Source = nullptr); // Виртуальный метод основной интерактивности с объектом

	virtual bool CanTake(const AActor* Actor = nullptr) const; // Виртуальный метод проверки возможности взять объект 

	virtual void OnTerrainChange(); // Виртуальный метод обработки изменения местности 

	void EnablePhysics(); // Метод включения физики для объекта
    
	UContainerComponent* GetContainer(const FString& Name); // Метод получения контейнера по имени 

	template<class T>  // Шаблонный метод для получения первого компонента по имени 
	T* GetFirstComponentByName(FString ComponentName) { 
		TArray<T*> Components;  // Массив компонентов типа T 
		GetComponents<T>(Components);  // Получение всех компонентов типа T 
		for (T* Component : Components) {  // Цикл по всем компонентам 
			if (Component->GetName() == ComponentName)  // Если имя компонента совпадает с заданным 
				return Component;  // Возврат найденного компонента 
		}

		return nullptr;  // Возврат nullptr если компонент не найден 
	}

	void SetProperty(FString Key, FString Value);  // Метод установки свойства по ключу и значению 

	FString GetProperty(FString Key) const;  // Метод получения свойства по ключу 

	void RemoveProperty(FString Key);  // Метод удаления свойства по ключу 

	virtual void PostLoadProperties();  // Виртуальный метод обработки свойств после загрузки 

	virtual void OnPlaceToWorld();  // Виртуальный метод обработки размещения объекта в мире 

	virtual bool PlaceToWorldClcPosition(const UWorld* World, const FVector& SourcePos, const FRotator& SourceRotation, const FHitResult& Res, FVector& Location, FRotator& Rotation, bool bFinal = false) const;  // Виртуальный метод вычисления позиции размещения объекта в мире 

	virtual const UStaticMeshComponent* GetMarkerMesh() const;  // Виртуальный метод получения меша маркера 
};

//TODO
class IWearable {  // Объявление интерфейса IWearable
public:
};

UCLASS()  // Определение класса ASandboxSkeletalModule
class UNREALSANDBOXTOOLKIT_API ASandboxSkeletalModule : public ASandboxObject, public IWearable {  // Определение класса ASandboxSkeletalModule как наследника ASandboxObject и IWearable
	GENERATED_BODY()  // Генерация тела класса

public:

	UPROPERTY(EditAnywhere, Category = "Sandbox")  // Свойство SkMeshBindName редактируемое в редакторе в категории "Sandbox"
	FName SkMeshBindName;  // Имя привязки скелетного меша 

	UPROPERTY(EditAnywhere, Category = "Sandbox")  // Свойство SkeletalMesh редактируемое в редакторе в категории "Sandbox"
	USkeletalMesh* SkeletalMesh;  // Скелетный меш 

	UPROPERTY(EditAnywhere, Category = "Sandbox")  // Свойство bModifyFootPose редактируемое в редакторе в категории "Sandbox"
	bool bModifyFootPose;  // Флаг указывающий на изменение позы ноги 

	UPROPERTY(EditAnywhere, Category = "Sandbox")  // Свойство FootRotator редактируемое в редакторе в категории "Sandbox"
	FRotator FootRotator;  // Ротация ноги 

	UPROPERTY(EditAnywhere, Category = "Sandbox")  // Свойство MorphMap редактируемое в редакторе в категории "Sandbox"
	TMap<FString, float> MorphMap;  // Карта морфов (имя морфа -> значение)

	UPROPERTY(EditAnywhere, Category = "Sandbox")  // Свойство ParentMorphMap редактируемое в редакторе в категории "Sandbox"
	TMap<FString, float> ParentMorphMap;  // Карта родительских морфов (имя морфа -> значение)

	UPROPERTY(EditAnywhere, Category = "Sandbox")  // Свойство AffectParamMap редактируемое в редакторе в категории "Sandbox"
	TMap<FString, float> AffectParamMap;  // Карта параметров влияния (имя параметра -> значение)

	virtual float GetAffectParam(const FString& ParamName) const;  // Виртуальный метод получения параметра влияния по имени 

	int GetSandboxTypeId() const override;  // Переопределение метода получения идентификатора типа объекта песочницы 

	void GetFootPose(FRotator& LeftFootRotator, FRotator& RightFootRotator);  // Метод получения позы ног 
};


class UNREALSANDBOXTOOLKIT_API ASandboxObjectUtils {  // Объявление класса утилит для объектов песочницы
public:

	static TArray<ASandboxObject*> GetContainerContent(AActor* AnyActor, const FString& Name);  // Статический метод для получения содержимого контейнера по имени
};

UCLASS()  // Определение класса USandboxDamageType
class UNREALSANDBOXTOOLKIT_API USandboxDamageType : public UDamageType {  // Определение класса USandboxDamageType как наследника UDamageType
	GENERATED_UCLASS_BODY()  // Генерация тела класса


public:

	UPROPERTY(EditAnywhere, Category = "Sandbox")  // Свойство FireDamageFactor редактируемое в редакторе в категории "Sandbox"
	float FireDamageFactor = 1.f;  // Фактор урона от огня 

	UPROPERTY(EditAnywhere, Category = "Sandbox")  // Свойство ExplosionDamageFactor редактируемое в редакторе в категории "Sandbox"
	float ExplosionDamageFactor = 1.f;  // Фактор взрывного урона 

	UPROPERTY(EditAnywhere, Category = "Sandbox")  // Свойство HitDamageFactor редактируемое в редакторе в категории "Sandbox"
	float HitDamageFactor = 1.f;  // Фактор урона от удара 
};
