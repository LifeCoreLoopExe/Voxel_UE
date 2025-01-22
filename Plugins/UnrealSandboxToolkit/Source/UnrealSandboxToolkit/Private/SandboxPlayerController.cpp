
#include "SandboxPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "SandboxCharacter.h"
#include "SandboxObject.h"
#include "ContainerComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

// Конструктор класса ASandboxPlayerController
ASandboxPlayerController::ASandboxPlayerController() {
	// Показать курсор мыши (закомментировано)
	//bShowMouseCursor = true;
	// Установить курсор мыши по умолчанию
	DefaultMouseCursor = EMouseCursor::Default;
	// Инициализировать текущий слот инвентаря
	CurrentInventorySlot = -1;
	// Инициализировать флаг блокировки игрового ввода
	bIsGameInputBlocked = false;
}

// Метод, вызываемый при начале игры
void ASandboxPlayerController::BeginPlay() {
	// Вызов метода родительского класса
	Super::BeginPlay();

	// Инициализировать контроллер уровня
	LevelController = nullptr;
	// Итерация по всем акторам типа ASandboxLevelController в мире
	for (TActorIterator<ASandboxLevelController> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		// Приведение актора к типу ASandboxLevelController
		ASandboxLevelController* LevelCtrl = Cast<ASandboxLevelController>(*ActorItr);
		if (LevelCtrl) {
			// Логирование найденного контроллера уровня
			UE_LOG(LogTemp, Log, TEXT("Found ALevelController -> %s"), *LevelCtrl->GetName());
			// Установка найденного контроллера уровня
			LevelController = LevelCtrl;
			break;
		}
	}
}

// Метод, вызываемый каждый тик игрока
void ASandboxPlayerController::PlayerTick(float DeltaTime) {
	// Вызов метода родительского класса
	Super::PlayerTick(DeltaTime);

	// Если флаг перемещения к курсору мыши установлен
	if (bMoveToMouseCursor)	{
		// Перемещение к курсору мыши
		MoveToMouseCursor();
	}
}

// Метод для настройки компонента ввода
void ASandboxPlayerController::SetupInputComponent() {
	// Настройка привязок клавиш для игрового процесса
	Super::SetupInputComponent();

	// Привязка действия "MainAction" к методам OnMainActionPressedInternal и OnMainActionReleasedInternal
	InputComponent->BindAction("MainAction", IE_Pressed, this, &ASandboxPlayerController::OnMainActionPressedInternal);
	InputComponent->BindAction("MainAction", IE_Released, this, &ASandboxPlayerController::OnMainActionReleasedInternal);

	// Привязка действия "AltAction" к методам OnAltActionPressedInternal и OnAltActionReleasedInternal
	InputComponent->BindAction("AltAction", IE_Pressed, this, &ASandboxPlayerController::OnAltActionPressedInternal);
	InputComponent->BindAction("AltAction", IE_Released, this, &ASandboxPlayerController::OnAltActionReleasedInternal);

	// Поддержка сенсорных устройств (закомментировано)
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AUE4VoxelTerrainPlayerController::MoveToTouchLocation);
	//InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AUE4VoxelTerrainPlayerController::MoveToTouchLocation);

	// Привязка действия "ToggleView" к методу ToggleView
	InputComponent->BindAction("ToggleView", IE_Pressed, this, &ASandboxPlayerController::ToggleView);
}


