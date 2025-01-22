#pragma once // Директива препроцессора для предотвращения множественного включения заголовочного файла

#include "Engine.h" // Подключение заголовочного файла движка Unreal Engine
#include "GameFramework/Actor.h" // Подключение заголовочного файла для акторов
#include "ContainerComponent.h" // Подключение заголовочного файла для компонента контейнера
#include "Engine/DamageEvents.h" // Подключение заголовочного файла для событий урона
#include "SandboxObject.generated.h" // Генерация метаданных для UCLASS

#define DAMAGE_ENABLE_PHYS_THRESHOLD 1.f // Определение порогового значения для включения физики при уроне

UCLASS(BlueprintType, Blueprintable) // Макрос для определения класса, который может быть использован в Blueprint
class UNREALSANDBOXTOOLKIT_API ASandboxObject : public AActor { // Определение класса ASandboxObject, наследуемого от AActor
    GENERATED_BODY() // Макрос для генерации тела класса

public:
    ASandboxObject(); // Конструктор класса

    // UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе Unreal
    UPROPERTY(Category = StaticMeshActor, VisibleAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true")) // Свойство для статической меши
    UStaticMeshComponent* SandboxRootMesh; // Компонент статической меши

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе Unreal
    UTexture2D* IconTexture; // Текстура иконки

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе Unreal
    uint64 SandboxClassId; // Идентификатор класса песочницы

    UPROPERTY(Replicated) // Свойство, реплицируемое по сети
    FString SandboxNetUid; // Сетевой уникальный идентификатор песочницы

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе Unreal
    bool bStackable; // Флаг, указывающий, можно ли складывать объекты в стопку

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе Unreal
    uint32 MaxStackSize; // Максимальный размер стопки

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе Unreal
    TMap<FString, FString> PropertyMap; // Карта свойств

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе Unreal
    bool bCanPlaceSandboxObject; // Флаг, указывающий, можно ли разместить объект песочницы

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе Unreal
    float DamageEnablePhysThreshold = DAMAGE_ENABLE_PHYS_THRESHOLD; // Пороговое значение для включения физики при уроне

protected:
    virtual void BeginPlay() override; // Виртуальная функция, вызываемая при начале игры

    UFUNCTION() // Макрос для определения функции, доступной в Blueprint
    void OnSleep(UPrimitiveComponent* SleepingComponent, FName BoneName); // Функция, вызываемая при засыпании компонента

    // UFUNCTION() // Макрос для определения функции, доступной в Blueprint
    // void OnTakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser); // Функция, вызываемая при получении радиального урона

public:
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser); // Виртуальная функция для получения урона

    FString GetSandboxNetUid() const; // Функция для получения сетевого уникального идентификатора песочницы

    uint64 GetSandboxClassId() const; // Функция для получения идентификатора класса песочницы

    virtual FString GetSandboxName(); // Виртуальная функция для получения имени песочницы

    virtual int GetSandboxTypeId() const; // Виртуальная функция для получения идентификатора типа песочницы

    virtual int GetMaxStackSize(); // Виртуальная функция для получения максимального размера стопки

    virtual UTexture2D* GetSandboxIconTexture(); // Виртуальная функция для получения текстуры иконки песочницы

    virtual void TickInInventoryActive(float DeltaTime, UWorld* World, const FHitResult& HitResult); // Виртуальная функция для обновления состояния в инвентаре

    virtual void ActionInInventoryActive(UWorld* World, const FHitResult& HitResult); // Виртуальная функция для выполнения действия в инвентаре

    virtual bool IsInteractive(const APawn* Source = nullptr); // Виртуальная функция для проверки, является ли объект интерактивным

    virtual void MainInteraction(const APawn* Source = nullptr); // Виртуальная функция для основного взаимодействия

    virtual bool CanTake(const AActor* Actor = nullptr) const; // Виртуальная функция для проверки, можно ли взять объект

    virtual void OnTerrainChange(); // Виртуальная функция для обработки изменения территории

    void EnablePhysics(); // Функция для включения физики

    UContainerComponent* GetContainer(const FString& Name); // Функция для получения контейнера по имени

    template<class T> // Шаблонная функция
    T* GetFirstComponentByName(FString ComponentName) { // Функция для получения первого компонента по имени
        TArray<T*> Components; // Массив компонентов
        GetComponents<T>(Components); // Получение всех компонентов типа T
        for (T* Component : Components) { // Перебор всех компонентов
            if (Component->GetName() == ComponentName) // Если имя компонента совпадает с заданным
                return Component; // Возвращаем компонент
        }

        return nullptr; // Возвращаем nullptr, если компонент не найден
    }

    void SetProperty(FString Key, FString Value); // Функция для установки свойства

    FString GetProperty(FString Key) const; // Функция для получения свойства

    void RemoveProperty(FString Key); // Функция для удаления свойства

    virtual void PostLoadProperties(); // Виртуальная функция для загрузки свойств после загрузки

    virtual void OnPlaceToWorld(); // Виртуальная функция для размещения в мире

    virtual bool PlaceToWorldClcPosition(const UWorld* World, const FVector& SourcePos, const FRotator& SourceRotation, const FHitResult& Res, FVector& Location, FRotator& Rotation, bool bFinal = false) const; // Виртуальная функция для расчета позиции размещения в мире

    virtual const UStaticMeshComponent* GetMarkerMesh() const; // Виртуальная функция для получения маркерной меши
};

// TODO // Заметка для будущего
class IWearable { // Интерфейс для носимых объектов

public:

};

UCLASS() // Макрос для определения класса
class UNREALSANDBOXTOOLKIT_API ASandboxSkeletalModule : public ASandboxObject, public IWearable { // Определение класса ASandboxSkeletalModule, наследуемого от ASandboxObject и IWearable
    GENERATED_BODY() // Макрос для генерации тела класса

public:
    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе Unreal
    FName SkMeshBindName; // Имя привязки скелетной меши

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе Unreal
    USkeletalMesh* SkeletalMesh; // Скелетная меш

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе Unreal
    bool bModifyFootPose; // Флаг, указывающий, нужно ли изменять позу ног

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе Unreal
    FRotator FootRotator; // Ротатор для ног

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе Unreal
    TMap<FString, float> MorphMap; // Карта морфинга

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе Unreal
    TMap<FString, float> ParentMorphMap; // Карта морфинга родителя

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе Unreal
    TMap<FString, float> AffectParamMap; // Карта параметров воздействия

    virtual float GetAffectParam(const FString& ParamName) const; // Виртуальная функция для получения параметра воздействия

    int GetSandboxTypeId() const override; // Переопределение функции для получения идентификатора типа песочницы

    void GetFootPose(FRotator& LeftFootRotator, FRotator& RightFootRotator); // Функция для получения позы ног

};

class UNREALSANDBOXTOOLKIT_API ASandboxObjectUtils { // Утилиты для объектов песочницы

public:
    static TArray<ASandboxObject*> GetContainerContent(AActor* AnyActor, const FString& Name); // Статическая функция для получения содержимого контейнера
};

UCLASS() // Макрос для определения класса
class UNREALSANDBOXTOOLKIT_API USandboxDamageType : public UDamageType { // Определение класса USandboxDamageType, наследуемого от UDamageType
    GENERATED_UCLASS_BODY() // Макрос для генерации тела класса

public:
    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе Unreal
    float FireDamageFactor = 1.f; // Фактор урона от огня

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе Unreal
    float ExplosionDamageFactor = 1.f; // Фактор урона от взрыва

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе Unreal
    float HitDamageFactor = 1.f; // Фактор урона от удара

};
