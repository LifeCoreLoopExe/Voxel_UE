#pragma once

// Подключаем основные заголовочные файлы Unreal Engine
#include "Engine.h"
#include "GameFramework/Actor.h"
#include "SandboxObjectMap.h"
#include "SandboxLevelController.generated.h"

// Структура для хранения временного стека контейнера
// Используется для сохранения предметов в инвентаре
USTRUCT()
struct FTempContainerStack {
	GENERATED_BODY()

	// ID слота в контейнере
	UPROPERTY()
	int SlotId;

	// Стек (куча) предметов в контейнере
	UPROPERTY()
	FContainerStack Stack;
};

// Структура, описывающая объект в мире игры
// Содержит всю необходимую информацию для создания и сохранения объекта
USTRUCT()
struct UNREALSANDBOXTOOLKIT_API FSandboxObjectDescriptor {
	GENERATED_BODY()

	// Положение, поворот и масштаб объекта в мире
	UPROPERTY()
	FTransform Transform;

	// Уникальный ID класса объекта
	UPROPERTY()
	uint64 ClassId;

	// Уникальный сетевой идентификатор объекта
	UPROPERTY()
	FString NetUid;

	// Тип объекта
	UPROPERTY()
	int TypeId;

	// Карта дополнительных свойств объекта
	UPROPERTY()
	TMap<FString, FString> PropertyMap;

	// Массив стеков контейнера (для инвентаря)
	UPROPERTY()
	TArray<FTempContainerStack> Container;

	// Статический метод для создания дескриптора из существующего объекта
	static FSandboxObjectDescriptor MakeObjDescriptor(ASandboxObject* SandboxObject);
};

// Основной класс контроллера уровня
// Управляет всеми объектами на уровне, их созданием, удалением и сохранением
UCLASS()
class UNREALSANDBOXTOOLKIT_API ASandboxLevelController : public AActor {
	GENERATED_BODY()

public:
	// Конструктор
	ASandboxLevelController();

	// Вызывается при старте игры
	virtual void BeginPlay() override;

	// Вызывается при завершении игры
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Вызывается каждый кадр
	virtual void Tick(float DeltaSeconds) override;

	// Название карты/уровня
	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit")
	FString MapName;

	// Карта объектов уровня
	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit")
	USandboxObjectMap* ObjectMap;

	// Получить класс объекта по его ID (устаревший метод)
	TSubclassOf<ASandboxObject> GetSandboxObjectByClassId(int32 ClassId);

	// Получить объект по его ID класса
	ASandboxObject* GetSandboxObject(uint64 ClassId);

	// Получить объект-образец по ID класса
	static ASandboxObject* GetDefaultSandboxObject(uint64 ClassId);

	// Получить экземпляр контроллера уровня
	static ASandboxLevelController* GetInstance();

	// Подготовить метаданные
	void PrepareMetaData();

	// Создать объект в мире
	virtual ASandboxObject* SpawnSandboxObject(const int ClassId, const FTransform& Transform, const FString& SandboxNetUid = "");

	// Удалить объект из мира
	virtual bool RemoveSandboxObject(ASandboxObject* Obj);

	// Найти объект по сетевому ID
	ASandboxObject* GetObjectByNetUid(FString NetUid);

	// Создать подготовленный объект
	ASandboxObject* SpawnPreparedObject(const FSandboxObjectDescriptor& ObjDesc);

	// Создать эффект в мире
	ASandboxEffect* SpawnEffect(const int32 EffectId, const FTransform& Transform);

protected:
	// Карта классов объектов по имени класса
	TMap<FString, TSubclassOf<ASandboxObject>> ObjectMapByClassName;

	// Сохранить уровень в JSON
	virtual void SaveLevelJson();

	// Дополнительное сохранение в JSON
	virtual void SaveLevelJsonExt(TSharedRef<TJsonWriter<TCHAR>> JsonWriter);

	// Загрузить уровень из JSON
	virtual void LoadLevelJson();

	// Дополнительная загрузка из JSON
	virtual void LoadLevelJsonExt(TSharedPtr<FJsonObject> JsonParsed);

	// Сохранить отдельный объект
	void SaveObject(TSharedRef <TJsonWriter<TCHAR>> JsonWriter, const FSandboxObjectDescriptor& ObjDesc);

	// Создать подготовленные объекты
	virtual void SpawnPreparedObjects(const TArray<FSandboxObjectDescriptor>& ObjDescList);

	// Сохранить подготовленные объекты
	void SavePreparedObjects(const TArray<FSandboxObjectDescriptor>& ObjDescList);

	// Подготовить объекты к сохранению
	virtual void PrepareObjectForSave(TArray<FSandboxObjectDescriptor>& ObjDescList);

	// Создать новый уникальный ID
	FString GetNewUid() const;

private:
	// Флаг готовности метаданных
	bool bIsMetaDataReady = false;

	// Статическая карта объектов по ID
	static TMap<int32, TSubclassOf<ASandboxObject>> ObjectMapById;

	// Счетчик объектов
	uint64 ObjectCounter;

	// Глобальная карта всех объектов
	TMap<FString, ASandboxObject*> GlobalObjectMap;

	// Статический указатель на себя
	static ASandboxLevelController* StaticSelf;
};