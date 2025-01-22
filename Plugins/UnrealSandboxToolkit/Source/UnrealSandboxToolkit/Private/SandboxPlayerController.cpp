#include "SandboxPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "SandboxCharacter.h"
#include "SandboxObject.h"
#include "ContainerComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

// Конструктор класса ASandboxPlayerController
ASandboxPlayerController::ASandboxPlayerController() {
	//bShowMouseCursor = true; // Показать курсор мыши
	DefaultMouseCursor = EMouseCursor::Default; // Установить курсор мыши по умолчанию
	CurrentInventorySlot = -1; // Текущий слот инвентаря
	bIsGameInputBlocked = false; // Флаг блокировки ввода игры
}

// Метод, вызываемый при начале игры
void ASandboxPlayerController::BeginPlay() {
	Super::BeginPlay(); // Вызов родительского метода

	LevelController = nullptr; // Инициализация контроллера уровня
	// Поиск контроллера уровня в мире
	for (TActorIterator<ASandboxLevelController> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		ASandboxLevelController* LevelCtrl = Cast<ASandboxLevelController>(*ActorItr);
		if (LevelCtrl) {
			UE_LOG(LogTemp, Log, TEXT("Found ALevelController -> %s"), *LevelCtrl->GetName()); // Логирование найденного контроллера уровня
			LevelController = LevelCtrl; // Установка найденного контроллера уровня
			break;
		}
	}
}

// Метод, вызываемый каждый тик игры
void ASandboxPlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime); // Вызов родительского метода

	if (bMoveToMouseCursor)	{
		MoveToMouseCursor(); // Перемещение к курсору мыши
	}
}

// Настройка компонента ввода
void ASandboxPlayerController::SetupInputComponent() {
	// Настройка привязок клавиш для игрового процесса
	Super::SetupInputComponent(); // Вызов родительского метода

	// Привязка действий к методам
	InputComponent->BindAction("MainAction", IE_Pressed, this, &ASandboxPlayerController::OnMainActionPressedInternal);
	InputComponent->BindAction("MainAction", IE_Released, this, &ASandboxPlayerController::OnMainActionReleasedInternal);

	InputComponent->BindAction("AltAction", IE_Pressed, this, &ASandboxPlayerController::OnAltActionPressedInternal);
	InputComponent->BindAction("AltAction", IE_Released, this, &ASandboxPlayerController::OnAltActionReleasedInternal);

	// Поддержка сенсорных устройств
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AUE4VoxelTerrainPlayerController::MoveToTouchLocation);
	//InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AUE4VoxelTerrainPlayerController::MoveToTouchLocation);

	InputComponent->BindAction("ToggleView", IE_Pressed, this, &ASandboxPlayerController::ToggleView); // Переключение вида
}

// Метод для перемещения к курсору мыши
void ASandboxPlayerController::MoveToMouseCursor() {
	FHitResult Hit; // Результат попадания
	GetHitResultUnderCursor(ECC_WorldStatic, false, Hit); // Получение результата попадания под курсором

	if (Hit.bBlockingHit) {
		// Мы попали во что-то, перемещаемся туда
		SetNewMoveDestination(Hit.ImpactPoint); // Установка новой цели перемещения
	}
}

// Метод для перемещения к месту касания
void ASandboxPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location) {
	FVector2D ScreenSpaceLocation(Location); // Позиция на экране

	FHitResult HitResult; // Результат попадания
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult); // Получение результата попадания на экране
	if (HitResult.bBlockingHit) {
		SetNewMoveDestination(HitResult.ImpactPoint); // Установка новой цели перемещения
	}
}

// Метод для установки новой цели перемещения
void ASandboxPlayerController::SetNewMoveDestination(const FVector DestLocation) {
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter()); // Получение персонажа

	if (SandboxCharacter) {
		float const Distance = FVector::Dist(DestLocation, SandboxCharacter->GetActorLocation()); // Расчет расстояния до цели
		if (Distance > 120.0f) {
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation); // Перемещение к цели
		}
	}
}

