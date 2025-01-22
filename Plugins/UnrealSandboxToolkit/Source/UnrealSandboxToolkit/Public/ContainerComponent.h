// Copyright blackw 2015-2020 // Авторские права blackw 2015-2020

#pragma once // Указание компилятору включить этот файл только один раз

#include "Engine.h" // Подключение основного заголовочного файла движка Unreal Engine
#include "Components/ActorComponent.h" // Подключение заголовочного файла для компонентов акторов
#include "ContainerComponent.generated.h" // Генерация заголовочного файла для контейнерного компонента

class ASandboxObject; // Объявление класса ASandboxObject

USTRUCT() // Определение структуры Unreal Engine
struct FContainerStack {
	GENERATED_USTRUCT_BODY() // Генерация кода для структуры

	UPROPERTY(EditAnywhere) // Указание, что это свойство можно редактировать в редакторе
	int32 Amount; // Количество объектов в стеке

	UPROPERTY(EditAnywhere) // Указание, что это свойство можно редактировать в редакторе
	uint64 SandboxClassId; // Идентификатор класса песочницы

	FContainerStack() { // Конструктор по умолчанию
		Clear(); // Вызов метода очистки
	}

	FContainerStack(int32 Amount_, uint64 SandboxClassId_) : Amount(Amount_), SandboxClassId(SandboxClassId_) { } // Конструктор с параметрами

	void Clear() { // Метод очистки стека
		Amount = 0; // Сброс количества объектов до нуля
		SandboxClassId = 0; // Сброс идентификатора класса песочницы до нуля
	}

	bool IsEmpty() const { // Проверка, пустой ли стек
		return Amount == 0; // Возвращает true, если количество объектов равно нулю
	}

	const ASandboxObject* GetObject() const; // Объявление метода для получения объекта из стека
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) ) // Определение класса компонента с метаданными для Blueprints
class UNREALSANDBOXTOOLKIT_API UContainerComponent : public UActorComponent { // Определение класса UContainerComponent, наследующего от UActorComponent
	GENERATED_BODY() // Генерация тела класса

public:

	UPROPERTY(ReplicatedUsing = OnRep_Content, EditAnywhere, Category = "Sandbox") // Свойство, которое будет реплицироваться и редактироваться в редакторе
	int32 MaxCapacity = 13; // Максимальная емкость контейнера

	UPROPERTY(ReplicatedUsing = OnRep_Content, EditAnywhere, Category = "Sandbox") // Свойство, которое будет реплицироваться и редактироваться в редакторе
	TArray<FContainerStack> Content; // Массив стеков контейнера

public:	
	UContainerComponent(); // Конструктор класса UContainerComponent

	virtual void BeginPlay() override; // Переопределение метода BeginPlay для инициализации при старте игры
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override; // Переопределение метода TickComponent для обновления компонента каждый кадр

	UFUNCTION() // Объявление функции, которая может быть вызвана из Blueprints
	void OnRep_Content(); // Метод для обработки изменений содержимого контейнера

	const TArray<FContainerStack>& GetContent(); // Метод для получения содержимого контейнера
    
	bool SetStackDirectly(const FContainerStack& Stack, const int SlotId); // Метод для установки стека напрямую в указанный слот

	bool AddObject(ASandboxObject* Obj); // Метод для добавления объекта в контейнер
    
	FContainerStack* GetSlot(const int Slot); // Метод для получения указателя на стек в указанном слоте

	const FContainerStack* GetSlot(const int Slot) const; // Константный метод для получения указателя на стек в указанном слоте
    
	void ChangeAmount(int Slot, int Num); // Метод для изменения количества объектов в указанном слоте

    bool DecreaseObjectsInContainer(int slot, int num); // Метод для уменьшения количества объектов в контейнере в указанном слоте

	bool IsEmpty() const; // Метод для проверки, пуст ли контейнер

	bool IsSlotEmpty(int SlotId) const; // Метод для проверки, пуст ли указанный слот

	void CopyTo(UContainerComponent* Target); // Метод для копирования содержимого в другой контейнер

	TArray<uint64> GetAllObjects() const; // Метод для получения всех объектов из контейнера

	bool SlotTransfer(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer, bool bOnlyOne = false); // Метод для передачи объектов между слотами

	bool IsUpdated(); // Метод для проверки, был ли компонент обновлен

	void ResetUpdatedFlag(); // Метод для сброса флага обновления

	const TMap<uint64, uint32>& GetStats() const; // Метод для получения статистики инвентаря

private:

	TMap<uint64, uint32> InventoryStats; // Карта статистики инвентаря (идентификатор объекта -> количество)

	void MakeStats(); // Метод для создания статистики инвентаря

	bool bUpdated = false; // Флаг обновления компонента (true - обновлен)

}

