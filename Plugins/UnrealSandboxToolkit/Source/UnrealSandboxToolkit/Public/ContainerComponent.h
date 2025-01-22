// Copyright blackw 2015-2020
#pragma once // Включает файл только один раз

#include "Engine.h" // Основные функции движка
#include "Components/ActorComponent.h" // Базовый класс для компонентов актера
#include "ContainerComponent.generated.h" // Метаданные и рефлексия для ContainerComponent

class ASandboxObject; // Предварительное объявление класса ASandboxObject


USTRUCT() // Объявление структуры, доступной для рефлексии UObject
struct FContainerStack {
    GENERATED_USTRUCT_BODY() // Макрос для генерации тела структуры

    UPROPERTY(EditAnywhere) // Свойство, доступное для редактирования в редакторе
    int32 Amount; // Количество

    UPROPERTY(EditAnywhere) // Свойство, доступное для редактирования в редакторе
    uint64 SandboxClassId; // Идентификатор класса Sandbox

    FContainerStack() { // Конструктор по умолчанию
        Clear(); // Очистка значений
    }

    FContainerStack(int32 Amount_, uint64 SandboxClassId_) : Amount(Amount_), SandboxClassId(SandboxClassId_) { } // Конструктор с параметрами

    void Clear() { // Метод для очистки значений
        Amount = 0;
        SandboxClassId = 0;
    }

    bool IsEmpty() const { // Метод для проверки, пуст ли контейнер
        return Amount == 0;
    }

    const ASandboxObject* GetObject() const; // Метод для получения объекта Sandbox (объявление)
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent)) // Объявление класса, доступного для рефлексии UObject и Blueprint
class UNREALSANDBOXTOOLKIT_API UContainerComponent : public UActorComponent { // Объявление класса UContainerComponent, наследующегося от UActorComponent
    GENERATED_BODY() // Макрос для генерации тела класса

public:
    UPROPERTY(ReplicatedUsing = OnRep_Content, EditAnywhere, Category = "Sandbox") // Свойство, реплицируемое и доступное для редактирования в редакторе
    int32 MaxCapacity = 13; // Максимальная емкость контейнера

    UPROPERTY(ReplicatedUsing = OnRep_Content, EditAnywhere, Category = "Sandbox") // Свойство, реплицируемое и доступное для редактирования в редакторе
    TArray<FContainerStack> Content; // Содержимое контейнера

public:
    UContainerComponent(); // Конструктор

    virtual void BeginPlay() override; // Метод, вызываемый при начале игры

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override; // Метод, вызываемый каждый тик

    UFUNCTION() // Объявление функции, доступной для рефлексии UObject
    void OnRep_Content(); // Метод для обработки репликации содержимого

    const TArray<FContainerStack>& GetContent(); // Метод для получения содержимого контейнера

    bool SetStackDirectly(const FContainerStack& Stack, const int SlotId); // Метод для установки стека в слот

    bool AddObject(ASandboxObject* Obj); // Метод для добавления объекта в контейнер

    FContainerStack* GetSlot(const int Slot); // Метод для получения слота

    const FContainerStack* GetSlot(const int Slot) const; // Метод для получения слота (константный)

    void ChangeAmount(int Slot, int Num); // Метод для изменения количества в слоте

    bool DecreaseObjectsInContainer(int slot, int num); // Метод для уменьшения количества объектов в контейнере

    bool IsEmpty() const; // Метод для проверки, пуст ли контейнер

    bool IsSlotEmpty(int SlotId) const; // Метод для проверки, пуст ли слот

    void CopyTo(UContainerComponent* Target); // Метод для копирования содержимого в другой контейнер

    TArray<uint64> GetAllObjects() const; // Метод для получения всех объектов в контейнере

    bool SlotTransfer(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne = false); // Метод для переноса слота

    bool IsUpdated(); // Метод для проверки, обновлен ли контейнер

    void ResetUpdatedFlag(); // Метод для сброса флага обновления

    const TMap<uint64, uint32>& GetStats() const; // Метод для получения статистики инвентаря

private:
    TMap<uint64, uint32> InventoryStats; // Статистика инвентаря

    void MakeStats(); // Метод для создания статистики

    bool bUpdated = false; // Флаг обновления
};
