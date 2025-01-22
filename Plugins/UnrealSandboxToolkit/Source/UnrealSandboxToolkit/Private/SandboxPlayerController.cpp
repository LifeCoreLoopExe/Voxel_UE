#include "SandboxPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "SandboxCharacter.h"
#include "SandboxObject.h"
#include "ContainerComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

ASandboxPlayerController::ASandboxPlayerController() {
	// Конструктор класса ASandboxPlayerController
	//bShowMouseCursor = true; // Показать курсор мыши
	DefaultMouseCursor = EMouseCursor::Default; // Установить курсор по умолчанию
	CurrentInventorySlot = -1; // Текущий слот инвентаря
	bIsGameInputBlocked = false; // Блокировка игрового ввода
}

void ASandboxPlayerController::BeginPlay() {
	Super::BeginPlay(); // Вызов родительского метода BeginPlay

	LevelController = nullptr; // Инициализация контроллера уровня
	for (TActorIterator<ASandboxLevelController> ActorItr(GetWorld()); ActorItr; ++ActorItr) { // Итерация по всем актерам ASandboxLevelController в мире
		ASandboxLevelController* LevelCtrl = Cast<ASandboxLevelController>(*ActorItr); // Приведение типа
		if (LevelCtrl) { // Если контроллер уровня найден
			UE_LOG(LogTemp, Log, TEXT("Found ALevelController -> %s"), *LevelCtrl->GetName()); // Логирование имени контроллера
			LevelController = LevelCtrl; // Сохранение ссылки на контроллер уровня
			break; // Выход из цикла
		}
	}
}

void ASandboxPlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime); // Вызов родительского метода PlayerTick

	if (bMoveToMouseCursor) { // Если нужно двигаться к курсору мыши
		MoveToMouseCursor(); // Вызов функции для движения к курсору
	}
}

void ASandboxPlayerController::SetupInputComponent() {
	// Настройка привязок клавиш для игрового процесса
	Super::SetupInputComponent(); // Вызов родительского метода SetupInputComponent

	InputComponent->BindAction("MainAction", IE_Pressed, this, &ASandboxPlayerController::OnMainActionPressedInternal); // Привязка основной действия на нажатие
	InputComponent->BindAction("MainAction", IE_Released, this, &ASandboxPlayerController::OnMainActionReleasedInternal); // Привязка основной действия на отпускание

	InputComponent->BindAction("AltAction", IE_Pressed, this, &ASandboxPlayerController::OnAltActionPressedInternal); // Привязка альтернативного действия на нажатие
	InputComponent->BindAction("AltAction", IE_Released, this, &ASandboxPlayerController::OnAltActionReleasedInternal); // Привязка альтернативного действия на отпускание

	// Поддержка сенсорных устройств 
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AUE4VoxelTerrainPlayerController::MoveToTouchLocation);
	//InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AUE4VoxelTerrainPlayerController::MoveToTouchLocation);

	InputComponent->BindAction("ToggleView", IE_Pressed, this, &ASandboxPlayerController::ToggleView); // Привязка действия переключения вида
}

void ASandboxPlayerController::MoveToMouseCursor() {
	FHitResult Hit; // Результат попадания
	GetHitResultUnderCursor(ECC_WorldStatic, false, Hit); // Получаем результат попадания под курсором

	if (Hit.bBlockingHit) { // Если произошло столкновение
		// Мы попали во что-то, двигаемся туда
		SetNewMoveDestination(Hit.ImpactPoint); // Устанавливаем новую цель движения
	}
}

void ASandboxPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location) {
	FVector2D ScreenSpaceLocation(Location); // Переводим координаты в экранное пространство

	FHitResult HitResult; // Результат попадания
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult); // Получаем результат попадания по экранным координатам
	if (HitResult.bBlockingHit) { // Если произошло столкновение
		SetNewMoveDestination(HitResult.ImpactPoint); // Устанавливаем новую цель движения
	}
}

