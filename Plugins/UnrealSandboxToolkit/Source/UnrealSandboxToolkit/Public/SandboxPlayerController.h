#pragma once // Указание компилятору включить этот файл только один раз

#include "Engine.h" // Подключение основного заголовочного файла движка Unreal Engine
#include "GameFramework/PlayerController.h" // Подключение заголовочного файла для класса APlayerController
#include "SandboxLevelController.h" // Подключение заголовочного файла для контроллера уровня песочницы
#include "SandboxPlayerController.generated.h" // Генерация заголовочного файла для класса ASandboxPlayerController

class ASandboxObject; // Объявление класса ASandboxObject
class UUserWidget; // Объявление класса UUserWidget
class UContainerComponent; // Объявление класса UContainerComponent

USTRUCT() // Определение структуры FPlayerInfo
struct FPlayerInfo {
	GENERATED_BODY() // Генерация тела структуры

	UPROPERTY() // Свойство PlayerUid, уникальный идентификатор игрока
	FString PlayerUid; // Уникальный идентификатор игрока
};

UCLASS() // Определение класса ASandboxPlayerController, доступного в Blueprints
class UNREALSANDBOXTOOLKIT_API ASandboxPlayerController : public APlayerController // Определение класса ASandboxPlayerController, наследующего от APlayerController
{
	GENERATED_BODY() // Генерация тела класса

public:
	virtual void BeginPlay() override; // Переопределение метода BeginPlay для инициализации при старте игры

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство CurrentInventorySlot, редактируемое в редакторе в категории "Sandbox"
	int32 CurrentInventorySlot; // Текущий слот инвентаря

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство bIsAdmin, редактируемое в редакторе в категории "Sandbox"
	bool bIsAdmin; // Флаг, указывающий, является ли игрок администратором

	ASandboxPlayerController(); // Конструктор класса ASandboxPlayerController

	virtual void OnPossess(APawn* NewPawn) override; // Переопределение метода OnPossess для обработки захвата нового персонажа

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Метод BlockGameInput доступный из Blueprints в категории "Sandbox"
	void BlockGameInput(); // Метод для блокировки ввода игры

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Метод UnblockGameInput доступный из Blueprints в категории "Sandbox"
	void UnblockGameInput(); // Метод для разблокировки ввода игры

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Метод SetCurrentInventorySlot доступный из Blueprints в категории "Sandbox"
	virtual void SetCurrentInventorySlot(int32 Slot); // Виртуальный метод для установки текущего слота инвентаря

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Метод TakeObjectToInventory доступный из Blueprints в категории "Sandbox"
	bool TakeObjectToInventory(); // Метод для добавления объекта в инвентарь

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Метод OpenObjectContainer доступный из Blueprints в категории "Sandbox"
	bool OpenObjectContainer(ASandboxObject* Obj); // Метод для открытия контейнера объекта песочницы

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Метод TraceAndOpenObjectContainer доступный из Blueprints в категории "Sandbox"
	bool TraceAndOpenObjectContainer(); // Метод для трассировки и открытия контейнера объекта

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Метод HasOpenContainer доступный из Blueprints в категории "Sandbox"
	bool HasOpenContainer(); // Метод проверки наличия открытого контейнера

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Метод GetOpenedContainer доступный из Blueprints в категории "Sandbox"
	UContainerComponent* GetOpenedContainer(); // Метод получения открытого контейнера

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Метод GetOpenedObject доступный из Blueprints в категории "Sandbox"
	ASandboxObject* GetOpenedObject(); // Метод получения открытого объекта песочницы

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Метод CloseObjectWithContainer доступный из Blueprints в категории "Sandbox"
	void CloseObjectWithContainer(); // Метод закрытия объекта с контейнером

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Метод TracePlayerActionPoint доступный из Blueprints в категории "Sandbox"
	FHitResult TracePlayerActionPoint(); // Метод трассировки точки действия игрока

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Метод SelectActionObject доступный из Blueprints в категории "Sandbox"
	void SelectActionObject(AActor* Actor); // Метод выбора объекта действия

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Метод IsGameInputBlocked доступный из Blueprints в категории "Sandbox"
	bool IsGameInputBlocked(); // Метод проверки заблокирован ли ввод игры

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Метод TraceAndSelectActionObject доступный из Blueprints в категории "Sandbox"
	void TraceAndSelectActionObject(); // Метод трассировки и выбора объекта действия

