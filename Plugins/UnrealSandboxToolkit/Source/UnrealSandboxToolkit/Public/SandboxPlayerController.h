#pragma once

// Подключаем основные заголовочные файлы Unreal Engine
#include "Engine.h"
#include "GameFramework/PlayerController.h"
#include "SandboxLevelController.h"
#include "SandboxPlayerController.generated.h"

// Предварительные объявления классов, которые будут использоваться
class ASandboxObject;
class UUserWidget;
class UContainerComponent;

// Структура для хранения информации об игроке
USTRUCT()
struct FPlayerInfo {
	GENERATED_BODY()

	// Уникальный идентификатор игрока
	UPROPERTY()
	FString PlayerUid;
};

// Основной класс контроллера игрока, наследуется от стандартного контроллера игрока UE
UCLASS()
class UNREALSANDBOXTOOLKIT_API ASandboxPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Вызывается при начале игры
	virtual void BeginPlay() override;

	// Текущий выбранный слот инвентаря
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	int32 CurrentInventorySlot;

	// Флаг, указывающий является ли игрок администратором
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	bool bIsAdmin;

	// Конструктор класса
	ASandboxPlayerController();

	// Вызывается при вселении в пешку (персонажа)
	virtual void OnPossess(APawn* NewPawn) override;

	// Методы для блокировки/разблокировки игрового ввода
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	void BlockGameInput();

	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	void UnblockGameInput();

	// Установка текущего слота инвентаря
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	virtual void SetCurrentInventorySlot(int32 Slot);

	// Методы для работы с инвентарем и контейнерами
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	bool TakeObjectToInventory();

	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	bool OpenObjectContainer(ASandboxObject* Obj);

	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	bool TraceAndOpenObjectContainer();

	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	bool HasOpenContainer();

	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	UContainerComponent* GetOpenedContainer();

	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	ASandboxObject* GetOpenedObject();

	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	void CloseObjectWithContainer();

	// Методы для взаимодействия с объектами в мире
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	FHitResult TracePlayerActionPoint();

	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	void SelectActionObject(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	bool IsGameInputBlocked();

	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	void TraceAndSelectActionObject();

	// Сетевые функции для передачи предметов между контейнерами
	UFUNCTION(Server, Reliable)
	void TransferContainerStack(const FString& ObjectNetUid, const FString& ContainerName, const FContainerStack& Stack, const int SlotId);

	UFUNCTION(Server, Reliable)
	void TransferInventoryStack(const FString& ContainerName, const FContainerStack& Stack, const int SlotId);

protected:
	// Флаг для перемещения к курсору мыши
	uint32 bMoveToMouseCursor : 1;

	// Переопределенные методы тика и настройки ввода
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

	// Методы для перемещения игрока
	void MoveToMouseCursor();
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	void SetNewMoveDestination(const FVector DestLocation);
	void SetDestinationPressed();
	void SetDestinationReleased();

	// Виртуальные методы для обработки действий игрока
	virtual void OnMainActionPressed();
	virtual void OnMainActionReleased();
	virtual void OnAltActionPressed();
	virtual void OnAltActionReleased();
	virtual void ToggleView();

	// Вспомогательные методы для работы с контейнерами
	UContainerComponent* GetContainerByName(FName ContainerName);
	UContainerComponent* GetInventory();

	// Методы обработки взаимодействия с объектами
	virtual void OnTracePlayerActionPoint(const FHitResult& Res);
	virtual void OnSelectActionObject(AActor* Actor);
	virtual void OnDeselectActionObject(AActor* Actor);

	// Ссылка на контроллер уровня
	UPROPERTY()
	ASandboxLevelController* LevelController;

public:
	// Управление видимостью курсора мыши
	void ShowMouseCursor(bool bShowCursor);

	// Виртуальные методы для обработки действий с контейнерами
	virtual void OnContainerMainAction(int32 SlotId, FName ContainerName);
	virtual void OnContainerDropSuccess(int32 SlotId, FName SourceName, FName TargetName);
	virtual bool OnContainerDropCheck(int32 SlotId, FName ContainerName, const ASandboxObject* Obj) const;
	virtual bool OnContainerSlotHover(int32 SlotId, FName ContainerName);

	// Получение контроллера уровня
	ASandboxLevelController* GetLevelController();

private:
	// Внутренние методы обработки действий
	void OnMainActionPressedInternal();
	void OnMainActionReleasedInternal();
	void OnAltActionPressedInternal();
	void OnAltActionReleasedInternal();

	// Приватные переменные для хранения состояния
	FHitResult CurrentPos;
	bool bIsGameInputBlocked;

	// Ссылки на выбранные и открытые объекты
	UPROPERTY()
	ASandboxObject* SelectedObject;

	UPROPERTY()
	ASandboxObject* OpenedObject;

	UPROPERTY()
	UContainerComponent* OpenedContainer;
};


