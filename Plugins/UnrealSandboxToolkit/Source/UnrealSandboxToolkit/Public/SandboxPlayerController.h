#pragma once // Защита от множественного включения этого заголовочного файла

#include "Engine.h" // Подключение основного заголовочного файла движка Unreal Engine
#include "GameFramework/PlayerController.h" // Подключение заголовочного файла для класса PlayerController
#include "SandboxLevelController.h" // Подключение заголовочного файла для контроллера уровня песочницы
#include "SandboxPlayerController.generated.h" // Генерация кода для этого заголовочного файла

class ASandboxObject; // Предварительное объявление класса ASandboxObject
class UUserWidget; // Предварительное объявление класса UUserWidget
class UContainerComponent; // Предварительное объявление класса UContainerComponent

// Структура для хранения информации о игроке
USTRUCT() 
struct FPlayerInfo {
	GENERATED_BODY() // Генерация тела структуры

	UPROPERTY() 
	FString PlayerUid; // Уникальный идентификатор игрока
};

// Класс контроллера игрока в песочнице, наследующий от APlayerController
UCLASS() 
class UNREALSANDBOXTOOLKIT_API ASandboxPlayerController : public APlayerController {
	GENERATED_BODY() // Генерация тела класса

public:
	virtual void BeginPlay() override; // Переопределение метода BeginPlay для инициализации при старте игры

	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	int32 CurrentInventorySlot; // Текущий слот инвентаря

	UPROPERTY(EditAnywhere, Category = "Sandbox") 
	bool bIsAdmin; // Флаг, указывающий, является ли игрок администратором

	ASandboxPlayerController(); // Конструктор класса

	virtual void OnPossess(APawn* NewPawn) override; // Переопределение метода OnPossess для обработки захвата нового пешки

	UFUNCTION(BlueprintCallable, Category = "Sandbox") 
	void BlockGameInput(); // Метод для блокировки ввода игры

	UFUNCTION(BlueprintCallable, Category = "Sandbox") 
	void UnblockGameInput(); // Метод для разблокировки ввода игры

	UFUNCTION(BlueprintCallable, Category = "Sandbox") 
	virtual void SetCurrentInventorySlot(int32 Slot); // Метод для установки текущего слота инвентаря

	UFUNCTION(BlueprintCallable, Category = "Sandbox") 
	bool TakeObjectToInventory(); // Метод для взятия объекта в инвентарь

	UFUNCTION(BlueprintCallable, Category = "Sandbox") 
	bool OpenObjectContainer(ASandboxObject* Obj); // Метод для открытия контейнера объекта

	UFUNCTION(BlueprintCallable, Category = "Sandbox") 
	bool TraceAndOpenObjectContainer(); // Метод трассировки и открытия контейнера объекта

	UFUNCTION(BlueprintCallable, Category = "Sandbox") 
	bool HasOpenContainer(); // Метод проверки наличия открытого контейнера

	UFUNCTION(BlueprintCallable, Category = "Sandbox") 
	UContainerComponent* GetOpenedContainer(); // Метод получения открытого контейнера

	UFUNCTION(BlueprintCallable, Category = "Sandbox") 
	ASandboxObject* GetOpenedObject(); // Метод получения открытого объекта

	UFUNCTION(BlueprintCallable, Category = "Sandbox") 
	void CloseObjectWithContainer(); // Метод закрытия объекта с контейнером

	UFUNCTION(BlueprintCallable, Category = "Sandbox") 
	FHitResult TracePlayerActionPoint(); // Метод трассировки точки действия игрока

	UFUNCTION(BlueprintCallable, Category = "Sandbox") 
	void SelectActionObject(AActor* Actor); // Метод выбора объекта действия

	UFUNCTION(BlueprintCallable, Category = "Sandbox") 
	bool IsGameInputBlocked(); // Метод проверки блокировки ввода игры

	UFUNCTION(BlueprintCallable, Category = "Sandbox") 
	void TraceAndSelectActionObject(); // Метод трассировки и выбора объекта действия

