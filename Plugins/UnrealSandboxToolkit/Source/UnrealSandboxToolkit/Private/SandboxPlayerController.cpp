#include "SandboxPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "SandboxCharacter.h"
#include "SandboxObject.h"
#include "ContainerComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

// Основной контроллер игрока, управляющий всем взаимодействием с игровым миром
ASandboxPlayerController::ASandboxPlayerController() {
	//bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default; // Устанавливаем стандартный курсор мыши
	CurrentInventorySlot = -1; // Изначально нет выбранного слота инвентаря
	bIsGameInputBlocked = false; // По умолчанию ввод не заблокирован
}

// Вызывается при старте игры
void ASandboxPlayerController::BeginPlay() {
	Super::BeginPlay();

	// Ищем контроллер уровня в мире
	LevelController = nullptr;
	for (TActorIterator<ASandboxLevelController> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		ASandboxLevelController* LevelCtrl = Cast<ASandboxLevelController>(*ActorItr);
		if (LevelCtrl) {
			UE_LOG(LogTemp, Log, TEXT("Found ALevelController -> %s"), *LevelCtrl->GetName());
			LevelController = LevelCtrl;
			break;
		}
	}
}

// Обновление каждый кадр
void ASandboxPlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime);

	// Если включено движение к курсору - двигаемся
	if (bMoveToMouseCursor)	{
		MoveToMouseCursor();
	}
}

// Настройка управления
void ASandboxPlayerController::SetupInputComponent() {
	Super::SetupInputComponent();

	// Привязываем основные действия к кнопкам
	InputComponent->BindAction("MainAction", IE_Pressed, this, &ASandboxPlayerController::OnMainActionPressedInternal);
	InputComponent->BindAction("MainAction", IE_Released, this, &ASandboxPlayerController::OnMainActionReleasedInternal);

	InputComponent->BindAction("AltAction", IE_Pressed, this, &ASandboxPlayerController::OnAltActionPressedInternal);
	InputComponent->BindAction("AltAction", IE_Released, this, &ASandboxPlayerController::OnAltActionReleasedInternal);

	// support touch devices 
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AUE4VoxelTerrainPlayerController::MoveToTouchLocation);
	//InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AUE4VoxelTerrainPlayerController::MoveToTouchLocation);

	InputComponent->BindAction("ToggleView", IE_Pressed, this, &ASandboxPlayerController::ToggleView);
}

// Перемещение к позиции курсора мыши
void ASandboxPlayerController::MoveToMouseCursor() {
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldStatic, false, Hit);

	if (Hit.bBlockingHit) {
		// Если попали во что-то - двигаемся туда
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

// Перемещение при касании (для мобильных устройств)
void ASandboxPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location) {
	FVector2D ScreenSpaceLocation(Location);

	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit) {
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

// Установка новой точки назначения для движения
void ASandboxPlayerController::SetNewMoveDestination(const FVector DestLocation) {
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter());

	if (SandboxCharacter) {
		float const Distance = FVector::Dist(DestLocation, SandboxCharacter->GetActorLocation());
		// Двигаемся только если точка достаточно далеко
		if (Distance > 120.0f) {
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

// Обработка нажатия кнопки перемещения
void ASandboxPlayerController::SetDestinationPressed() {
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter());
	if (!SandboxCharacter) {
		return;
	}

	// Проверяем, что мы в режиме вида сверху и персонаж жив
	if (SandboxCharacter->GetSandboxPlayerView() != PlayerView::TOP_DOWN) {
		return;
	}

	if (SandboxCharacter->IsDead()) {
		return;
	}

	bMoveToMouseCursor = true;
}

// Обработка отпускания кнопки перемещения
void ASandboxPlayerController::SetDestinationReleased() {
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter());
	if (!SandboxCharacter) {
		return;
	}

	if (SandboxCharacter->GetSandboxPlayerView() != PlayerView::TOP_DOWN) {
		return;
	}

	if (SandboxCharacter->IsDead()) {
		return;
	}

	bMoveToMouseCursor = false;
}

// Внутренние обработчики действий (проверяют блокировку ввода)
void ASandboxPlayerController::OnMainActionPressedInternal() {
	if (!IsGameInputBlocked()) {
		OnMainActionPressed();
	}
}

void ASandboxPlayerController::OnMainActionReleasedInternal() {
	if (!IsGameInputBlocked()) {
		OnMainActionReleased();
	}
}

void ASandboxPlayerController::OnAltActionPressedInternal() {
	if (!IsGameInputBlocked()) {
		OnAltActionPressed();
	}
}

void ASandboxPlayerController::OnAltActionReleasedInternal() {
	if (!IsGameInputBlocked()) {
		OnAltActionReleased();
	}
}

// Пустые функции для переопределения в Blueprint
void ASandboxPlayerController::OnMainActionPressed() {

}

void ASandboxPlayerController::OnMainActionReleased() {

}

void ASandboxPlayerController::OnAltActionPressed() {

}

void ASandboxPlayerController::OnAltActionReleased() {

}

// Переключение вида камеры
void ASandboxPlayerController::ToggleView() {
	if (IsGameInputBlocked()) {
		return;
	}
}

// Вызывается при получении контроля над персонажем
void ASandboxPlayerController::OnPossess(APawn* NewPawn) {
	Super::OnPossess(NewPawn);

	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(NewPawn);
	if (SandboxCharacter) {
		SandboxCharacter->EnableInput(this);
		if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
			//bShowMouseCursor = true;
		} else {
			//bShowMouseCursor = false;
		}
	}

	//bShowMouseCursor = false;
}

