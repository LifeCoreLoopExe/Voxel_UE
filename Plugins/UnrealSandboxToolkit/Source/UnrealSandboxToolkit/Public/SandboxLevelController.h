#pragma once // Указание компилятору включить этот файл только один раз

#include "Engine.h" // Подключение основного заголовочного файла движка Unreal Engine
#include "GameFramework/Actor.h" // Подключение заголовочного файла для класса AActor
#include "SandboxObjectMap.h" // Подключение заголовочного файла для карты объектов песочницы
#include "SandboxLevelController.generated.h" // Генерация заголовочного файла для класса ASandboxLevelController

USTRUCT() // Определение структуры FTempContainerStack
struct FTempContainerStack {
	GENERATED_BODY() // Генерация тела структуры

	UPROPERTY() // Свойство SlotId, идентификатор слота
	int SlotId; 

	UPROPERTY() // Свойство Stack, стек контейнера
	FContainerStack Stack; 
};


USTRUCT() // Определение структуры FSandboxObjectDescriptor
struct UNREALSANDBOXTOOLKIT_API FSandboxObjectDescriptor {
	GENERATED_BODY() // Генерация тела структуры

	UPROPERTY() // Свойство Transform, трансформация объекта
	FTransform Transform; 

	UPROPERTY() // Свойство ClassId, идентификатор класса объекта
	uint64 ClassId; 

	UPROPERTY() // Свойство NetUid, сетевой уникальный идентификатор объекта
	FString NetUid; 

	UPROPERTY() // Свойство TypeId, идентификатор типа объекта
	int TypeId; 

	UPROPERTY() // Свойство PropertyMap, карта свойств объекта
	TMap<FString, FString> PropertyMap; 

	UPROPERTY() // Свойство Container, массив временных стеков контейнеров
	TArray<FTempContainerStack> Container; 

	static FSandboxObjectDescriptor MakeObjDescriptor(ASandboxObject* SandboxObject); // Статический метод для создания дескриптора объекта песочницы
};


UCLASS() // Определение класса ASandboxLevelController, доступного в Blueprints
class UNREALSANDBOXTOOLKIT_API ASandboxLevelController : public AActor { // Определение класса ASandboxLevelController, наследующего от AActor
	GENERATED_BODY() // Генерация тела класса

public:
	ASandboxLevelController(); // Конструктор класса ASandboxLevelController

	virtual void BeginPlay() override; // Переопределение метода BeginPlay для инициализации при старте уровня

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; // Переопределение метода EndPlay для обработки завершения игры

	virtual void Tick(float DeltaSeconds) override; // Переопределение метода Tick для обновления состояния каждый кадр

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit") // Свойство MapName, редактируемое в редакторе в категории "UnrealSandbox Toolkit"
	FString MapName; 

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit") // Свойство ObjectMap, редактируемое в редакторе в категории "UnrealSandbox Toolkit"
	USandboxObjectMap* ObjectMap; 

	//TODO remove 
	TSubclassOf<ASandboxObject> GetSandboxObjectByClassId(int32 ClassId); // Метод для получения объекта песочницы по идентификатору класса

	ASandboxObject* GetSandboxObject(uint64 ClassId); // Метод для получения объекта песочницы по идентификатору класса (64-битный)

	static ASandboxObject* GetDefaultSandboxObject(uint64 ClassId); // Статический метод для получения стандартного объекта песочницы по идентификатору класса

	static ASandboxLevelController* GetInstance(); // Статический метод для получения экземпляра контроллера уровня

	void PrepareMetaData(); // Метод подготовки метаданных

	virtual ASandboxObject* SpawnSandboxObject(const int ClassId, const FTransform& Transform, const FString& SandboxNetUid = ""); // Виртуальный метод для создания объекта песочницы с заданным классом и трансформацией

	virtual bool RemoveSandboxObject(ASandboxObject* Obj); // Виртуальный метод для удаления объекта песочницы

	ASandboxObject* GetObjectByNetUid(FString NetUid); // Метод для получения объекта по сетевому уникальному идентификатору

	ASandboxObject* SpawnPreparedObject(const FSandboxObjectDescriptor& ObjDesc); // Метод для создания подготовленного объекта на основе дескриптора

	ASandboxEffect* SpawnEffect(const int32 EffectId, const FTransform& Transform); // Метод для создания эффекта по идентификатору эффекта и трансформации

protected:

	TMap<FString, TSubclassOf<ASandboxObject>> ObjectMapByClassName; // Карта объектов по имени класса

	//TMap<uint64, TSubclassOf<ASandboxObject>> ObjectMapById;  // Закомментированная карта объектов по идентификатору класса

	virtual void SaveLevelJson(); // Виртуальный метод для сохранения уровня в формате JSON

	virtual void SaveLevelJsonExt(TSharedRef<TJsonWriter<TCHAR>> JsonWriter); // Виртуальный метод для расширенного сохранения уровня в формате JSON

	virtual void LoadLevelJson(); // Виртуальный метод для загрузки уровня из формата JSON

	virtual void LoadLevelJsonExt(TSharedPtr<FJsonObject> JsonParsed); // Виртуальный метод для расширенной загрузки уровня из формата JSON

	void SaveObject(TSharedRef <TJsonWriter<TCHAR>> JsonWriter, const FSandboxObjectDescriptor& ObjDesc); // Метод сохранения объекта в формате JSON на основе дескриптора объекта

	virtual void SpawnPreparedObjects(const TArray<FSandboxObjectDescriptor>& ObjDescList); // Виртуальный метод для создания подготовленных объектов на основе списка дескрипторов объектов

	void SavePreparedObjects(const TArray<FSandboxObjectDescriptor>& ObjDescList); // Метод сохранения подготовленных объектов на основе списка дескрипторов объектов

	virtual void PrepareObjectForSave(TArray<FSandboxObjectDescriptor>& ObjDescList); // Виртуальный метод подготовки объектов к сохранению на основе списка дескрипторов объектов

	FString GetNewUid() const; // Метод получения нового уникального идентификатора (UID)

private:

	bool bIsMetaDataReady = false; // Флаг готовности метаданных (по умолчанию false)

	static TMap<int32, TSubclassOf<ASandboxObject>> ObjectMapById;  // Статическая карта объектов по идентификатору класса (32-битный)

	uint64 ObjectCounter;  // Счетчик объектов (64-битный)

	TMap<FString, ASandboxObject*> GlobalObjectMap;  // Глобальная карта объектов (имя -> объект)

	static ASandboxLevelController* StaticSelf;  // Статический указатель на экземпляр контроллера уровня
};