	UFUNCTION(Server, Reliable) 
	void TransferContainerStack(const FString& ObjectNetUid, const FString& ContainerName, const FContainerStack& Stack, const int SlotId); 
    /* Метод передачи стека контейнера на сервер */

	UFUNCTION(Server, Reliable) 
	void TransferInventoryStack(const FString& ContainerName, const FContainerStack& Stack, const int SlotId);  
    /* Метод передачи стека инвентаря на сервер */

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;  // Флаг навигации персонажа к курсору мыши

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;  // Переопределение метода PlayerTick для обновления состояния игрока
	virtual void SetupInputComponent() override;  // Переопределение метода SetupInputComponent для настройки ввода
	// End PlayerController interface

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();  // Метод навигации персонажа к текущему положению курсора мыши

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);  
    /* Метод навигации персонажа к текущему положению касания */

	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);  // Метод установки нового направления движения персонажа

	/** Input handlers for SetDestination action. */
	void SetDestinationPressed();  // Обработчик нажатия на установку направления движения
	void SetDestinationReleased();  // Обработчик отпускания установки направления движения

	// ==========================================================================================================

	virtual void OnMainActionPressed();  // Обработчик нажатия основной действия
	virtual void OnMainActionReleased();  // Обработчик отпускания основной действия
	virtual void OnAltActionPressed();  // Обработчик нажатия альтернативного действия
	virtual void OnAltActionReleased();  // Обработчик отпускания альтернативного действия
	virtual void ToggleView();  // Метод переключения вида 

	UContainerComponent* GetContainerByName(FName ContainerName);  // Метод получения контейнера по имени 

	UContainerComponent* GetInventory();  // Метод получения инвентаря 

	virtual void OnTracePlayerActionPoint(const FHitResult& Res);  // Виртуальный метод обработки трассировки точки действия игрока 

	virtual void OnSelectActionObject(AActor* Actor);  // Виртуальный метод обработки выбора объекта действия 

	virtual void OnDeselectActionObject(AActor* Actor);  // Виртуальный метод обработки снятия выбора с объекта действия 

	UPROPERTY() 
	ASandboxLevelController* LevelController;  // Указатель на контроллер уровня песочницы 

public:
	void ShowMouseCursor(bool bShowCursor);  // Метод отображения или скрытия курсора мыши 

	virtual void OnContainerMainAction(int32 SlotId, FName ContainerName);  
    /* Виртуальный метод обработки основного действия с контейнером */

	virtual void OnContainerDropSuccess(int32 SlotId, FName SourceName, FName TargetName);  
    /* Виртуальный метод обработки успешного сброса с контейнера */

	virtual bool OnContainerDropCheck(int32 SlotId, FName ContainerName, const ASandboxObject* Obj) const;  
    /* Виртуальный метод проверки возможности сброса на контейнер */

	virtual bool OnContainerSlotHover(int32 SlotId, FName ContainerName);  
    /* Виртуальный метод обработки наведения на слот контейнера */

	ASandboxLevelController* GetLevelController();  // Метод получения контроллера уровня 

private:
	void OnMainActionPressedInternal();  /* Внутренний обработчик нажатия основной действия */
	void OnMainActionReleasedInternal(); /* Внутренний обработчик отпускания основной действия */
	void OnAltActionPressedInternal();    /* Внутренний обработчик нажатия альтернативного действия */
	void OnAltActionReleasedInternal();   /* Внутренний обработчик отпускания альтернативного действия */

	FHitResult CurrentPos;  /* Текущая позиция трассировки */
	bool bIsGameInputBlocked;  /* Флаг блокировки ввода игры */

	UPROPERTY() 
	ASandboxObject* SelectedObject;  /* Указатель на выбранный объект */

	UPROPERTY() 
	ASandboxObject* OpenedObject;   /* Указатель на открытый объект */

	UPROPERTY() 
	UContainerComponent* OpenedContainer;   /* Указатель на открытый контейнер */
};