// Метод для перемещения к курсору мыши
void ASandboxPlayerController::MoveToMouseCursor() {
	// Объект для хранения результата попадания
	FHitResult Hit;
	// Получение результата попадания под курсором
	GetHitResultUnderCursor(ECC_WorldStatic, false, Hit);

	// Если попадание произошло
	if (Hit.bBlockingHit) {
		// Перемещение к точке попадания
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

// Метод для перемещения к месту касания на экране
void ASandboxPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location) {
	// Преобразование местоположения касания в экранные координаты
	FVector2D ScreenSpaceLocation(Location);

	// Объект для хранения результата попадания
	FHitResult HitResult;
	// Получение результата попадания в экранной позиции
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	// Если попадание произошло
	if (HitResult.bBlockingHit) {
		// Перемещение к точке попадания
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

// Метод для установки новой цели перемещения
void ASandboxPlayerController::SetNewMoveDestination(const FVector DestLocation) {
	// Приведение текущего персонажа к типу ASandboxCharacter
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter());

	// Если персонаж существует
	if (SandboxCharacter) {
		// Вычисление расстояния до новой цели
		float const Distance = FVector::Dist(DestLocation, SandboxCharacter->GetActorLocation());
		// Если расстояние больше 120.0f
		if (Distance > 120.0f) {
			// Перемещение к новой цели
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

// Метод для обработки нажатия на установку цели
void ASandboxPlayerController::SetDestinationPressed() {
	// Приведение текущего персонажа к типу ASandboxCharacter
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter());
	// Если персонаж не существует, выход из метода
	if (!SandboxCharacter) {
		return;
	}

	// Если вид персонажа не TOP_DOWN, выход из метода
	if (SandboxCharacter->GetSandboxPlayerView() != PlayerView::TOP_DOWN) {
		return;
	}

	// Если персонаж мертв, выход из метода
	if (SandboxCharacter->IsDead()) {
		return;
	}

	// Установка флага перемещения к курсору мыши
	bMoveToMouseCursor = true;
}

// Метод для обработки отпускания на установку цели
void ASandboxPlayerController::SetDestinationReleased() {
	// Приведение текущего персонажа к типу ASandboxCharacter
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter());
	// Если персонаж не существует, выход из метода
	if (!SandboxCharacter) {
		return;
	}

	// Если вид персонажа не TOP_DOWN, выход из метода
	if (SandboxCharacter->GetSandboxPlayerView() != PlayerView::TOP_DOWN) {
		return;
	}

	// Если персонаж мертв, выход из метода
	if (SandboxCharacter->IsDead()) {
		return;
	}

	// Сброс флага перемещения к курсору мыши
	bMoveToMouseCursor = false;
}

// Метод для обработки нажатия основного действия
void ASandboxPlayerController::OnMainActionPressedInternal() {
	// Если ввод не заблокирован
	if (!IsGameInputBlocked()) {
		// Вызов метода OnMainActionPressed
		OnMainActionPressed();
	}
}


// Метод для обработки отпускания основного действия
void ASandboxPlayerController::OnMainActionReleasedInternal() {
	// Если ввод не заблокирован
	if (!IsGameInputBlocked()) {
		// Вызов метода OnMainActionReleased
		OnMainActionReleased();
	}
}

// Метод для обработки нажатия альтернативного действия
void ASandboxPlayerController::OnAltActionPressedInternal() {
	// Если ввод не заблокирован
	if (!IsGameInputBlocked()) {
		// Вызов метода OnAltActionPressed
		OnAltActionPressed();
	}
}

// Метод для обработки отпускания альтернативного действия
void ASandboxPlayerController::OnAltActionReleasedInternal() {
	// Если ввод не заблокирован
	if (!IsGameInputBlocked()) {
		// Вызов метода OnAltActionReleased
		OnAltActionReleased();
	}
}

// Метод для обработки нажатия основного действия (пустой метод, предназначенный для переопределения)
void ASandboxPlayerController::OnMainActionPressed() {

}

// Метод для обработки отпускания основного действия (пустой метод, предназначенный для переопределения)
void ASandboxPlayerController::OnMainActionReleased() {

}

// Метод для обработки нажатия альтернативного действия (пустой метод, предназначенный для переопределения)
void ASandboxPlayerController::OnAltActionPressed() {

}

// Метод для обработки отпускания альтернативного действия (пустой метод, предназначенный для переопределения)
void ASandboxPlayerController::OnAltActionReleased() {

}

// Метод для переключения вида
void ASandboxPlayerController::ToggleView() {
	// Если ввод заблокирован, выход из метода
	if (IsGameInputBlocked()) {
		return;
	}

	// Логика переключения вида (пустой метод, предназначенный для переопределения)
}

// Метод для обработки обладания новым персонажем
void ASandboxPlayerController::OnPossess(APawn* NewPawn) {
	// Вызов метода родительского класса
	Super::OnPossess(NewPawn);

	// Приведение нового персонажа к типу ASandboxCharacter
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(NewPawn);
	// Если персонаж существует
	if (SandboxCharacter) {
		// Включение ввода для персонажа
		SandboxCharacter->EnableInput(this);
		// Если вид персонажа TOP_DOWN
		if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
			// Показать курсор мыши (закомментировано)
			//bShowMouseCursor = true;
		} else {
			// Скрыть курсор мыши (закомментировано)
			//bShowMouseCursor = false;
		}
	}

	// Скрыть курсор мыши (закомментировано)
	//bShowMouseCursor = false;
}

// Метод для блокировки игрового ввода
void ASandboxPlayerController::BlockGameInput() {
	// Установка режима ввода игры и интерфейса (закомментировано)
	//UWidgetBlueprintLibrary::SetInputMode_GameAndUI(this, nullptr, false, false);
	// Установка флага блокировки игрового ввода
	bIsGameInputBlocked = true;
	// Показать курсор мыши (закомментировано)
	//bShowMouseCursor = true;
}

// Метод для разблокировки игрового ввода
void ASandboxPlayerController::UnblockGameInput() {
	// Установка режима ввода только игры (закомментировано)
	//UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
	// Сброс флага блокировки игрового ввода
	bIsGameInputBlocked = false;
	// Скрыть курсор мыши (закомментировано)
	//bShowMouseCursor = false;
}

// Метод для трассировки и выбора объекта действия
void ASandboxPlayerController::TraceAndSelectActionObject() {
	// Если ввод не заблокирован
	if (!IsGameInputBlocked()) {
		// Трассировка точки действия игрока
		FHitResult Res = TracePlayerActionPoint();
		// Вызов метода OnTracePlayerActionPoint с результатом трассировки
		OnTracePlayerActionPoint(Res);
		// Если попадание произошло
		if (Res.bBlockingHit) {
			// Получение выбранного актора
			AActor* SelectedActor = Res.GetActor();
			// Если актор существует
			if (SelectedActor) {
				// Выбор объекта действия
				SelectActionObject(SelectedActor);
			}
		}
	}
}


// Метод для трассировки точки действия игрока
FHitResult ASandboxPlayerController::TracePlayerActionPoint() {
	// Приведение текущего персонажа к типу ASandboxCharacter
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter());
	// Если персонаж не существует, возвращаем пустой результат трассировки
	if (!SandboxCharacter) {
		return FHitResult();
	}

	// Если вид персонажа TOP_DOWN, THIRD_PERSON или FIRST_PERSON
	if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::THIRD_PERSON || SandboxCharacter->GetSandboxPlayerView() == PlayerView::FIRST_PERSON) {
		// Максимальное расстояние для взаимодействия
		float MaxUseDistance = SandboxCharacter->InteractionTargetLength;

		// Если вид персонажа THIRD_PERSON
		if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::THIRD_PERSON) {
			// Если камера существует
			if (SandboxCharacter->GetCameraBoom() != NULL) {
				// Увеличение максимального расстояния для взаимодействия (закомментировано)
				//MaxUseDistance = Character->GetCameraBoom()->TargetArmLength + MaxUseDistance;
			}
		}

		// Получение позиции и ротации камеры
		FVector CamLoc;
		FRotator CamRot;
		GetPlayerViewPoint(CamLoc, CamRot);

		// Начальная точка трассировки
		const FVector StartTrace = CamLoc;
		// Направление трассировки
		const FVector Direction = CamRot.Vector();
		// Конечная точка трассировки
		const FVector EndTrace = StartTrace + (Direction * MaxUseDistance);

		// Параметры трассировки
		FCollisionQueryParams TraceParams(FName(TEXT("")), true, this);
		//TraceParams.bTraceAsyncScene = true;
		//TraceParams.bReturnPhysicalMaterial = false;

		// Включение трассировки сложных объектов
		TraceParams.bTraceComplex = true;
		// Включение возврата индекса грани
		TraceParams.bReturnFaceIndex = true;
		// Добавление игнорируемого актора
		TraceParams.AddIgnoredActor(SandboxCharacter);

		// Результат трассировки
		FHitResult Hit(ForceInit);
		// Выполнение трассировки
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, TraceParams);

		// Возвращение результата трассировки
		return Hit;
	}

	// Если вид персонажа TOP_DOWN
	if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
		// Результат трассировки
		FHitResult Hit;
		// Получение результата трассировки под курсором
		GetHitResultUnderCursor(ECC_Camera, false, Hit);
		// Возвращение результата трассировки
		return Hit;
	}

	// Возвращение пустого результата трассировки
	return FHitResult();
}

