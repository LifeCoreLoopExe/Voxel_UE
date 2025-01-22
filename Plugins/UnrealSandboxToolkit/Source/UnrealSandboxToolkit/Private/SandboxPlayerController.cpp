// Заполните свое уведомление об авторских правах на странице "Описание" в настройках проекта.

#include "SandboxPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "SandboxCharacter.h"
#include "SandboxObject.h"
#include "ContainerComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

// Конструктор для ASandboxPlayerController
ASandboxPlayerController::ASandboxPlayerController() {
    // bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;
    CurrentInventorySlot = -1;
    bIsGameInputBlocked = false;
}

// Вызывается, когда игра начинается или когда объект создается
void ASandboxPlayerController::BeginPlay() {
    Super::BeginPlay(); // Вызов функции BeginPlay родительского класса

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

// Вызывается каждый кадр
void ASandboxPlayerController::PlayerTick(float DeltaTime) {
    Super::PlayerTick(DeltaTime); // Вызов функции PlayerTick родительского класса

    if (bMoveToMouseCursor) {
        MoveToMouseCursor();
    }
}

// Настройка ввода
void ASandboxPlayerController::SetupInputComponent() {
    // Настройка привязок клавиш для игры
    Super::SetupInputComponent();

    InputComponent->BindAction("MainAction", IE_Pressed, this, &ASandboxPlayerController::OnMainActionPressedInternal);
    InputComponent->BindAction("MainAction", IE_Released, this, &ASandboxPlayerController::OnMainActionReleasedInternal);

    InputComponent->BindAction("AltAction", IE_Pressed, this, &ASandboxPlayerController::OnAltActionPressedInternal);
    InputComponent->BindAction("AltAction", IE_Released, this, &ASandboxPlayerController::OnAltActionReleasedInternal);

    // Поддержка сенсорных устройств
    // InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AUE4VoxelTerrainPlayerController::MoveToTouchLocation);
    // InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AUE4VoxelTerrainPlayerController::MoveToTouchLocation);

    InputComponent->BindAction("ToggleView", IE_Pressed, this, &ASandboxPlayerController::ToggleView);
}

// Перемещение к позиции курсора мыши
void ASandboxPlayerController::MoveToMouseCursor() {
    FHitResult Hit;
    GetHitResultUnderCursor(ECC_WorldStatic, false, Hit);

    if (Hit.bBlockingHit) {
        // Мы попали во что-то, перемещаемся туда
        SetNewMoveDestination(Hit.ImpactPoint);
    }
}

// Перемещение к позиции касания
void ASandboxPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location) {
    FVector2D ScreenSpaceLocation(Location);

    FHitResult HitResult;
    GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
    if (HitResult.bBlockingHit) {
        SetNewMoveDestination(HitResult.ImpactPoint);
    }
}

// Установка новой цели перемещения
void ASandboxPlayerController::SetNewMoveDestination(const FVector DestLocation) {
    ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter());

    if (SandboxCharacter) {
        float const Distance = FVector::Dist(DestLocation, SandboxCharacter->GetActorLocation());
        if (Distance > 120.0f) {
            UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
        }
    }
}

// Установка цели перемещения при нажатии
void ASandboxPlayerController::SetDestinationPressed() {
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

    bMoveToMouseCursor = true;
}

// Установка цели перемещения при отпускании
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

// Обработка нажатия основного действия
void ASandboxPlayerController::OnMainActionPressedInternal() {
    if (!IsGameInputBlocked()) {
        OnMainActionPressed();
    }
}

// Обработка отпускания основного действия
void ASandboxPlayerController::OnMainActionReleasedInternal() {
    if (!IsGameInputBlocked()) {
        OnMainActionReleased();
    }
}

// Обработка нажатия альтернативного действия
void ASandboxPlayerController::OnAltActionPressedInternal() {
    if (!IsGameInputBlocked()) {
        OnAltActionPressed();
    }
}

// Обработка отпускания альтернативного действия
void ASandboxPlayerController::OnAltActionReleasedInternal() {
    if (!IsGameInputBlocked()) {
        OnAltActionReleased();
    }
}

// Основное действие при нажатии
void ASandboxPlayerController::OnMainActionPressed() {
    // Реализация основного действия
}

// Основное действие при отпускании
void ASandboxPlayerController::OnMainActionReleased() {
    // Реализация основного действия
}

// Альтернативное действие при нажатии
void ASandboxPlayerController::OnAltActionPressed() {
    // Реализация альтернативного действия
}

// Альтернативное действие при отпускании
void ASandboxPlayerController::OnAltActionReleased() {
    // Реализация альтернативного действия
}

// Переключение вида
void ASandboxPlayerController::ToggleView() {
    if (IsGameInputBlocked()) {
        return;
    }
    // Реализация переключения вида
}

// Обработка обладания персонажем
void ASandboxPlayerController::OnPossess(APawn* NewPawn) {
    Super::OnPossess(NewPawn); // Вызов функции OnPossess родительского класса

    ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(NewPawn);
    if (SandboxCharacter) {
        SandboxCharacter->EnableInput(this);
        if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
            // bShowMouseCursor = true;
        } else {
            // bShowMouseCursor = false;
        }
    }

    // bShowMouseCursor = false;
}

