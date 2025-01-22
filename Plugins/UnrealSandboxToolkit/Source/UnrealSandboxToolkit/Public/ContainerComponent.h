// Copyright blackw 2015-2020

#pragma once

#include "Engine.h"
#include "Components/ActorComponent.h"
#include "ContainerComponent.generated.h"

// Предварительное объявление класса, который будет использоваться позже
class ASandboxObject;

// Структура для хранения стопки предметов в контейнере
USTRUCT()
struct FContainerStack {
	GENERATED_USTRUCT_BODY()

	// Количество предметов в стопке
	UPROPERTY(EditAnywhere)
	int32 Amount;

	// Уникальный идентификатор типа предмета
	UPROPERTY(EditAnywhere)
	uint64 SandboxClassId;

	// Конструктор по умолчанию - создает пустую стопку
	FContainerStack() {
		Clear();
	}

	// Конструктор с параметрами - создает стопку с заданным количеством и типом предметов
	FContainerStack(int32 Amount_, uint64 SandboxClassId_) : Amount(Amount_), SandboxClassId(SandboxClassId_) { }

	// Очищает стопку (делает её пустой)
	void Clear() {
		Amount = 0;
		SandboxClassId = 0;
	}

	// Проверяет, пуста ли стопка
	bool IsEmpty() const {
		return Amount == 0;
	}

	// Получает указатель на объект этого типа
	const ASandboxObject* GetObject() const;
};

// Компонент-контейнер для хранения предметов (как инвентарь)
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALSANDBOXTOOLKIT_API UContainerComponent : public UActorComponent {
	GENERATED_BODY()

public:
	// Максимальное количество слотов в контейнере
	UPROPERTY(ReplicatedUsing = OnRep_Content, EditAnywhere, Category = "Sandbox")
	int32 MaxCapacity = 13;

	// Массив стопок предметов (содержимое контейнера)
	UPROPERTY(ReplicatedUsing = OnRep_Content, EditAnywhere, Category = "Sandbox")
	TArray<FContainerStack> Content;

public:	
	// Конструктор компонента
	UContainerComponent();

	// Вызывается при начале игры
	virtual void BeginPlay() override;
	
	// Вызывается каждый кадр
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	// Функция, вызываемая при обновлении содержимого по сети
	UFUNCTION()
	void OnRep_Content();

	// Получает содержимое контейнера
	const TArray<FContainerStack>& GetContent();
    
	// Напрямую устанавливает стопку в указанный слот
	bool SetStackDirectly(const FContainerStack& Stack, const int SlotId);

	// Добавляет объект в контейнер
	bool AddObject(ASandboxObject* Obj);
    
	// Получает указатель на стопку в указанном слоте
	FContainerStack* GetSlot(const int Slot);

	// Получает константный указатель на стопку в указанном слоте
	const FContainerStack* GetSlot(const int Slot) const;
    
	// Изменяет количество предметов в слоте
	void ChangeAmount(int Slot, int Num);

	// Уменьшает количество предметов в слоте
	bool DecreaseObjectsInContainer(int slot, int num);

	// Проверяет, пуст ли контейнер
	bool IsEmpty() const;

	// Проверяет, пуст ли конкретный слот
	bool IsSlotEmpty(int SlotId) const;

	// Копирует содержимое в другой контейнер
	void CopyTo(UContainerComponent* Target);

	// Получает список всех типов объектов в контейнере
	TArray<uint64> GetAllObjects() const;

	// Перемещает предметы между слотами
	bool SlotTransfer(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne = false);

	// Проверяет, было ли обновление
	bool IsUpdated();

	// Сбрасывает флаг обновления
	void ResetUpdatedFlag();

	// Получает статистику по предметам
	const TMap<uint64, uint32>& GetStats() const;

private:
	// Хранит статистику по количеству разных типов предметов
	TMap<uint64, uint32> InventoryStats;

	// Обновляет статистику
	void MakeStats();

	// Флаг, показывающий было ли обновление
	bool bUpdated = false;

};

