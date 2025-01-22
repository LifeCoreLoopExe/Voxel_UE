#pragma once // Включает файл только один раз

#include "Engine.h" // Включает основные функции движка
#include "GameFramework/Actor.h" // Включает класс Actor
#include "SandboxObjectMap.h" // Включает заголовочный файл для SandboxObjectMap
#include "SandboxLevelController.generated.h" // Включает сгенерированный заголовочный файл для SandboxLevelController

USTRUCT() // Объявление структуры, доступной для рефлексии UObject
struct FTempContainerStack {
    GENERATED_BODY() // Макрос для генерации тела структуры

    UPROPERTY() // Свойство
    int SlotId; // Идентификатор слота

    UPROPERTY() // Свойство
    FContainerStack Stack; // Стек контейнера
};

USTRUCT() // Объявление структуры, доступной для рефлексии UObject
struct UNREALSANDBOXTOOLKIT_API FSandboxObjectDescriptor {
    GENERATED_BODY() // Макрос для генерации тела структуры

    UPROPERTY() // Свойство
    FTransform Transform; // Преобразование

    UPROPERTY() // Свойство
    uint64 ClassId; // Идентификатор класса

    UPROPERTY() // Свойство
    FString NetUid; // Сетевой уникальный идентификатор

    UPROPERTY() // Свойство
    int TypeId; // Идентификатор типа

    UPROPERTY() // Свойство
    TMap<FString, FString> PropertyMap; // Карта свойств

    UPROPERTY() // Свойство
    TArray<FTempContainerStack> Container; // Контейнер

    static FSandboxObjectDescriptor MakeObjDescriptor(ASandboxObject* SandboxObject); // Метод для создания дескриптора объекта Sandbox
};

UCLASS() // Объявление класса, доступного для рефлексии UObject
class UNREALSANDBOXTOOLKIT_API ASandboxLevelController : public AActor { // Объявление класса ASandboxLevelController, наследующегося от AActor
    GENERATED_BODY() // Макрос для генерации тела класса

public:
    ASandboxLevelController(); // Конструктор

    virtual void BeginPlay() override; // Метод, вызываемый при начале игры

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; // Метод, вызываемый при завершении игры

    virtual void Tick(float DeltaSeconds) override; // Метод, вызываемый каждый тик

    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit") // Свойство, доступное для редактирования в редакторе
    FString MapName; // Имя карты

    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit") // Свойство, доступное для редактирования в редакторе
    USandboxObjectMap* ObjectMap; // Карта объектов Sandbox

    // TODO: remove
    TSubclassOf<ASandboxObject> GetSandboxObjectByClassId(int32 ClassId); // Метод для получения объекта Sandbox по идентификатору класса

    ASandboxObject* GetSandboxObject(uint64 ClassId); // Метод для получения объекта Sandbox по идентификатору класса

    static ASandboxObject* GetDefaultSandboxObject(uint64 ClassId); // Метод для получения объекта Sandbox по умолчанию

    static ASandboxLevelController* GetInstance(); // Метод для получения экземпляра контроллера уровня

    void PrepareMetaData(); // Метод для подготовки метаданных

    virtual ASandboxObject* SpawnSandboxObject(const int ClassId, const FTransform& Transform, const FString& SandboxNetUid = ""); // Метод для создания объекта Sandbox

    virtual bool RemoveSandboxObject(ASandboxObject* Obj); // Метод для удаления объекта Sandbox

    ASandboxObject* GetObjectByNetUid(FString NetUid); // Метод для получения объекта Sandbox по сетевому идентификатору

    ASandboxObject* SpawnPreparedObject(const FSandboxObjectDescriptor& ObjDesc); // Метод для создания подготовленного объекта Sandbox

    ASandboxEffect* SpawnEffect(const int32 EffectId, const FTransform& Transform); // Метод для создания эффекта Sandbox

protected:
    TMap<FString, TSubclassOf<ASandboxObject>> ObjectMapByClassName; // Карта объектов Sandbox по имени класса

    // TMap<uint64, TSubclassOf<ASandboxObject>> ObjectMapById;

    virtual void SaveLevelJson(); // Метод для сохранения уровня в JSON

    virtual void SaveLevelJsonExt(TSharedRef<TJsonWriter<TCHAR>> JsonWriter); // Метод для расширенного сохранения уровня в JSON

    virtual void LoadLevelJson(); // Метод для загрузки уровня из JSON

    virtual void LoadLevelJsonExt(TSharedPtr<FJsonObject> JsonParsed); // Метод для расширенной загрузки уровня из JSON

    void SaveObject(TSharedRef<TJsonWriter<TCHAR>> JsonWriter, const FSandboxObjectDescriptor& ObjDesc); // Метод для сохранения объекта в JSON

    virtual void SpawnPreparedObjects(const TArray<FSandboxObjectDescriptor>& ObjDescList); // Метод для создания подготовленных объектов

    void SavePreparedObjects(const TArray<FSandboxObjectDescriptor>& ObjDescList); // Метод для сохранения подготовленных объектов

    virtual void PrepareObjectForSave(TArray<FSandboxObjectDescriptor>& ObjDescList); // Метод для подготовки объекта к сохранению

    FString GetNewUid() const; // Метод для получения нового уникального идентификатора

private:
    bool bIsMetaDataReady = false; // Флаг готовности метаданных

    static TMap<int32, TSubclassOf<ASandboxObject>> ObjectMapById; // Статическая карта объектов Sandbox по идентификатору класса

    uint64 ObjectCounter; // Счетчик объектов

    TMap<FString, ASandboxObject*> GlobalObjectMap; // Глобальная карта объектов Sandbox

    static ASandboxLevelController* StaticSelf; // Статический экземпляр контроллера уровня
};
