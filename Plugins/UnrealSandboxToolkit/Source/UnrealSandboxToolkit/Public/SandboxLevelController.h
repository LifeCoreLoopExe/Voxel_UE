#pragma once // Защита от множественного включения этого заголовочного файла

#include "Engine.h" // Подключение основного заголовочного файла движка Unreal Engine
#include "GameFramework/Actor.h" // Подключение заголовочного файла для класса Actor
#include "SandboxObjectMap.h" // Подключение заголовочного файла для карты объектов песочницы
#include "SandboxLevelController.generated.h" // Генерация кода для этого заголовочного файла

// Структура для временного хранения стека контейнера
USTRUCT() 
struct FTempContainerStack {
	GENERATED_BODY() // Генерация тела структуры

	UPROPERTY() 
	int SlotId; // Идентификатор слота

	UPROPERTY() 
	FContainerStack Stack; // Стек контейнера
};

// Структура для описания объектов песочницы
USTRUCT() 
struct UNREALSANDBOXTOOLKIT_API FSandboxObjectDescriptor {
	GENERATED_BODY() // Генерация тела структуры

	UPROPERTY() 
	FTransform Transform; // Трансформация объекта (позиция, вращение, масштаб)

	UPROPERTY() 
	uint64 ClassId; // Идентификатор класса объекта

	UPROPERTY() 
	FString NetUid; // Сетевая уникальная идентификация объекта

	UPROPERTY() 
	int TypeId; // Идентификатор типа объекта

	UPROPERTY() 
	TMap<FString, FString> PropertyMap; // Карта свойств объекта

	UPROPERTY() 
	TArray<FTempContainerStack> Container; // Массив временных стеков контейнеров

	// Метод для создания дескриптора объекта на основе экземпляра ASandboxObject
	static FSandboxObjectDescriptor MakeObjDescriptor(ASandboxObject* SandboxObject);
};

// Класс контроллера уровня песочницы, наследующий от AActor
UCLASS() 
class UNREALSANDBOXTOOLKIT_API ASandboxLevelController : public AActor {
	GENERATED_BODY() // Генерация тела класса

public:
	ASandboxLevelController(); // Конструктор класса

	virtual void BeginPlay() override; // Переопределение метода BeginPlay для инициализации при старте игры

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; // Переопределение метода EndPlay для обработки завершения игры

	virtual void Tick(float DeltaSeconds) override; // Переопределение метода Tick для обновления состояния контроллера каждый кадр

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit") 
	FString MapName; // Имя карты

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit") 
	USandboxObjectMap* ObjectMap; // Указатель на карту объектов песочницы

	//TODO remove
	TSubclassOf<ASandboxObject> GetSandboxObjectByClassId(int32 ClassId); // Метод для получения класса объекта по его идентификатору класса

	ASandboxObject* GetSandboxObject(uint64 ClassId); // Метод для получения объекта песочницы по его идентификатору класса

	static ASandboxObject* GetDefaultSandboxObject(uint64 ClassId); // Статический метод для получения стандартного объекта песочницы по его идентификатору класса

	static ASandboxLevelController* GetInstance(); // Статический метод для получения экземпляра контроллера уровня

	void PrepareMetaData(); // Метод подготовки метаданных уровня

	virtual ASandboxObject* SpawnSandboxObject(const int ClassId, const FTransform& Transform, const FString& SandboxNetUid = ""); 
    // Метод для создания объекта песочницы с заданным классом и трансформацией

	virtual bool RemoveSandboxObject(ASandboxObject* Obj); // Метод для удаления объекта песочницы

	ASandboxObject* GetObjectByNetUid(FString NetUid); // Метод для получения объекта по его сетевому уникальному идентификатору (NetUid)

	ASandboxObject* SpawnPreparedObject(const FSandboxObjectDescriptor& ObjDesc); 
    // Метод для создания подготовленного объекта на основе дескриптора

	ASandboxEffect* SpawnEffect(const int32 EffectId, const FTransform& Transform); 
    // Метод для создания эффекта на основе его идентификатора и трансформации

protected:
	TMap<FString, TSubclassOf<ASandboxObject>> ObjectMapByClassName; // Карта классов объектов по их именам

	//TMap<uint64, TSubclassOf<ASandboxObject>> ObjectMapById; 

	virtual void SaveLevelJson(); // Виртуальный метод для сохранения уровня в формате JSON

	virtual void SaveLevelJsonExt(TSharedRef<TJsonWriter<TCHAR>> JsonWriter); 
    // Виртуальный метод для расширенного сохранения уровня в формате JSON с использованием JSON-писателя

	virtual void LoadLevelJson(); // Виртуальный метод для загрузки уровня из формата JSON

	virtual void LoadLevelJsonExt(TSharedPtr<FJsonObject> JsonParsed); 
    // Виртуальный метод для расширенной загрузки уровня из формата JSON с использованием разобранного JSON-объекта 

	void SaveObject(TSharedRef<TJsonWriter<TCHAR>> JsonWriter, const FSandboxObjectDescriptor& ObjDesc); 
    // Метод для сохранения объекта в формате JSON на основе дескриптора 

	virtual void SpawnPreparedObjects(const TArray<FSandboxObjectDescriptor>& ObjDescList); 
    // Виртуальный метод для создания подготовленных объектов на основе списка дескрипторов 

	void SavePreparedObjects(const TArray<FSandboxObjectDescriptor>& ObjDescList); 
    // Метод для сохранения подготовленных объектов в формате JSON 

	virtual void PrepareObjectForSave(TArray<FSandboxObjectDescriptor>& ObjDescList); 
    // Виртуальный метод подготовки объектов к сохранению в формате JSON 

	FString GetNewUid() const;  // Метод генерации нового уникального идентификатора 

private:
	bool bIsMetaDataReady = false;  // Флаг готовности метаданных 

	static TMap<int32, TSubclassOf<ASandboxObject>> ObjectMapById;  // Статическая карта классов объектов по их идентификаторам 

	uint64 ObjectCounter;  // Счетчик объектов 

	TMap<FString, ASandboxObject*> GlobalObjectMap;  // Глобальная карта объектов 

	static ASandboxLevelController* StaticSelf;  // Статический указатель на экземпляр контроллера уровня 
};