void ASandboxPlayerController::SetNewMoveDestination(const FVector DestLocation) {
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter()); // Получаем персонажа

	if (SandboxCharacter) { // Если персонаж валиден
		float const Distance = FVector::Dist(DestLocation, SandboxCharacter->GetActorLocation()); // Вычисляем расстояние до цели
		if (Distance > 120.0f) { // Если расстояние больше 120
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation); // Двигаем персонажа к новой цели
		}
	}
}

void ASandboxPlayerController::SetDestinationPressed() {
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter()); // Получаем персонажа
	if (!SandboxCharacter) { // Если персонаж не найден
		return; // Выходим
	}

	if (SandboxCharacter->GetSandboxPlayerView() != PlayerView::TOP_DOWN) { // Если вид не сверху
		return; // Выходим
	}

	if (SandboxCharacter->IsDead()) { // Если персонаж мертв
		return; // Выходим
	}

	bMoveToMouseCursor = true; // Разрешаем движение к курсору
}

void ASandboxPlayerController::SetDestinationReleased() {
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter()); // Получаем персонажа
	if (!SandboxCharacter) { // Если персонаж не найден
		return; // Выходим
	}

	if (SandboxCharacter->GetSandboxPlayerView() != PlayerView::TOP_DOWN) { // Если вид не сверху
		return; // Выходим
	}

	if (SandboxCharacter->IsDead()) { // Если персонаж мертв
		return; // Выходим
	}

	bMoveToMouseCursor = false; // Запрещаем движение к курсору
}

void ASandboxPlayerController::OnMainActionPressedInternal() {
	if (!IsGameInputBlocked()) { // Если игровой ввод не заблокирован
		OnMainActionPressed(); // Вызов основной действия
	}
}

void ASandboxPlayerController::OnMainActionReleasedInternal() {
	if (!IsGameInputBlocked()) { // Если игровой ввод не заблокирован
		OnMainActionReleased(); // Вызов основной действия на отпускание
	}
}

void ASandboxPlayerController::OnAltActionPressedInternal() {
	if (!IsGameInputBlocked()) { // Если игровой ввод не заблокирован
		OnAltActionPressed(); // Вызов альтернативного действия
	}
}

void ASandboxPlayerController::OnAltActionReleasedInternal() {
	if (!IsGameInputBlocked()) { // Если игровой ввод не заблокирован
		OnAltActionReleased(); // Вызов альтернативного действия на отпускание
	}
}

void ASandboxPlayerController::OnMainActionPressed() {
	// Основное действие на нажатие
}

void ASandboxPlayerController::OnMainActionReleased() {
	// Основное действие на отпускание
}

void ASandboxPlayerController::OnAltActionPressed() {
	// Альтернативное действие на нажатие
}

void ASandboxPlayerController::OnAltActionReleased() {
	// Альтернативное действие на отпускание
}

void ASandboxPlayerController::ToggleView() {
	if (IsGameInputBlocked()) { // Если игровой ввод заблокирован
		return; // Выходим
	}

	// Код для переключения вида
}

void ASandboxPlayerController::OnPossess(APawn* NewPawn) {
	Super::OnPossess(NewPawn); // Вызов родительского метода OnPossess

	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(NewPawn); // Получаем нового персонажа
	if (SandboxCharacter) { // Если персонаж валиден
		SandboxCharacter->EnableInput(this); // Включаем ввод для персонажа
		if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::TOP_DOWN) { // Если вид сверху
			//bShowMouseCursor = true; // Показать курсор
		} else {
			//bShowMouseCursor = false; // Скрыть курсор
		}
	}

	//bShowMouseCursor = false; // Скрыть курсор
}

void ASandboxPlayerController::BlockGameInput() {
	//UWidgetBlueprintLibrary::SetInputMode_GameAndUI(this, nullptr, false, false); // Блокировка ввода
	bIsGameInputBlocked = true; // Устанавливаем флаг блокировки ввода
	//bShowMouseCursor = true; // Показать курсор
}

void ASandboxPlayerController::UnblockGameInput() {
	//UWidgetBlueprintLibrary::SetInputMode_GameOnly(this); // Разблокировка ввода
	bIsGameInputBlocked = false; // Сбрасываем флаг блокировки ввода
	//bShowMouseCursor = false; // Скрыть курсор
}

