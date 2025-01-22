#pragma once // Директива препроцессора для предотвращения множественного включения заголовочного файла

#include "Engine.h" // Подключение основного движка Unreal Engine
#include "GameFramework/PlayerController.h" // Подключение заголовочного файла для класса PlayerController
#include "SandboxLevelController.h" // Подключение пользовательского заголовочного файла для SandboxLevelController
#include "SandboxPlayerController.generated.h" // Генерация метаданных для Unreal Engine

class ASandboxObject; // Предварительное объявление класса ASandboxObject
class UUserWidget; // Предварительное объявление класса UUserWidget
class UContainerComponent; // Предварительное объявление класса UContainerComponent

USTRUCT() // Макрос для создания структуры, доступной в редакторе Unreal
struct FPlayerInfo { // Определение структуры FPlayerInfo
	GENERATED_BODY() // Макрос для генерации тела структуры

	UPROPERTY() // Макрос для создания свойства, доступного в редакторе Unreal
	FString PlayerUid; // Свойство для хранения идентификатора игрока
};

UCLASS() // Макрос для создания класса, доступного в редакторе Unreal
class UNREALSANDBOXTOOLKIT_API ASandboxPlayerController : public APlayerController // Определение класса ASandboxPlayerController, наследующегося от APlayerController
{
	GENERATED_BODY() // Макрос для генерации тела класса

public:
	virtual void BeginPlay() override; // Переопределение метода BeginPlay для выполнения начальных действий при старте игры

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Макрос для создания свойства, доступного для редактирования в редакторе Unreal
	int32 CurrentInventorySlot; // Свойство для хранения текущего слота инвентаря

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Макрос для создания свойства, доступного для редактирования в редакторе Unreal
	bool bIsAdmin; // Свойство для хранения информации о том, является ли игрок администратором

	ASandboxPlayerController(); // Конструктор класса ASandboxPlayerController

	virtual void OnPossess(APawn* NewPawn) override; // Переопределение метода OnPossess для выполнения действий при обладании новым персонажем

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Макрос для создания функции, доступной в Blueprint и редакторе Unreal
	void BlockGameInput(); // Метод для блокировки игрового ввода

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Макрос для создания функции, доступной в Blueprint и редакторе Unreal
	void UnblockGameInput(); // Метод для разблокировки игрового ввода

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Макрос для создания функции, доступной в Blueprint и редакторе Unreal
	virtual void SetCurrentInventorySlot(int32 Slot); // Метод для установки текущего слота инвентаря

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Макрос для создания функции, доступной в Blueprint и редакторе Unreal
	bool TakeObjectToInventory(); // Метод для взятия объекта в инвентарь

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Макрос для создания функции, доступной в Blueprint и редакторе Unreal
	bool OpenObjectContainer(ASandboxObject* Obj); // Метод для открытия контейнера объекта

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Макрос для создания функции, доступной в Blueprint и редакторе Unreal
	bool TraceAndOpenObjectContainer(); // Метод для трассировки и открытия контейнера объекта

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Макрос для создания функции, доступной в Blueprint и редакторе Unreal
	bool HasOpenContainer(); // Метод для проверки наличия открытого контейнера

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Макрос для создания функции, доступной в Blueprint и редакторе Unreal
	UContainerComponent* GetOpenedContainer(); // Метод для получения открытого контейнера

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Макрос для создания функции, доступной в Blueprint и редакторе Unreal
	ASandboxObject* GetOpenedObject(); // Метод для получения открытого объекта

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Макрос для создания функции, доступной в Blueprint и редакторе Unreal
	void CloseObjectWithContainer(); // Метод для закрытия объекта с контейнером

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Макрос для создания функции, доступной в Blueprint и редакторе Unreal
	FHitResult TracePlayerActionPoint(); // Метод для трассировки точки действия игрока

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Макрос для создания функции, доступной в Blueprint и редакторе Unreal
	void SelectActionObject(AActor* Actor); // Метод для выбора объекта действия

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Макрос для создания функции, доступной в Blueprint и редакторе Unreal
	bool IsGameInputBlocked(); // Метод для проверки, заблокирован ли игровой ввод

	UFUNCTION(BlueprintCallable, Category = "Sandbox") // Макрос для создания функции, доступной в Blueprint и редакторе Unreal
	void TraceAndSelectActionObject(); // Метод для трассировки и выбора объекта действия