// Блокировка игрового ввода
void ASandboxPlayerController::BlockGameInput() {
    // UWidgetBlueprintLibrary::SetInputMode_GameAndUI(this, nullptr, false, false);
    bIsGameInputBlocked = true;
    // bShowMouseCursor = true;
}

// Разблокировка игрового ввода
void ASandboxPlayerController::UnblockGameInput() {
    // UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
    bIsGameInputBlocked = false;
    // bShowMouseCursor = false;
}

// Трассировка и выбор объекта действия
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

// Трассировка точки действия игрока
FHitResult ASandboxPlayerController::TracePlayerActionPoint() {
    ASandboxCharacter* SandboxCharacter = Cast<ASandboxCharacter>(GetCharacter());
    if (!SandboxCharacter) {
        return FHitResult();
    }

    if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::THIRD_PERSON || SandboxCharacter->GetSandboxPlayerView() == PlayerView::FIRST_PERSON) {
        float MaxUseDistance = SandboxCharacter->InteractionTargetLength;

        if (SandboxCharacter->GetSandboxPlayerView() == PlayerView::THIRD_PERSON) {
            if (SandboxCharacter->GetCameraBoom() != nullptr) {
                // MaxUseDistance = Character->GetCameraBoom()->TargetArmLength + MaxUseDistance;
            }
        }

        FVector CamLoc;
        FRotator CamRot;
        GetPlayerViewPoint(CamLoc, CamRot);

        const FVector StartTrace = CamLoc;
        const FVector Direction = CamRot.Vector();
        const FVector EndTrace = StartTrace + (Direction * MaxUseDistance);

        FCollisionQueryParams TraceParams(FName(TEXT("")), true, this);
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

// Установка рендеринга пользовательской глубины
void SetRenderCustomDepth2(AActor* Actor, bool RenderCustomDepth) {
    TArray<UStaticMeshComponent*> MeshComponentList;
    Actor->GetComponents<UStaticMeshComponent>(MeshComponentList);

    for (UStaticMeshComponent* MeshComponent : MeshComponentList) {
        MeshComponent->SetRenderCustomDepth(RenderCustomDepth);
    }
}

// Обработка выбора объекта действия
void ASandboxPlayerController::OnSelectActionObject(AActor* Actor) {
    SetRenderCustomDepth2(Actor, true);
}

// Обработка отмены выбора объекта действия
void ASandboxPlayerController::OnDeselectActionObject(AActor* Actor) {
    SetRenderCustomDepth2(Actor, false);
}

// Выбор объекта действия
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

// Получение контейнера по имени
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

// Получение инвентаря
UContainerComponent* ASandboxPlayerController::GetInventory() {
    return GetContainerByName(TEXT("Inventory"));
}

// Взятие объекта в инвентарь
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

// Проверка, открыт ли контейнер
bool ASandboxPlayerController::HasOpenContainer() {
    return OpenedObject != nullptr;
}

// Трассировка и открытие контейнера объекта
bool ASandboxPlayerController::TraceAndOpenObjectContainer() {
    FHitResult ActionPoint = TracePlayerActionPoint();

    if (ActionPoint.bBlockingHit) {
        ASandboxObject* Obj = Cast<ASandboxObject>(ActionPoint.GetActor());
        return OpenObjectContainer(Obj);
    }

    return false;
}

// Закрытие объекта с контейнером
void ASandboxPlayerController::CloseObjectWithContainer() {
    this->OpenedObject = nullptr;
    this->OpenedContainer = nullptr;
}

// Обработка трассировки точки действия игрока
void ASandboxPlayerController::OnTracePlayerActionPoint(const FHitResult& Res) {
    // Реализация обработки трассировки
}

// Проверка, заблокирован ли игровой ввод
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

// Получение открытого контейнера
UContainerComponent* ASandboxPlayerController::GetOpenedContainer() {
    return this->OpenedContainer;
}

// Получение открытого объекта
ASandboxObject* ASandboxPlayerController::GetOpenedObject() {
    return this->OpenedObject;
}

// Показать или скрыть курсор мыши
void ASandboxPlayerController::ShowMouseCursor(bool bShowCursor) {
    this->bShowMouseCursor = bShowCursor;
}

// Обработка основного действия контейнера
void ASandboxPlayerController::OnContainerMainAction(int32 SlotId, FName ContainerName) {
    // Реализация обработки основного действия контейнера
}

// Обработка успешного сброса контейнера
void ASandboxPlayerController::OnContainerDropSuccess(int32 SlotId, FName SourceName, FName TargetName) {
    // Реализация обработки успешного сброса контейнера
}

// Проверка возможности сброса контейнера
bool ASandboxPlayerController::OnContainerDropCheck(int32 SlotId, FName ContainerName, const ASandboxObject* Obj) const {
    return true;
}

// Реализация переноса стека контейнера
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

// Реализация переноса стека инвентаря
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
