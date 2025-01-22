#pragma once // Директива препроцессора, предотвращающая множественное включение этого файла

#include "Engine.h" // Подключение основных заголовочных файлов движка Unreal Engine
#include "GameFramework/Actor.h" // Подключение заголовочного файла для класса AActor
#include "SandboxObjectMap.h" // Подключение пользовательского заголовочного файла для класса SandboxObjectMap
#include "SandboxLevelController.generated.h" // Подключение сгенерированного файла для рефлексии Unreal Engine

USTRUCT() // Макрос для объявления структуры, доступной для рефлексии Unreal Engine
struct FTempContainerStack { // Объявление структуры FTempContainerStack
	GENERATED_BODY() // Макрос для генерации тела структуры

	UPROPERTY() // Макрос для объявления свойства, доступного для редактирования в редакторе Unreal
	int SlotId; // Идентификатор слота

	UPROPERTY() // Макрос для объявления свойства, доступного для редактирования в редакторе Unreal
	FContainerStack Stack; // Стек контейнера
};

USTRUCT() // Макрос для объявления структуры, доступной для рефлексии Unreal Engine
struct UNREALSANDBOXTOOLKIT_API FSandboxObjectDescriptor { // Объявление структуры FSandboxObjectDescriptor
	GENERATED_BODY() // Макрос для генерации тела структуры

	UPROPERTY() // Макрос для объявления свойства, доступного для редактирования в редакторе Unreal
	FTransform Transform; // Трансформация объекта

	UPROPERTY() // Макрос для объявления свойства, доступного для редактирования в редакторе Unreal
	uint64 ClassId; // Идентификатор класса

	UPROPERTY() // Макрос для объявления свойства, доступного для редактирования в редакторе Unreal
	FString NetUid; // Сетевой уникальный идентификатор

	UPROPERTY() // Макрос для объявления свойства, доступного для редактирования в редакторе Unreal
	int TypeId; // Идентификатор типа

	UPROPERTY() // Макрос для объявления свойства, доступного для редактирования в редакторе Unreal
	TMap<FString, FString> PropertyMap; // Карта свойств

	UPROPERTY() // Макрос для объявления свойства, доступного для редактирования в редакторе Unreal
	TArray<FTempContainerStack> Container; // Массив контейнеров

	static FSandboxObjectDescriptor MakeObjDescriptor(ASandboxObject* SandboxObject); // Статический метод для создания дескриптора объекта
};

UCLASS() // Макрос для объявления класса, доступного для рефлексии Unreal Engine
class UNREALSANDBOXTOOLKIT_API ASandboxLevelController : public AActor { // Объявление класса ASandboxLevelController, наследующегося от AActor
	GENERATED_BODY() // Макрос для генерации тела класса

public:
	ASandboxLevelController(); // Конструктор класса

	virtual void BeginPlay() override; // Метод, вызываемый при начале игры

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; // Метод, вызываемый при завершении игры

	virtual void Tick(float DeltaSeconds) override; // Метод, вызываемый каждый кадр

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit") // Макрос для объявления свойства, доступного для редактирования в редакторе Unreal
	FString MapName; // Имя карты

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit") // Макрос для объявления свойства, доступного для редактирования в редакторе Unreal
	USandboxObjectMap* ObjectMap; // Карта объектов

	//TODO remove
	TSubclassOf<ASandboxObject> GetSandboxObjectByClassId(int32 ClassId); // Метод для получения объекта песочницы по идентификатору класса

	ASandboxObject* GetSandboxObject(uint64 ClassId); // Метод для получения объекта песочницы по идентификатору класса

	static ASandboxObject* GetDefaultSandboxObject(uint64 ClassId); // Статический метод для получения объекта песочницы по умолчанию по идентификатору класса

	static ASandboxLevelController* GetInstance(); // Статический метод для получения экземпляра контроллера уровня песочницы

	void PrepareMetaData(); // Метод для подготовки метаданных

	virtual ASandboxObject* SpawnSandboxObject(const int ClassId, const FTransform& Transform, const FString& SandboxNetUid = ""); // Метод для создания объекта песочницы

	virtual bool RemoveSandboxObject(ASandboxObject* Obj); // Метод для удаления объекта песочницы

	ASandboxObject* GetObjectByNetUid(FString NetUid); // Метод для получения объекта песочницы по сетевому уникальному идентификатору

	ASandboxObject* SpawnPreparedObject(const FSandboxObjectDescriptor& ObjDesc); // Метод для создания подготовленного объекта песочницы

	ASandboxEffect* SpawnEffect(const int32 EffectId, const FTransform& Transform); // Метод для создания эффекта

protected:

	TMap<FString, TSubclassOf<ASandboxObject>> ObjectMapByClassName; // Карта объектов по имени класса

	//TMap<uint64, TSubclassOf<ASandboxObject>> ObjectMapById;

	virtual void SaveLevelJson(); // Метод для сохранения уровня в формате JSON

	virtual void SaveLevelJsonExt(TSharedRef<TJsonWriter<TCHAR>> JsonWriter); // Метод для расширенного сохранения уровня в формате JSON

	virtual void LoadLevelJson(); // Метод для загрузки уровня из формата JSON

	virtual void LoadLevelJsonExt(TSharedPtr<FJsonObject> JsonParsed); // Метод для расширенной загрузки уровня из формата JSON

	void SaveObject(TSharedRef <TJsonWriter<TCHAR>> JsonWriter, const FSandboxObjectDescriptor& ObjDesc); // Метод для сохранения объекта в формате JSON

	virtual void SpawnPreparedObjects(const TArray<FSandboxObjectDescriptor>& ObjDescList); // Метод для создания подготовленных объектов песочницы

	void SavePreparedObjects(const TArray<FSandboxObjectDescriptor>& ObjDescList); // Метод для сохранения подготовленных объектов песочницы

	virtual void PrepareObjectForSave(TArray<FSandboxObjectDescriptor>& ObjDescList); // Метод для подготовки объекта для сохранения

	FString GetNewUid() const; // Метод для получения нового уникального идентификатора

private:

	bool bIsMetaDataReady = false; // Флаг, указывающий, готовы ли метаданные

	static TMap<int32, TSubclassOf<ASandboxObject>> ObjectMapById; // Статическая карта объектов по идентификатору класса

	uint64 ObjectCounter; // Счетчик объектов

	TMap<FString, ASandboxObject*> GlobalObjectMap; // Глобальная карта объектов

	static ASandboxLevelController* StaticSelf; // Статический указатель на экземпляр контроллера уровня песочницы

};