	UFUNCTION(Server, Reliable) // Макрос для создания функции, вызываемой на сервере и надежной
	void TransferContainerStack(const FString& ObjectNetUid, const FString& ContainerName, const FContainerStack& Stack, const int SlotId); // Метод для передачи стека контейнера

	UFUNCTION(Server, Reliable) // Макрос для создания функции, вызываемой на сервере и надежной
	void TransferInventoryStack(const FString& ContainerName, const FContainerStack& Stack, const int SlotId); // Метод для передачи стека инвентаря

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1; // Флаг для перемещения персонажа к курсору мыши

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override; // Переопределение метода PlayerTick для выполнения действий каждый тик
	virtual void SetupInputComponent() override; // Переопределение метода SetupInputComponent для настройки компонента ввода
	// End PlayerController interface

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor(); // Метод для перемещения персонажа к текущему положению курсора мыши

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location); // Метод для перемещения персонажа к текущему положению касания

	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation); // Метод для установки новой цели перемещения

	/** Input handlers for SetDestination action. */
	void SetDestinationPressed(); // Метод для обработки нажатия на установку цели

	void SetDestinationReleased(); // Метод для обработки отпускания установки цели

	// ==========================================================================================================

	virtual void OnMainActionPressed(); // Метод для обработки нажатия основного действия

	virtual void OnMainActionReleased(); // Метод для обработки отпускания основного действия

	virtual void OnAltActionPressed(); // Метод для обработки нажатия альтернативного действия

	virtual void OnAltActionReleased(); // Метод для обработки отпускания альтернативного действия

	virtual void ToggleView(); // Метод для переключения вида

	UContainerComponent* GetContainerByName(FName ContainerName); // Метод для получения контейнера по имени

	UContainerComponent* GetInventory(); // Метод для получения инвентаря

	virtual void OnTracePlayerActionPoint(const FHitResult& Res); // Метод для обработки трассировки точки действия игрока

	virtual void OnSelectActionObject(AActor* Actor); // Метод для обработки выбора объекта действия

	virtual void OnDeselectActionObject(AActor* Actor); // Метод для обработки отмены выбора объекта действия

	UPROPERTY() // Макрос для создания свойства, доступного в редакторе Unreal
	ASandboxLevelController* LevelController; // Свойство для хранения контроллера уровня

public:

	void ShowMouseCursor(bool bShowCursor); // Метод для показа или скрытия курсора мыши

	virtual void OnContainerMainAction(int32 SlotId, FName ContainerName); // Метод для обработки основного действия контейнера

	virtual void OnContainerDropSuccess(int32 SlotId, FName SourceName, FName TargetName); // Метод для обработки успешного выброса из контейнера

	virtual bool OnContainerDropCheck(int32 SlotId, FName ContainerName, const ASandboxObject* Obj) const; // Метод для проверки возможности выброса из контейнера

	virtual bool OnContainerSlotHover(int32 SlotId, FName ContainerName); // Метод для обработки наведения на слот контейнера

	ASandboxLevelController* GetLevelController(); // Метод для получения контроллера уровня

private:

	void OnMainActionPressedInternal(); // Внутренний метод для обработки нажатия основного действия

	void OnMainActionReleasedInternal(); // Внутренний метод для обработки отпускания основного действия

	void OnAltActionPressedInternal(); // Внутренний метод для обработки нажатия альтернативного действия

	void OnAltActionReleasedInternal(); // Внутренний метод для обработки отпускания альтернативного действия

	FHitResult CurrentPos; // Свойство для хранения текущей позиции трассировки

	bool bIsGameInputBlocked; // Свойство для хранения информации о том, заблокирован ли игровой ввод

	UPROPERTY() // Макрос для создания свойства, доступного в редакторе Unreal
	ASandboxObject* SelectedObject; // Свойство для хранения выбранного объекта

	UPROPERTY() // Макрос для создания свойства, доступного в редакторе Unreal
	ASandboxObject* OpenedObject; // Свойство для хранения открытого объекта

	UPROPERTY() // Макрос для создания свойства, доступного в редакторе Unreal
	UContainerComponent* OpenedContainer; // Свойство для хранения открытого контейнера
};
