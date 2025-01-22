
#pragma once

#include "Engine.h"
#include "GameFramework/PlayerController.h"
#include "SandboxLevelController.h"
#include "SandboxPlayerController.generated.h"

// Предварительное объявление классов
class ASandboxObject;
class UUserWidget;
class UContainerComponent;

// Структура для хранения информации о игроке
USTRUCT()
struct FPlayerInfo {
	GENERATED_BODY()

	// Уникальный идентификатор игрока
	UPROPERTY()
	FString PlayerUid;
};

// Объявление класса ASandboxPlayerController, наследуемого от APlayerController
UCLASS()
class UNREALSANDBOXTOOLKIT_API ASandboxPlayerController : public APlayerController {
	GENERATED_BODY()

public:
	// Метод, вызываемый при начале игры
	virtual void BeginPlay() override;

	// Текущий слот инвентаря
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	int32 CurrentInventorySlot;

	// Флаг, указывающий, является ли игрок администратором
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	bool bIsAdmin;

	// Конструктор класса
	ASandboxPlayerController();

	// Метод, вызываемый при обладании новым персонажем
	virtual void OnPossess(APawn* NewPawn) override;

	// Метод для блокировки игрового ввода
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	void BlockGameInput();

	// Метод для разблокировки игрового ввода
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	void UnblockGameInput();

	// Метод для установки текущего слота инвентаря
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	virtual void SetCurrentInventorySlot(int32 Slot);

	// Метод для взятия объекта в инвентарь
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	bool TakeObjectToInventory();

	// Метод для открытия контейнера объекта
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	bool OpenObjectContainer(ASandboxObject* Obj);

	// Метод для трассировки и открытия контейнера объекта
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	bool TraceAndOpenObjectContainer();

	// Метод для проверки наличия открытого контейнера
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	bool HasOpenContainer();

	// Метод для получения открытого контейнера
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	UContainerComponent* GetOpenedContainer();

	// Метод для получения открытого объекта
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	ASandboxObject* GetOpenedObject();

	// Метод для закрытия объекта с контейнером
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	void CloseObjectWithContainer();

	// Метод для трассировки точки действия игрока
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	FHitResult TracePlayerActionPoint();

	// Метод для выбора объекта действия
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	void SelectActionObject(AActor* Actor);

	// Метод для проверки, заблокирован ли игровой ввод
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	bool IsGameInputBlocked();

	// Метод для трассировки и выбора объекта действия
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	void TraceAndSelectActionObject();

	// Метод для перемещения стека объектов в контейнер
	UFUNCTION(Server, Reliable)
	void TransferContainerStack(const FString& ObjectNetUid, const FString& ContainerName, const FContainerStack& Stack, const int SlotId);

	// Метод для перемещения стека объектов в инвентарь
	UFUNCTION(Server, Reliable)
	void TransferInventoryStack(const FString& ContainerName, const FContainerStack& Stack, const int SlotId);

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	// Метод для обновления игрока каждый тик
	virtual void PlayerTick(float DeltaTime) override;
	// Метод для настройки компонента ввода
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Перемещение игрока к текущей позиции курсора мыши. */
	void MoveToMouseCursor();

	/** Перемещение игрока к текущей позиции касания. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Перемещение игрока к заданной позиции в мире. */
	void SetNewMoveDestination(const FVector DestLocation);

	/** Обработчики ввода для действия SetDestination. */
	void SetDestinationPressed();

	void SetDestinationReleased();

	// ==========================================================================================================

	// Метод для обработки нажатия основного действия
	virtual void OnMainActionPressed();

	// Метод для обработки отпускания основного действия
	virtual void OnMainActionReleased();

	// Метод для обработки нажатия альтернативного действия
	virtual void OnAltActionPressed();

	// Метод для обработки отпускания альтернативного действия
	virtual void OnAltActionReleased();

	// Метод для переключения вида
	virtual void ToggleView();

	// Метод для получения контейнера по имени
	UContainerComponent* GetContainerByName(FName ContainerName);

	// Метод для получения инвентаря
	UContainerComponent* GetInventory();

	// Метод для обработки трассировки точки действия игрока
	virtual void OnTracePlayerActionPoint(const FHitResult& Res);

	// Метод для обработки выбора объекта действия
	virtual void OnSelectActionObject(AActor* Actor);

	// Метод для обработки отмены выбора объекта действия
	virtual void OnDeselectActionObject(AActor* Actor);

	// Контроллер уровня
	UPROPERTY()
	ASandboxLevelController* LevelController;

public:
	// Метод для показа или скрытия курсора мыши
	void ShowMouseCursor(bool bShowCursor);

	// Метод для обработки основного действия контейнера
	virtual void OnContainerMainAction(int32 SlotId, FName ContainerName);

	// Метод для обработки успешного перемещения объекта между контейнерами
	virtual void OnContainerDropSuccess(int32 SlotId, FName SourceName, FName TargetName);

	// Метод для проверки возможности перемещения объекта между контейнерами
	virtual bool OnContainerDropCheck(int32 SlotId, FName ContainerName, const ASandboxObject* Obj) const;

	// Метод для обработки наведения на слот контейнера
	virtual bool OnContainerSlotHover(int32 SlotId, FName ContainerName);

	// Метод для получения контроллера уровня
	ASandboxLevelController* GetLevelController();

private:
	// Метод для обработки нажатия основного действия внутри
	void OnMainActionPressedInternal();

	// Метод для обработки отпускания основного действия внутри
	void OnMainActionReleasedInternal();

	// Метод для обработки нажатия альтернативного действия внутри
	void OnAltActionPressedInternal();

	// Метод для обработки отпускания альтернативного действия внутри
	void OnAltActionReleasedInternal();

	// Текущая позиция
	FHitResult CurrentPos;

	// Флаг, указывающий, заблокирован ли игровой ввод
	bool bIsGameInputBlocked;

	// Выбранный объект
	UPROPERTY()
	ASandboxObject* SelectedObject;

	// Открытый объект
	UPROPERTY()
	ASandboxObject* OpenedObject;

	// Открытый контейнер
	UPROPERTY()
	UContainerComponent* OpenedContainer;
};