// Метод для установки цели перемещения при нажатии
void ASandboxPlayerController::SetDestinationPressed() {
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter()); // Получение персонажа
	if (!SandboxCharacter) {
		return;
	}

	if (SandboxCharacter->GetSandboxPlayerView() != PlayerView::TOP_DOWN) {
		return;
	}

	if (SandboxCharacter->IsDead()) {
		return;
	}

	bMoveToMouseCursor = true; // Установка флага перемещения к курсору мыши
}

// Метод для снятия цели перемещения при отпускании
void ASandboxPlayerController::SetDestinationReleased() {
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter()); // Получение персонажа
	if (!SandboxCharacter) {
		return;
	}

	if (SandboxCharacter->GetSandboxPlayerView() != PlayerView::TOP_DOWN) {
		return;
	}

	if (SandboxCharacter->IsDead()) {
		return;
	}

	bMoveToMouseCursor = false; // Снятие флага перемещения к курсору мыши
}

// Метод для обработки нажатия основного действия
void ASandboxPlayerController::OnMainActionPressedInternal() {
	if (!IsGameInputBlocked()) {
		OnMainActionPressed(); // Вызов метода нажатия основного действия
	}
}

// Метод для обработки отпускания основного действия
void ASandboxPlayerController::OnMainActionReleasedInternal() {
	if (!IsGameInputBlocked()) {
		OnMainActionReleased(); // Вызов метода отпускания основного действия
	}
}

// Метод для обработки нажатия альтернативного действия
void ASandboxPlayerController::OnAltActionPressedInternal() {
	if (!IsGameInputBlocked()) {
		OnAltActionPressed(); // Вызов метода нажатия альтернативного действия
	}
}

// Метод для обработки отпускания альтернативного действия
void ASandboxPlayerController::OnAltActionReleasedInternal() {
	if (!IsGameInputBlocked()) {
		OnAltActionReleased(); // Вызов метода отпускания альтернативного действия
	}
}

// Метод для обработки нажатия основного действия
void ASandboxPlayerController::OnMainActionPressed() {

}

// Метод для обработки отпускания основного действия
void ASandboxPlayerController::OnMainActionReleased() {

}

// Метод для обработки нажатия альтернативного действия
void ASandboxPlayerController::OnAltActionPressed() {

}

// Метод для обработки отпускания альтернативного действия
void ASandboxPlayerController::OnAltActionReleased() {

}

// Метод для переключения вида
void ASandboxPlayerController::ToggleView() {
	if (IsGameInputBlocked()) {
		return;
	}

}

// Метод для обработки обладания новым персонажем
void ASandboxPlayerController::OnPossess(APawn* NewPawn) {
	Super::OnPossess(NewPawn); // Вызов родительского метода

	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(NewPawn); // Получение персонажа
	if (SandboxCharacter) {
		SandboxCharacter->EnableInput(this); // Включение ввода для персонажа
		if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
			//bShowMouseCursor = true; // Показать курсор мыши
		} else {
			//bShowMouseCursor = false; // Скрыть курсор мыши
		}
	}

	//bShowMouseCursor = false; // Скрыть курсор мыши
}

// Метод для блокировки ввода игры
void ASandboxPlayerController::BlockGameInput() {
	//UWidgetBlueprintLibrary::SetInputMode_GameAndUI(this, nullptr, false, false); // Установка режима ввода
	bIsGameInputBlocked = true; // Установка флага блокировки ввода
	//bShowMouseCursor = true; // Показать курсор мыши
}

// Метод для разблокировки ввода игры
void ASandboxPlayerController::UnblockGameInput() {
	//UWidgetBlueprintLibrary::SetInputMode_GameOnly(this); // Установка режима ввода
	bIsGameInputBlocked = false; // Снятие флага блокировки ввода
	//bShowMouseCursor = false; // Скрыть курсор мыши
}