void ASandboxPlayerController::TraceAndSelectActionObject() {
	if (!IsGameInputBlocked()) { // Если игровой ввод не заблокирован
		FHitResult Res = TracePlayerActionPoint(); // Получаем результат трассировки
		OnTracePlayerActionPoint(Res); // Обрабатываем результат трассировки
		if (Res.bBlockingHit) { // Если произошло столкновение
			AActor* SelectedActor = Res.GetActor(); // Получаем актера, с которым произошло столкновение
			if (SelectedActor) { // Если актер валиден
				SelectActionObject(SelectedActor); // Выбор объекта действия
			}
		}
	}
}

FHitResult ASandboxPlayerController::TracePlayerActionPoint() {
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter()); // Получаем персонажа
	if (!SandboxCharacter) { // Если персонаж не найден
		return FHitResult(); // Возвращаем пустой результат
	}

	if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::THIRD_PERSON || SandboxCharacter->GetSandboxPlayerView() == PlayerView::FIRST_PERSON) { // Если вид третий или первый
		float MaxUseDistance = SandboxCharacter->InteractionTargetLength; // Максимальная дистанция взаимодействия

		if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::THIRD_PERSON) { // Если вид третий
			if (SandboxCharacter->GetCameraBoom() != NULL) { // Если камера существует
				//MaxUseDistance = Character->GetCameraBoom()->TargetArmLength + MaxUseDistance; // Корректировка максимальной дистанции
			}
		}

		FVector CamLoc; // Локация камеры
		FRotator CamRot; // Поворот камеры
		GetPlayerViewPoint(CamLoc, CamRot); // Получаем точку зрения игрока

		const FVector StartTrace = CamLoc; // Начальная точка трассировки
		const FVector Direction = CamRot.Vector(); // Направление трассировки
		const FVector EndTrace = StartTrace + (Direction * MaxUseDistance); // Конечная точка трассировки

		FCollisionQueryParams TraceParams(FName(TEXT("")), true, this); // Параметры трассировки
		//TraceParams.bTraceAsyncScene = true; // Асинхронная трассировка
		//TraceParams.bReturnPhysicalMaterial = false; // Не возвращать физический материал

		TraceParams.bTraceComplex = true; // Сложная трассировка
		TraceParams.bReturnFaceIndex = true; // Возвращать индекс поверхности
		TraceParams.AddIgnoredActor(SandboxCharacter); // Игнорировать самого персонажа

		FHitResult Hit(ForceInit); // Инициализация результата трассировки
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, TraceParams); // Выполнение трассировки

		return Hit; // Возвращаем результат трассировки
	}

	if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::TOP_DOWN) { // Если вид сверху
		FHitResult Hit; // Результат попадания
		GetHitResultUnderCursor(ECC_Camera, false, Hit); // Получаем результат попадания под курсором
		return Hit; // Возвращаем результат
	}

	return FHitResult(); // Возвращаем пустой результат
}

void SetRenderCustomDepth2(AActor* Actor, bool RenderCustomDepth) {
	TArray<UStaticMeshComponent*> MeshComponentList; // Список компонентов статической сетки
	Actor->GetComponents<UStaticMeshComponent>(MeshComponentList); // Получаем все компоненты статической сетки

	for (UStaticMeshComponent* MeshComponent : MeshComponentList) { // Итерация по компонентам
		MeshComponent->SetRenderCustomDepth(RenderCustomDepth); // Установка рендеринга с пользовательской глубиной
	}
}

void ASandboxPlayerController::OnSelectActionObject(AActor* Actor) {
	SetRenderCustomDepth2(Actor, true); // Устанавливаем рендеринг с пользовательской глубиной для выбранного объекта
}

void ASandboxPlayerController::OnDeselectActionObject(AActor* Actor) {
	SetRenderCustomDepth2(Actor, false); // Устанавливаем рендеринг без пользовательской глубины для отмененного выбора
}