// Функция для установки пользовательской глубины рендеринга для актора
void SetRenderCustomDepth2(AActor* Actor, bool RenderCustomDepth) {
	// Список компонентов статической меши
	TArray<UStaticMeshComponent*> MeshComponentList;
	// Получение всех компонентов статической меши актора
	Actor->GetComponents<UStaticMeshComponent>(MeshComponentList);

	// Установка пользовательской глубины рендеринга для каждого компонента
	for (UStaticMeshComponent* MeshComponent : MeshComponentList) {
		MeshComponent->SetRenderCustomDepth(RenderCustomDepth);
	}
}

// Метод для обработки выбора объекта действия
void ASandboxPlayerController::OnSelectActionObject(AActor* Actor) {
	// Установка пользовательской глубины рендеринга для объекта
	SetRenderCustomDepth2(Actor, true);
}

// Метод для обработки отмены выбора объекта действия
void ASandboxPlayerController::OnDeselectActionObject(AActor* Actor) {
	// Сброс пользовательской глубины рендеринга для объекта
	SetRenderCustomDepth2(Actor, false);
}

// Метод для выбора объекта действия
void ASandboxPlayerController::SelectActionObject(AActor* Actor) {
	// Приведение объекта к типу ASandboxObject
	ASandboxObject* Obj = Cast<ASandboxObject>(Actor);

	// Если выбранный объект изменился
	if (SelectedObject != Obj) {
		// Если предыдущий выбранный объект существует и валиден
		if (SelectedObject != nullptr && SelectedObject->IsValidLowLevel()) {
			// Отмена выбора предыдущего объекта
			OnDeselectActionObject(SelectedObject);
		}
	}

	// Если новый объект существует
	if (Obj != nullptr) {
		// Выбор нового объекта
		OnSelectActionObject(Obj);
		// Установка нового выбранного объекта
		SelectedObject = Obj;
	} else {
		// Если предыдущий выбранный объект существует и валиден
		if (SelectedObject != nullptr && SelectedObject->IsValidLowLevel()) {
			// Отмена выбора предыдущего объекта
			OnDeselectActionObject(SelectedObject);
		}
	}
}