// Метод для трассировки и выбора объекта действия
void ASandboxPlayerController::TraceAndSelectActionObject() {
	if (!IsGameInputBlocked()) {
		FHitResult Res = TracePlayerActionPoint(); // Трассировка точки действия игрока
		OnTracePlayerActionPoint(Res); // Обработка результата трассировки
		if (Res.bBlockingHit) {
			AActor* SelectedActor = Res.GetActor(); // Получение выбранного актора
			if (SelectedActor) {
				SelectActionObject(SelectedActor); // Выбор объекта действия
			}
		}
	}
}

// Метод для трассировки точки действия игрока
FHitResult ASandboxPlayerController::TracePlayerActionPoint() {
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter()); // Получение персонажа
	if (!SandboxCharacter) {
		return FHitResult(); // Возврат пустого результата
	}

	if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::THIRD_PERSON || SandboxCharacter->GetSandboxPlayerView() == PlayerView::FIRST_PERSON) {
		float MaxUseDistance = SandboxCharacter->InteractionTargetLength; // Максимальное расстояние взаимодействия

		if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::THIRD_PERSON) {
			if (SandboxCharacter->GetCameraBoom() != NULL) {
				//MaxUseDistance = Character->GetCameraBoom()->TargetArmLength + MaxUseDistance; // Установка максимального расстояния взаимодействия
			}
		}

		FVector CamLoc; // Позиция камеры
		FRotator CamRot; // Ротация камеры
		GetPlayerViewPoint(CamLoc, CamRot); // Получение точки зрения игрока

		const FVector StartTrace = CamLoc; // Начальная точка трассировки
		const FVector Direction = CamRot.Vector(); // Направление трассировки
		const FVector EndTrace = StartTrace + (Direction * MaxUseDistance); // Конечная точка трассировки

		FCollisionQueryParams TraceParams(FName(TEXT("")), true, this); // Параметры трассировки
		//TraceParams.bTraceAsyncScene = true; // Асинхронная трассировка сцены
		//TraceParams.bReturnPhysicalMaterial = false; // Возврат физического материала

		TraceParams.bTraceComplex = true; // Сложная трассировка
		TraceParams.bReturnFaceIndex = true; // Возврат индекса грани
		TraceParams.AddIgnoredActor(SandboxCharacter); // Игнорирование персонажа

		FHitResult Hit(ForceInit); // Результат попадания
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, TraceParams); // Трассировка луча

		return Hit; // Возврат результата попадания
	}

	if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
		FHitResult Hit; // Результат попадания
		GetHitResultUnderCursor(ECC_Camera, false, Hit); // Получение результата попадания под курсором
		return Hit; // Возврат результата попадания
	}

	return FHitResult(); // Возврат пустого результата
}

// Метод для установки рендеринга кастомной глубины
void SetRenderCustomDepth2(AActor* Actor, bool RenderCustomDepth) {
	TArray<UStaticMeshComponent*> MeshComponentList; // Список компонентов статической меши
	Actor->GetComponents<UStaticMeshComponent>(MeshComponentList); // Получение компонентов статической меши

	for (UStaticMeshComponent* MeshComponent : MeshComponentList) {
		MeshComponent->SetRenderCustomDepth(RenderCustomDepth); // Установка рендеринга кастомной глубины
	}
}

// Метод для обработки выбора объекта действия
void ASandboxPlayerController::OnSelectActionObject(AActor* Actor) {
	SetRenderCustomDepth2(Actor, true); // Установка рендеринга кастомной глубины
}

// Метод для обработки снятия выбора объекта действия
void ASandboxPlayerController::OnDeselectActionObject(AActor* Actor) {
	SetRenderCustomDepth2(Actor, false); // Снятие рендеринга кастомной глубины
}