void ASandboxPlayerController::SelectActionObject(AActor* Actor) {
	ASandboxObject* Obj = Cast<ASandboxObject>(Actor); // Приведение типа к ASandboxObject

	if (SelectedObject != Obj) { // Если выбранный объект не равен текущему
		if (SelectedObject != nullptr && SelectedObject->IsValidLowLevel()) { // Если предыдущий объект валиден
			OnDeselectActionObject(SelectedObject); // Отменяем выбор предыдущего объекта
		}
	}

	if (Obj != nullptr) { // Если объект валиден
		OnSelectActionObject(Obj); // Выбираем новый объект
		SelectedObject = Obj; // Устанавливаем выбранный объект
	} else {
		if (SelectedObject != nullptr && SelectedObject->IsValidLowLevel()) { // Если ранее выбранный объект валиден
			OnDeselectActionObject(SelectedObject); // Отменяем выбор
		}
	}
}

UContainerComponent* ASandboxPlayerController::GetContainerByName(FName ContainerName) {
	APawn* PlayerPawn = GetPawn(); // Получаем пешку игрока
	if (PlayerPawn) { // Если пешка валидна
		TArray<UContainerComponent*> Components; // Список компонентов контейнера
		PlayerPawn->GetComponents<UContainerComponent>(Components); // Получаем все компоненты контейнера

		for (UContainerComponent* Container : Components) { // Итерация по компонентам
			if (Container->GetName() == ContainerName.ToString()) { // Если имя контейнера совпадает
				return Container; // Возвращаем контейнер
			}
		}
	}

	return nullptr; // Возвращаем nullptr, если контейнер не найден
}

UContainerComponent* ASandboxPlayerController::GetInventory() {
	return GetContainerByName(TEXT("Inventory")); // Получаем контейнер инвентаря
}

bool ASandboxPlayerController::TakeObjectToInventory() {
	UContainerComponent* Inventory = GetInventory(); // Получаем инвентарь

	if (Inventory != nullptr) { // Если инвентарь валиден
		FHitResult ActionPoint = TracePlayerActionPoint(); // Получаем точку действия
		if (ActionPoint.bBlockingHit) { // Если произошло столкновение
			ASandboxObject* Obj = Cast<ASandboxObject>(ActionPoint.GetActor()); // Приведение типа к ASandboxObject
			if (Obj) { // Если объект валиден
				if (Obj->CanTake(nullptr)) { // Если объект можно взять
					if (Inventory->AddObject(Obj)) { // Добавляем объект в инвентарь
						if (LevelController) { // Если контроллер уровня валиден
							LevelController->RemoveSandboxObject(Obj); // Удаляем объект из уровня
						} else {
							Obj->Destroy(); // Уничтожаем объект
						}

						return true; // Возвращаем успех
					}
				}
			}
		}
	}

	return false; // Возвращаем неудачу
}

bool ASandboxPlayerController::OpenObjectContainer(ASandboxObject* Obj) {
	if (Obj != nullptr) { // Если объект валиден
		TArray<UContainerComponent*> Components; // Список компонентов контейнера
		Obj->GetComponents<UContainerComponent>(Components); // Получаем компоненты контейнера
		for (UContainerComponent* Container : Components) { // Итерация по компонентам
			if (Container->GetName() == "ObjectContainer") { // Если имя контейнера совпадает
				this->OpenedObject = Obj; // Устанавливаем открытый объект
				this->OpenedContainer = Container; // Устанавливаем открытый контейнер
				return true; // Возвращаем успех
			}
		}
	}

	return false; // Возвращаем неудачу
}

bool ASandboxPlayerController::HasOpenContainer() { 
	return OpenedObject != nullptr; // Проверка, открыт ли контейнер
}

bool ASandboxPlayerController::TraceAndOpenObjectContainer() {
	FHitResult ActionPoint = TracePlayerActionPoint(); // Получаем точку действия

	if (ActionPoint.bBlockingHit) { // Если произошло столкновение
		ASandboxObject* Obj = Cast<ASandboxObject>(ActionPoint.GetActor()); // Приведение типа к ASandboxObject
		return OpenObjectContainer(Obj); // Открываем контейнер объекта
	}

	return false; // Возвращаем неудачу
}