// Метод для получения контейнера по имени
UContainerComponent* ASandboxPlayerController::GetContainerByName(FName ContainerName) {
	// Получение персонажа игрока
	APawn* PlayerPawn = GetPawn();
	// Если персонаж существует
	if (PlayerPawn) {
		// Список компонентов контейнера
		TArray<UContainerComponent*> Components;
		// Получение всех компонентов контейнера персонажа
		PlayerPawn->GetComponents<UContainerComponent>(Components);

		// Поиск контейнера по имени
		for (UContainerComponent* Container : Components) {
			if (Container->GetName() == ContainerName.ToString()) {
				return Container;
			}
		}
	}

	// Возвращение nullptr, если контейнер не найден
	return nullptr;
}

// Метод для получения инвентаря
UContainerComponent* ASandboxPlayerController::GetInventory() {
	// Получение контейнера по имени "Inventory"
	return GetContainerByName(TEXT("Inventory"));
}

// Метод для взятия объекта в инвентарь
bool ASandboxPlayerController::TakeObjectToInventory() {
	// Получение инвентаря
	UContainerComponent* Inventory = GetInventory();

	// Если инвентарь существует
	if (Inventory != nullptr) {
		// Трассировка точки действия игрока
		FHitResult ActionPoint = TracePlayerActionPoint();
		// Если попадание произошло
		if (ActionPoint.bBlockingHit) {
			// Приведение объекта к типу ASandboxObject
			ASandboxObject* Obj = Cast<ASandboxObject>(ActionPoint.GetActor());
			// Если объект существует
			if (Obj) {
				// Если объект можно взять
				if (Obj->CanTake(nullptr)) {
					// Если объект добавлен в инвентарь
					if (Inventory->AddObject(Obj)) {
						// Если контроллер уровня существует
						if (LevelController) {
							// Удаление объекта из уровня
							LevelController->RemoveSandboxObject(Obj);
						} else {
							// Уничтожение объекта
							Obj->Destroy();
						}

						// Возвращение true, если объект успешно взят в инвентарь
						return true;
					}
				}
			}
		}
	}

	// Возвращение false, если объект не взят в инвентарь
	return false;
}

