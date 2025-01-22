// Copyright blackw 2015-2020

#pragma once // Защита от множественного включения этого заголовочного файла

#include "Engine.h" // Подключение основного заголовочного файла движка Unreal Engine
#include "Components/ActorComponent.h" // Подключение заголовочного файла для компонентов акторов
#include "ContainerComponent.generated.h" // Генерация кода для этого заголовочного файла

class ASandboxObject; // Предварительное объявление класса ASandboxObject

// Структура для хранения информации о контейнере
USTRUCT()
struct FContainerStack {
	GENERATED_USTRUCT_BODY() // Генерация тела структуры

	UPROPERTY(EditAnywhere) // Свойство, доступное для редактирования в редакторе
	int32 Amount; // Количество объектов в стеке

	UPROPERTY(EditAnywhere) // Свойство, доступное для редактирования в редакторе
	uint64 SandboxClassId; // Идентификатор класса объекта в контейнере

	FContainerStack() { // Конструктор по умолчанию
		Clear(); // Очистка значений
	}

	FContainerStack(int32 Amount_, uint64 SandboxClassId_) : Amount(Amount_), SandboxClassId(SandboxClassId_) { } // Конструктор с параметрами

	void Clear() { // Метод для очистки значений стека
		Amount = 0; // Установка количества в 0
		SandboxClassId = 0; // Установка идентификатора класса в 0
	}

	bool IsEmpty() const { // Метод для проверки, пуст ли стек
		return Amount == 0; // Возвращает true, если количество равно 0
	}

	const ASandboxObject* GetObject() const; // Метод для получения объекта из стека (реализация вне этого файла)
};

// Класс компонента контейнера, наследующий от UActorComponent
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent)) 
class UNREALSANDBOXTOOLKIT_API UContainerComponent : public UActorComponent {
	GENERATED_BODY() // Генерация тела класса

public:
	UPROPERTY(ReplicatedUsing = OnRep_Content, EditAnywhere, Category = "Sandbox") 
	int32 MaxCapacity = 13; // Максимальная ёмкость контейнера (по умолчанию 13)

	UPROPERTY(ReplicatedUsing = OnRep_Content, EditAnywhere, Category = "Sandbox") 
	TArray<FContainerStack> Content; // Массив стеков, содержащих объекты в контейнере

public:	
	UContainerComponent(); // Конструктор компонента

	virtual void BeginPlay() override; // Переопределение метода BeginPlay для инициализации при старте игры
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override; 
    // Переопределение метода TickComponent для обновления состояния компонента каждый кадр

	UFUNCTION() 
	void OnRep_Content(); // Метод для обработки репликации содержимого контейнера

	const TArray<FContainerStack>& GetContent(); // Метод для получения содержимого контейнера
    
	bool SetStackDirectly(const FContainerStack& Stack, const int SlotId); 
    // Метод для установки стека объектов напрямую в указанный слот

	bool AddObject(ASandboxObject* Obj); // Метод для добавления объекта в контейнер
    
	FContainerStack* GetSlot(const int Slot); // Метод для получения указателя на стек в указанном слоте

	const FContainerStack* GetSlot(const int Slot) const; // Константная версия метода получения слота
    
	void ChangeAmount(int Slot, int Num); // Метод для изменения количества объектов в указанном слоте

    bool DecreaseObjectsInContainer(int slot, int num); 
    // Метод для уменьшения количества объектов в контейнере в указанном слоте

	bool IsEmpty() const; // Метод для проверки, пуст ли контейнер

	bool IsSlotEmpty(int SlotId) const; // Метод для проверки, пуст ли указанный слот

	void CopyTo(UContainerComponent* Target); 
    // Метод для копирования содержимого текущего контейнера в целевой контейнер

	TArray<uint64> GetAllObjects() const; // Метод для получения всех объектов из контейнера

	bool SlotTransfer(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne = false);
    // Метод для передачи объектов между слотами (или контейнерами)

	bool IsUpdated(); // Метод проверки обновления состояния контейнера

	void ResetUpdatedFlag(); // Метод сброса флага обновления состояния

	const TMap<uint64, uint32>& GetStats() const; 
    // Метод для получения статистики инвентаря (количество объектов и их идентификаторы)

private:
	TMap<uint64, uint32> InventoryStats; // Карта статистики инвентаря (идентификатор объекта -> количество)

	void MakeStats(); // Метод для создания статистики инвентаря

	bool bUpdated = false; // Флаг обновления состояния контейнера
};