// Блокировка игрового ввода
void ASandboxPlayerController::BlockGameInput() {
	//UWidgetBlueprintLibrary::SetInputMode_GameAndUI(this, nullptr, false, false);
	bIsGameInputBlocked = true;
	//bShowMouseCursor = true;
}

// Разблокировка игрового ввода
void ASandboxPlayerController::UnblockGameInput() {
	//UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
	bIsGameInputBlocked = false;
	//bShowMouseCursor = false;
}

// Поиск и выбор объекта для взаимодействия
void ASandboxPlayerController::TraceAndSelectActionObject() {
	if (!IsGameInputBlocked()) {
		FHitResult Res = TracePlayerActionPoint();
		OnTracePlayerActionPoint(Res);
		if (Res.bBlockingHit) {
			AActor* SelectedActor = Res.GetActor();
			if (SelectedActor) {
				SelectActionObject(SelectedActor);
			}
		}
	}
}

// Трассировка луча для определения точки взаимодействия игрока
FHitResult ASandboxPlayerController::TracePlayerActionPoint() {
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter());
	if (!SandboxCharacter) {
		return FHitResult();
	}

	// Разная логика трассировки для разных режимов камеры
	if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::THIRD_PERSON || SandboxCharacter->GetSandboxPlayerView() == PlayerView::FIRST_PERSON) {
		float MaxUseDistance = SandboxCharacter->InteractionTargetLength;

		if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::THIRD_PERSON) {
			if (SandboxCharacter->GetCameraBoom() != NULL) {
				//MaxUseDistance = Character->GetCameraBoom()->TargetArmLength + MaxUseDistance;
			}
		}

		FVector CamLoc;
		FRotator CamRot;
		GetPlayerViewPoint(CamLoc, CamRot);

		const FVector StartTrace = CamLoc;
		const FVector Direction = CamRot.Vector();
		const FVector EndTrace = StartTrace + (Direction * MaxUseDistance);

		FCollisionQueryParams TraceParams(FName(TEXT("")), true, this);
		//TraceParams.bTraceAsyncScene = true;
		//TraceParams.bReturnPhysicalMaterial = false;

		TraceParams.bTraceComplex = true;
		TraceParams.bReturnFaceIndex = true;
		TraceParams.AddIgnoredActor(SandboxCharacter);

		FHitResult Hit(ForceInit);
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, TraceParams);

		return Hit;
	}

	if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Camera, false, Hit);
		return Hit;
	}

	return FHitResult();
}

// Вспомогательная функция для установки подсветки объекта
void SetRenderCustomDepth2(AActor* Actor, bool RenderCustomDepth) {
	TArray<UStaticMeshComponent*> MeshComponentList;
	Actor->GetComponents<UStaticMeshComponent>(MeshComponentList);

	for (UStaticMeshComponent* MeshComponent : MeshComponentList) {
		MeshComponent->SetRenderCustomDepth(RenderCustomDepth);
	}
}

// Обработка выбора объекта для взаимодействия
void ASandboxPlayerController::OnSelectActionObject(AActor* Actor) {
	SetRenderCustomDepth2(Actor, true);
}

// Обработка отмены выбора объекта
void ASandboxPlayerController::OnDeselectActionObject(AActor* Actor) {
	SetRenderCustomDepth2(Actor, false);
}

// Выбор объекта для взаимодействия
void ASandboxPlayerController::SelectActionObject(AActor* Actor) {
	ASandboxObject* Obj = Cast<ASandboxObject>(Actor);

	if (SelectedObject != Obj) {
		if (SelectedObject != nullptr && SelectedObject->IsValidLowLevel()) {
			OnDeselectActionObject(SelectedObject);
		}
	}

	if (Obj != nullptr) {
		OnSelectActionObject(Obj);
		SelectedObject = Obj;
	} else {
		if (SelectedObject != nullptr && SelectedObject->IsValidLowLevel()) {
			OnDeselectActionObject(SelectedObject);
		}
	}
}

// Получение компонента контейнера по имени
UContainerComponent* ASandboxPlayerController::GetContainerByName(FName ContainerName) {
	APawn* PlayerPawn = GetPawn();
	if (PlayerPawn) {
		TArray<UContainerComponent*> Components;
		PlayerPawn->GetComponents<UContainerComponent>(Components);

		for (UContainerComponent* Container : Components) {
			if (Container->GetName() == ContainerName.ToString()) {
				return Container;
			}
		}
	}

	return nullptr;
}

// Получение инвентаря игрока
UContainerComponent* ASandboxPlayerController::GetInventory() {
	return GetContainerByName(TEXT("Inventory"));
}