// Метод для открытия контейнера объекта
bool ASandboxPlayerController::OpenObjectContainer(ASandboxObject* Obj) {
	// Если объект существует
	if (Obj != nullptr) {
		// Список компонентов контейнера
		TArray<UContainerComponent*> Components;
		// Получение всех компонентов контейнера объекта
		Obj->GetComponents<UContainerComponent>(Components);
		// Поиск контейнера по имени "ObjectContainer"
		for (UContainerComponent* Container : Components) {
			if (Container->GetName() == "ObjectContainer") {
				// Установка открытого объекта и контейнера
				this->OpenedObject = Obj;
				this->OpenedContainer = Container;
				// Возвращение true, если контейнер успешно открыт
				return true;
			}
		}
	}

	// Возвращение false, если контейнер не открыт
	return false;
}

// Метод для проверки наличия открытого контейнера
bool ASandboxPlayerController::HasOpenContainer() {
	// Возвращение true, если открытый объект существует
	return OpenedObject != nullptr;
}

// Метод для трассировки и открытия контейнера объекта
bool ASandboxPlayerController::TraceAndOpenObjectContainer() {
	// Трассировка точки действия игрока
	FHitResult ActionPoint = TracePlayerActionPoint();

	// Если попадание произошло
	if (ActionPoint.bBlockingHit) {
		// Приведение объекта к типу ASandboxObject
		ASandboxObject* Obj = Cast<ASandboxObject>(ActionPoint.GetActor());
		// Открытие контейнера объекта
		return OpenObjectContainer(Obj);
	}

	// Возвращение false, если контейнер не открыт
	return false;
}

// Метод для закрытия объекта с контейнером
void ASandboxPlayerController::CloseObjectWithContainer() {
	// Сброс открытого объекта и контейнера
	this->OpenedObject = nullptr;
	this->OpenedContainer = nullptr;
}

// Метод для обработки трассировки точки действия игрока
void ASandboxPlayerController::OnTracePlayerActionPoint(const FHitResult& Res) {
	// Пустой метод, предназначенный для переопределения
}

// Метод для проверки, заблокирован ли игровой ввод
bool ASandboxPlayerController::IsGameInputBlocked() {
	// Приведение текущего персонажа к типу ASandboxCharacter
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter());
	// Если персонаж существует и его ввод отключен
	if (SandboxCharacter && !SandboxCharacter->InputEnabled()) {
		// Возвращение true, если ввод заблокирован
		return true;
	}

	// Возвращение значения флага блокировки игрового ввода
	return bIsGameInputBlocked;
}