// Метод для выбора объекта действия
void ASandboxPlayerController::SelectActionObject(AActor* Actor) {
	ASandboxObject* Obj = Cast<ASandboxObject>(Actor); // Получение объекта

	if (SelectedObject != Obj) {
		if (SelectedObject != nullptr && SelectedObject->IsValidLowLevel()) {
			OnDeselectActionObject(SelectedObject); // Снятие выбора предыдущего объекта
		}
	}

	if (Obj != nullptr) {
		OnSelectActionObject(Obj); // Выбор нового объекта
		SelectedObject = Obj; // Установка выбранного объекта
	} else {
		if (SelectedObject != nullptr && SelectedObject->IsValidLowLevel()) {
			OnDeselectActionObject(SelectedObject); // Снятие выбора предыдущего объекта
		}
	}
}

// Метод для получения контейнера по имени
UContainerComponent* ASandboxPlayerController::GetContainerByName(FName ContainerName) {
	APawn* PlayerPawn = GetPawn(); // Получение персонажа игрока
	if (PlayerPawn) {
		TArray<UContainerComponent*> Components; // Список компонентов контейнера
		PlayerPawn->GetComponents<UContainerComponent>(Components); // Получение компонентов контейнера

		for (UContainerComponent* Container : Components) {
			if (Container->GetName() == ContainerName.ToString()) {
				return Container; // Возврат найденного контейнера
			}
		}
	}

	return nullptr; // Возврат nullptr, если контейнер не найден
}

// Метод для получения инвентаря
UContainerComponent* ASandboxPlayerController::GetInventory() {
	return GetContainerByName(TEXT("Inventory")); // Получение контейнера по имени "Inventory"
}

// Метод для взятия объекта в инвентарь
bool ASandboxPlayerController::TakeObjectToInventory() {
	UContainerComponent* Inventory = GetInventory(); // Получение инвентаря

	if (Inventory != nullptr) {
		FHitResult ActionPoint = TracePlayerActionPoint(); // Трассировка точки действия игрока
		if (ActionPoint.bBlockingHit) {
			ASandboxObject* Obj = Cast<ASandboxObject>(ActionPoint.GetActor()); // Получение объекта
			if (Obj) {
				if (Obj->CanTake(nullptr)) {
					if (Inventory->AddObject(Obj)) {
						if (LevelController) {
							LevelController->RemoveSandboxObject(Obj); // Удаление объекта из уровня
						} else {
							Obj->Destroy(); // Уничтожение объекта
						}

						return true; // Возврат true, если объект успешно взят в инвентарь
					}
				}
			}
		}
	}

	return false; // Возврат false, если объект не взят в инвентарь
}

// Метод для открытия контейнера объекта
bool ASandboxPlayerController::OpenObjectContainer(ASandboxObject* Obj) {
	if (Obj != nullptr) {
		TArray<UContainerComponent*> Components; // Список компонентов контейнера
		Obj->GetComponents<UContainerComponent>(Components); // Получение компонентов контейнера
		for (UContainerComponent* Container : Components) {
			if (Container->GetName() == "ObjectContainer") {
				this->OpenedObject = Obj; // Установка открытого объекта
				this->OpenedContainer = Container; // Установка открытого контейнера
				return true; // Возврат true, если контейнер успешно открыт
			}
		}
	}

	return false; // Возврат false, если контейнер не открыт
}

// Метод для проверки наличия открытого контейнера
bool ASandboxPlayerController::HasOpenContainer() {
	return OpenedObject != nullptr; // Возврат true, если есть открытый контейнер
}

// Метод для трассировки и открытия контейнера объекта
bool ASandboxPlayerController::TraceAndOpenObjectContainer() {
	FHitResult ActionPoint = TracePlayerActionPoint(); // Трассировка точки действия игрока

	if (ActionPoint.bBlockingHit) {
		ASandboxObject* Obj = Cast<ASandboxObject>(ActionPoint.GetActor()); // Получение объекта
		return OpenObjectContainer(Obj); // Открытие контейнера объекта
	}

	return false; // Возврат false, если контейнер не открыт
}

// Метод для закрытия объекта с контейнером
void ASandboxPlayerController::CloseObjectWithContainer() {
	this->OpenedObject = nullptr; // Сброс открытого объекта
	this->OpenedContainer = nullptr; // Сброс открытого контейнера
}