void ASandboxPlayerController::CloseObjectWithContainer() {
	this->OpenedObject = nullptr; // Закрываем открытый объект
	this->OpenedContainer = nullptr; // Закрываем открытый контейнер
}

void ASandboxPlayerController::OnTracePlayerActionPoint(const FHitResult& Res) {
	// Обработка результата трассировки точки действия игрока
}

bool ASandboxPlayerController::IsGameInputBlocked() {
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter()); // Получаем персонажа
	if (SandboxCharacter && !SandboxCharacter->InputEnabled()) { // Если персонаж не может получать ввод
		return true; // Возвращаем, что ввод заблокирован
	}

	return bIsGameInputBlocked; // Возвращаем состояние блокировки ввода
}

void ASandboxPlayerController::SetCurrentInventorySlot(int32 Slot) { 
	ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter()); // Получаем персонажа
	if (SandboxCharacter && SandboxCharacter->IsDead()) { // Если персонаж мертв
		return; // Выходим
	}

	CurrentInventorySlot = Slot; // Устанавливаем текущий слот инвентаря
}

UContainerComponent* ASandboxPlayerController::GetOpenedContainer() { 
	return this->OpenedContainer; // Возвращаем открытый контейнер
}

ASandboxObject* ASandboxPlayerController::GetOpenedObject() { 
	return this->OpenedObject; // Возвращаем открытый объект
}

void ASandboxPlayerController::ShowMouseCursor(bool bShowCursor) { 
	this->bShowMouseCursor = bShowCursor; // Установка видимости курсора
};

void ASandboxPlayerController::OnContainerMainAction(int32 SlotId, FName ContainerName) {
	// Основное действие с контейнером
}

void ASandboxPlayerController::OnContainerDropSuccess(int32 SlotId, FName SourceName, FName TargetName) {
	// Успешное действие при сбросе контейнера
}

bool ASandboxPlayerController::OnContainerDropCheck(int32 SlotId, FName ContainerName, const ASandboxObject* Obj) const {
	return true; // Проверка на возможность сброса объекта в контейнер
}

void ASandboxPlayerController::TransferContainerStack_Implementation(const FString& ObjectNetUid, const FString& ContainerName, const FContainerStack& Stack, const int SlotId) {
	if (LevelController) { // Если контроллер уровня валиден
		ASandboxObject* Obj = LevelController->GetObjectByNetUid(ObjectNetUid); // Получаем объект по его сетевому UID
		if (Obj) { // Если объект валиден
			TArray<UContainerComponent*> Components; // Список компонентов контейнера
			Obj->GetComponents<UContainerComponent>(Components); // Получаем компоненты контейнера
			for (UContainerComponent* Container : Components) { // Итерация по компонентам
				if (Container->GetName() == ContainerName) { // Если имя совпадает
					Container->SetStackDirectly(Stack, SlotId); // Устанавливаем стек напрямую
				}
			}

			Obj->ForceNetUpdate(); // Принудительное обновление сетевого состояния объекта
		}
	}
}

void ASandboxPlayerController::TransferInventoryStack_Implementation(const FString& ContainerName, const FContainerStack& Stack, const int SlotId) {
	ACharacter* PlayerCharacter = Cast<ACharacter>(GetCharacter()); // Получаем персонажа
	TArray<UContainerComponent*> Components; // Список компонентов контейнера
	PlayerCharacter->GetComponents<UContainerComponent>(Components); // Получаем компоненты контейнера
	for (UContainerComponent* Container : Components) { // Итерация по компонентам
		if (Container->GetName() == ContainerName) { // Если имя совпадает
			Container->SetStackDirectly(Stack, SlotId); // Устанавливаем стек напрямую
		}
	}

	PlayerCharacter->ForceNetUpdate(); // Принудительное обновление сетевого состояния персонажа
}

ASandboxLevelController* ASandboxPlayerController::GetLevelController() {
	return LevelController; // Возвращаем контроллер уровня
}

bool ASandboxPlayerController::OnContainerSlotHover(int32 SlotId, FName ContainerName) {
	return false; // Проверка на наведение курсора на слот контейнера
}