// Метод для установки текущего слота инвентаря
void ASandboxPlayerController::SetCurrentInventorySlot(int32 Slot) {
	// Приведение текущего персонажа к типу ASandboxCharacter
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter());
	// Если персонаж существует и мертв, выход из метода
	if (SandboxCharacter && SandboxCharacter->IsDead()) {
		return;
	}

	// Установка текущего слота инвентаря
	CurrentInventorySlot = Slot;
}

// Метод для получения открытого контейнера
UContainerComponent* ASandboxPlayerController::GetOpenedContainer() {
	// Возвращение открытого контейнера
	return this->OpenedContainer;
}

// Метод для получения открытого объекта
ASandboxObject* ASandboxPlayerController::GetOpenedObject() {
	// Возвращение открытого объекта
	return this->OpenedObject;
}

// Метод для показа или скрытия курсора мыши
void ASandboxPlayerController::ShowMouseCursor(bool bShowCursor) {
	// Установка флага показа курсора мыши
	this->bShowMouseCursor = bShowCursor;
}

// Метод для обработки основного действия контейнера
void ASandboxPlayerController::OnContainerMainAction(int32 SlotId, FName ContainerName) {
	// Пустой метод, предназначенный для переопределения
}

// Метод для обработки успешного перемещения объекта между контейнерами
void ASandboxPlayerController::OnContainerDropSuccess(int32 SlotId, FName SourceName, FName TargetName) {
	// Пустой метод, предназначенный для переопределения
}

// Метод для проверки возможности перемещения объекта между контейнерами
bool ASandboxPlayerController::OnContainerDropCheck(int32 SlotId, FName ContainerName, const ASandboxObject* Obj) const {
	// Возвращение true, если перемещение возможно
	return true;
}

// Метод для перемещения стека объектов в контейнер
void ASandboxPlayerController::TransferContainerStack_Implementation(const FString& ObjectNetUid, const FString& ContainerName, const FContainerStack& Stack, const int SlotId) {
	// Если контроллер уровня существует
	if (LevelController) {
		// Получение объекта по сетевому идентификатору
		ASandboxObject* Obj = LevelController->GetObjectByNetUid(ObjectNetUid);
		// Если объект существует
		if (Obj) {
			// Список компонентов контейнера
			TArray<UContainerComponent*> Components;
			// Получение всех компонентов контейнера объекта
			Obj->GetComponents<UContainerComponent>(Components);
			// Поиск контейнера по имени
			for (UContainerComponent* Container : Components) {
				if (Container->GetName() == ContainerName) {
					// Установка стека объектов в контейнер
					Container->SetStackDirectly(Stack, SlotId);
				}
			}

			// Принудительное обновление объекта по сети
			Obj->ForceNetUpdate();
		}
	}
}

// Метод для перемещения стека объектов в инвентарь
void ASandboxPlayerController::TransferInventoryStack_Implementation(const FString& ContainerName, const FContainerStack& Stack, const int SlotId) {
	// Приведение текущего персонажа к типу ACharacter
	ACharacter* PlayerCharacter = Cast<ACharacter>(GetCharacter());
	// Список компонентов контейнера
	TArray<UContainerComponent*> Components;
	// Получение всех компонентов контейнера персонажа
	PlayerCharacter->GetComponents<UContainerComponent>(Components);
	// Поиск контейнера по имени
	for (UContainerComponent* Container : Components) {
		if (Container->GetName() == ContainerName) {
			// Установка стека объектов в контейнер
			Container->SetStackDirectly(Stack, SlotId);
		}
	}

	// Принудительное обновление персонажа по сети
	PlayerCharacter->ForceNetUpdate();
}

// Метод для получения контроллера уровня
ASandboxLevelController* ASandboxPlayerController::GetLevelController() {
	// Возвращение контроллера уровня
	return LevelController;
}

// Метод для обработки наведения на слот контейнера
bool ASandboxPlayerController::OnContainerSlotHover(int32 SlotId, FName ContainerName) {
	// Возвращение false, если наведение не обработано
	return false;
}