	UFUNCTION(Server, Reliable) // Методы с репликацией на сервере
	void TransferContainerStack(const FString& ObjectNetUid, const FString& ContainerName, const FContainerStack& Stack, const int SlotId); 

	UFUNCTION(Server, Reliable) 
	void TransferInventoryStack(const FString& ContainerName, const FContainerStack& Stack, const int SlotId); 

protected:
	/** True if the controlled character should navigate to the mouse cursor. */ 
	uint32 bMoveToMouseCursor : 1;  // Флаг указывающий на необходимость перемещения персонажа к курсору мыши

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;  // Переопределение метода PlayerTick для обновления состояния игрока
	virtual void SetupInputComponent() override;  // Переопределение метода SetupInputComponent для настройки ввода
	// End PlayerController interface

	/** Navigate player to the current mouse cursor location. */ 
	void MoveToMouseCursor();  // Метод перемещения персонажа к текущему положению курсора мыши

	/** Navigate player to the current touch location. */ 
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);  // Метод перемещения персонажа к текущему положению касания
	
	/** Navigate player to the given world location. */ 
	void SetNewMoveDestination(const FVector DestLocation);  // Метод установки новой точки назначения перемещения персонажа

	/** Input handlers for SetDestination action. */ 
	void SetDestinationPressed();  // Обработчик ввода для нажатия на установку точки назначения

	void SetDestinationReleased();  // Обработчик ввода для отпускания установки точки назначения

	// ==========================================================================================================

	virtual void OnMainActionPressed();  // Виртуальный метод обработки нажатия основной действия 

	virtual void OnMainActionReleased();  // Виртуальный метод обработки отпускания основной действия 

	virtual void OnAltActionPressed();  // Виртуальный метод обработки нажатия альтернативного действия 

	virtual void OnAltActionReleased();  // Виртуальный метод обработки отпускания альтернативного действия 

	virtual void ToggleView();  // Виртуальный метод переключения вида 

	UContainerComponent* GetContainerByName(FName ContainerName);  // Метод получения контейнера по имени 

	UContainerComponent* GetInventory();  // Метод получения инвентаря 

	virtual void OnTracePlayerActionPoint(const FHitResult& Res);  // Виртуальный метод обработки трассировки точки действия игрока 

	virtual void OnSelectActionObject(AActor* Actor);  // Виртуальный метод обработки выбора объекта действия 

	virtual void OnDeselectActionObject(AActor* Actor);  // Виртуальный метод обработки снятия выбора с объекта действия 

	UPROPERTY()  // Свойство LevelController без модификаторов доступа
	ASandboxLevelController* LevelController;  // Указатель на контроллер уровня песочницы

public:

	void ShowMouseCursor(bool bShowCursor);  // Метод отображения или скрытия курсора мыши 

	virtual void OnContainerMainAction(int32 SlotId, FName ContainerName);  // Виртуальный метод обработки основного действия с контейнером 

	virtual void OnContainerDropSuccess(int32 SlotId, FName SourceName, FName TargetName);  // Виртуальный метод обработки успешного сброса контейнера 

	virtual bool OnContainerDropCheck(int32 SlotId, FName ContainerName, const ASandboxObject* Obj) const;  // Виртуальный метод проверки сброса контейнера 

	virtual bool OnContainerSlotHover(int32 SlotId, FName ContainerName);  // Виртуальный метод обработки наведения курсора на слот контейнера 

	ASandboxLevelController* GetLevelController();  // Метод получения контроллера уровня песочницы 

private:

	void OnMainActionPressedInternal();  // Приватный метод обработки внутреннего нажатия основной действия 
	
	void OnMainActionReleasedInternal();  // Приватный метод обработки внутреннего отпускания основной действия 

	void OnAltActionPressedInternal();  // Приватный метод обработки внутреннего нажатия альтернативного действия 

	void OnAltActionReleasedInternal();  // Приватный метод обработки внутреннего отпускания альтернативного действия 


	FHitResult CurrentPos;  // Переменная для хранения текущей позиции удара 

	bool bIsGameInputBlocked;  // Флаг указывающий заблокирован ли ввод игры 

	UPROPERTY()  // Свойство SelectedObject без модификаторов доступа
	ASandboxObject* SelectedObject;  // Указатель на выбранный объект песочницы 

	UPROPERTY()  // Свойство OpenedObject без модификаторов доступа
	ASandboxObject* OpenedObject;  // Указатель на открытый объект песочницы 

	UPROPERTY()  // Свойство OpenedContainer без модификаторов доступа
	UContainerComponent* OpenedContainer;  // Указатель на открытый контейнер 
};