// Подбор объекта в инвентарь
bool ASandboxPlayerController::TakeObjectToInventory() {
	UContainerComponent* Inventory = GetInventory();

	if (Inventory != nullptr) {
		FHitResult ActionPoint = TracePlayerActionPoint();
		if (ActionPoint.bBlockingHit) {
			ASandboxObject* Obj = Cast<ASandboxObject>(ActionPoint.GetActor());
			if (Obj) {
				if (Obj->CanTake(nullptr)) {
					if (Inventory->AddObject(Obj)) {
						if (LevelController) {
							LevelController->RemoveSandboxObject(Obj);
						} else {
							Obj->Destroy();
						}

						return true;
					}
				}
			}
		}
	}

	return false;
}

// Открытие контейнера объекта
bool ASandboxPlayerController::OpenObjectContainer(ASandboxObject* Obj) {
	if (Obj != nullptr) {
		TArray<UContainerComponent*> Components;
		Obj->GetComponents<UContainerComponent>(Components);
		for (UContainerComponent* Container : Components) {
			if (Container->GetName() == "ObjectContainer") {
				this->OpenedObject = Obj;
				this->OpenedContainer = Container;
				return true;
			}
		}
	}

	return false;
}

// Проверка наличия открытого контейнера
bool ASandboxPlayerController::HasOpenContainer() { 
	return OpenedObject != nullptr; 
}

// Поиск и открытие контейнера объекта под прицелом
bool ASandboxPlayerController::TraceAndOpenObjectContainer() {
	FHitResult ActionPoint = TracePlayerActionPoint();

	if (ActionPoint.bBlockingHit) {
		ASandboxObject* Obj = Cast<ASandboxObject>(ActionPoint.GetActor());
		return OpenObjectContainer(Obj);
	}

	return false;
}

// Закрытие открытого контейнера
void ASandboxPlayerController::CloseObjectWithContainer() {
	this->OpenedObject = nullptr;
	this->OpenedContainer = nullptr;
}

// Пустая функция для переопределения в Blueprint
void ASandboxPlayerController::OnTracePlayerActionPoint(const FHitResult& Res) {

}

// Проверка блокировки игрового ввода
bool ASandboxPlayerController::IsGameInputBlocked() {
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter());
	if (SandboxCharacter && !SandboxCharacter->InputEnabled()) {
		return true;
	}

	return bIsGameInputBlocked; 
}

// Установка текущего слота инвентаря
void ASandboxPlayerController::SetCurrentInventorySlot(int32 Slot) { 
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter());
	if (SandboxCharacter && SandboxCharacter->IsDead()) {
		return;
	}

	CurrentInventorySlot = Slot; 
}

// Геттеры для доступа к открытому контейнеру и объекту
UContainerComponent* ASandboxPlayerController::GetOpenedContainer() { 
	return this->OpenedContainer; 
}

ASandboxObject* ASandboxPlayerController::GetOpenedObject() { 
	return this->OpenedObject; 
}

// Управление видимостью курсора мыши
void ASandboxPlayerController::ShowMouseCursor(bool bShowCursor) { 
	this->bShowMouseCursor = bShowCursor; 
};

// Пустые функции для переопределения в Blueprint
void ASandboxPlayerController::OnContainerMainAction(int32 SlotId, FName ContainerName) {

}

void ASandboxPlayerController::OnContainerDropSuccess(int32 SlotId, FName SourceName, FName TargetName) {

}

bool ASandboxPlayerController::OnContainerDropCheck(int32 SlotId, FName ContainerName, const ASandboxObject* Obj) const {
	return true;
}

// Сетевые функции для синхронизации контейнеров
void ASandboxPlayerController::TransferContainerStack_Implementation(const FString& ObjectNetUid, const FString& ContainerName, const FContainerStack& Stack, const int SlotId) {
	if (LevelController) {
		ASandboxObject* Obj = LevelController->GetObjectByNetUid(ObjectNetUid);
		if (Obj) {
			TArray<UContainerComponent*> Components;
			Obj->GetComponents<UContainerComponent>(Components);
			for (UContainerComponent* Container : Components) {
				if (Container->GetName() == ContainerName) {
					Container->SetStackDirectly(Stack, SlotId);
				}
			}

			Obj->ForceNetUpdate();
		}
	}
}

void ASandboxPlayerController::TransferInventoryStack_Implementation(const FString& ContainerName, const FContainerStack& Stack, const int SlotId) {
	ACharacter* PlayerCharacter = Cast<ACharacter>(GetCharacter());
	TArray<UContainerComponent*> Components;
	PlayerCharacter->GetComponents<UContainerComponent>(Components);
	for (UContainerComponent* Container : Components) {
		if (Container->GetName() == ContainerName) {
			Container->SetStackDirectly(Stack, SlotId);
		}
	}

	PlayerCharacter->ForceNetUpdate();
}

// Получение контроллера уровня
ASandboxLevelController* ASandboxPlayerController::GetLevelController() {
	return LevelController;
}

// Обработка наведения на слот контейнера
bool ASandboxPlayerController::OnContainerSlotHover(int32 SlotId, FName ContainerName) {
	return false;
}