// Метод для обработки трассировки точки действия игрока
void ASandboxPlayerController::OnTracePlayerActionPoint(const FHitResult& Res) {

}

// Метод для проверки блокировки ввода игры
bool ASandboxPlayerController::IsGameInputBlocked() {
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter()); // Получение персонажа
	if (SandboxCharacter && !SandboxCharacter->InputEnabled()) {
		return true; // Возврат true, если ввод заблокирован
	}

	return bIsGameInputBlocked; // Возврат флага блокировки ввода
}

// Метод для установки текущего слота инвентаря
void ASandboxPlayerController::SetCurrentInventorySlot(int32 Slot) {
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter()); // Получение персонажа
	if (SandboxCharacter && SandboxCharacter->IsDead()) {
		return;
	}

	CurrentInventorySlot = Slot; // Установка текущего слота инвентаря
}

// Метод для получения открытого контейнера
UContainerComponent* ASandboxPlayerController::GetOpenedContainer() {
	return this->OpenedContainer; // Возврат открытого контейнера
}

// Метод для получения открытого объекта
ASandboxObject* ASandboxPlayerController::GetOpenedObject() {
	return this->OpenedObject; // Возврат открытого объекта
}

// Метод для показа/скрытия курсора мыши
void  ASandboxPlayerController::ShowMouseCursor(bool bShowCursor) {
	this->bShowMouseCursor = bShowCursor; // Установка флага показа курсора мыши
};

// Метод для обработки основного действия с контейнером
void ASandboxPlayerController::OnContainerMainAction(int32 SlotId, FName ContainerName) {

}

// Метод для обработки успешного перемещения объекта между контейнерами
void ASandboxPlayerController::OnContainerDropSuccess(int32 SlotId, FName SourceName, FName TargetName) {

}

// Метод для проверки возможности перемещения объекта между контейнерами
bool ASandboxPlayerController::OnContainerDropCheck(int32 SlotId, FName ContainerName, const ASandboxObject* Obj) const {
	return true; // Возврат true, если перемещение возможно
}

// Метод для перемещения стека объектов в контейнер
void ASandboxPlayerController::TransferContainerStack_Implementation(const FString& ObjectNetUid, const FString& ContainerName, const FContainerStack& Stack, const int SlotId) {
	if (LevelController) {
		ASandboxObject* Obj = LevelController->GetObjectByNetUid(ObjectNetUid); // Получение объекта по NetUid
		if (Obj) {
			TArray<UContainerComponent*> Components; // Список компонентов контейнера
			Obj->GetComponents<UContainerComponent>(Components); // Получение компонентов контейнера
			for (UContainerComponent* Container : Components) {
				if (Container->GetName() == ContainerName) {
					Container->SetStackDirectly(Stack, SlotId); // Установка стека объектов в контейнер
				}
			}

			Obj->ForceNetUpdate(); // Принудительное обновление сети
		}
	}
}

// Метод для перемещения стека объектов в инвентарь
void ASandboxPlayerController::TransferInventoryStack_Implementation(const FString& ContainerName, const FContainerStack& Stack, const int SlotId) {
	ACharacter* PlayerCharacter = Cast<ACharacter>(GetCharacter()); // Получение персонажа игрока
	TArray<UContainerComponent*> Components; // Список компонентов контейнера
	PlayerCharacter->GetComponents<UContainerComponent>(Components); // Получение компонентов контейнера
	for (UContainerComponent* Container : Components) {
		if (Container->GetName() == ContainerName) {
			Container->SetStackDirectly(Stack, SlotId); // Установка стека объектов в инвентарь
		}
	}

	PlayerCharacter->ForceNetUpdate(); // Принудительное обновление сети
}

// Метод для получения контроллера уровня
ASandboxLevelController* ASandboxPlayerController::GetLevelController() {
	return LevelController; // Возврат контроллера уровня
}

// Метод для обработки наведения на слот контейнера
bool ASandboxPlayerController::OnContainerSlotHover(int32 SlotId, FName ContainerName) {
	return false; // Возврат false, если наведение не обработано
}