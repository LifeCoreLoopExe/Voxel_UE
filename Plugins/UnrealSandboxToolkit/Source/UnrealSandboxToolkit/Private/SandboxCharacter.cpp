// Fill out your copyright notice in the Description page of Project Settings.

#include "SandboxCharacter.h"
//#include "SandboxPlayerController.h"
//#include "VitalSystemComponent.h"


ASandboxCharacter::ASandboxCharacter() {
    VelocityHitThreshold = 1300; // Устанавливает порог удара по скорости
    VelocityHitFactor = 0.2f; // Устанавливает фактор удара по скорости
    VelocityHitTimestamp = 0; // Инициализирует временную метку удара

    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f); // Устанавливает размер капсулы

    bUseControllerRotationPitch = false; // Отключает использование тангажа контроллера
    bUseControllerRotationYaw = false; // Отключает использование рыскания контроллера
    bUseControllerRotationRoll = false; // Отключает использование крена контроллера

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom")); // Создает компонент камеры на пружине
    CameraBoom->SetupAttachment(RootComponent); // Прикрепляет камеру на пружине к корневому компоненту

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera")); // Создает компонент следования камеры
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Прикрепляет камеру к концу пружины
    FollowCamera->bUsePawnControlRotation = false; // Отключает вращение камеры относительно пружины
    FollowCamera->SetRelativeLocation(FVector(0, 0, 0)); // Устанавливает положение камеры

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera")); // Создает компонент камеры от первого лица
    FirstPersonCamera->SetupAttachment(GetMesh(), TEXT("head")); // Прикрепляет камеру к голове меша
    FirstPersonCamera->SetRelativeLocation(FVector(10.0f, 32.0f, 0.f)); // Устанавливает положение камеры
    FirstPersonCamera->SetRelativeRotation(FRotator(0, 90, -90)); // Устанавливает вращение камеры
    FirstPersonCamera->bUsePawnControlRotation = true; // Включает использование вращения аватара для камеры

    GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASandboxCharacter::OnHit); // Добавляет обработчик удара для капсулы

    CurrentPlayerView = PlayerView::TOP_DOWN; // Устанавливает начальный вид игрока
    InitTopDownView(); // Инициализирует вид сверху

    PrimaryActorTick.bCanEverTick = true; // Разрешает тикирование актера
    PrimaryActorTick.bStartWithTickEnabled = true; // Включает тикирование при старте

    MaxZoom = 500; // Устанавливает максимальный зум
    MaxZoomTopDown = 1200; // Устанавливает максимальный зум для вида сверху
    MinZoom = 100; // Устанавливает минимальный зум
    ZoomStep = 50; // Устанавливает шаг зума

    WalkSpeed = 200; // Устанавливает скорость ходьбы
    RunSpeed = 600; // Устанавливает скорость бега

    InteractionTargetLength = 200; // Устанавливает длину цели взаимодействия

    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; // Устанавливает максимальную скорость ходьбы
}

void ASandboxCharacter::BeginPlay() {
    Super::BeginPlay(); // Вызывает метод BeginPlay родительского класса

    CurrentPlayerView = InitialView; // Устанавливает текущий вид игрока

    if (InitialView == PlayerView::TOP_DOWN) {
        InitTopDownView(); // Инициализирует вид сверху
    }

    if (InitialView == PlayerView::THIRD_PERSON) {
        InitThirdPersonView(); // Инициализирует вид от третьего лица
    }

    if (InitialView == PlayerView::FIRST_PERSON) {
        InitFirstPersonView(); // Инициализирует вид от первого лица
    }

    /*
    TArray<UVitalSystemComponent*> Components;
    GetComponents<UVitalSystemComponent>(Components);

    for (UVitalSystemComponent* VitalSysCmp : Components) {
        VitalSystemComponent = VitalSysCmp;
        break;
    }
    */
}

void ASandboxCharacter::Tick(float DeltaTime) {
    Super::Tick(DeltaTime); // Вызывает метод Tick родительского класса

    if (IsDead()) {
        FVector MeshLoc = GetMesh()->GetSocketLocation(TEXT("pelvis")); // Получает положение таза меша
        //GetCapsuleComponent()->SetWorldLocation(MeshLoc - InitialMeshTransform.GetLocation());
    }
}

void ASandboxCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
    Super::SetupPlayerInputComponent(PlayerInputComponent); // Вызывает метод SetupPlayerInputComponent родительского класса

    //PlayerInputComponent->BindAction("ZoomIn", IE_Released, this, &ASandboxCharacter::ZoomIn);
    //PlayerInputComponent->BindAction("ZoomOut", IE_Released, this, &ASandboxCharacter::ZoomOut);

    PlayerInputComponent->BindAction("Boost", IE_Pressed, this, &ASandboxCharacter::BoostOn); // Привязывает действие ускорения
    PlayerInputComponent->BindAction("Boost", IE_Released, this, &ASandboxCharacter::BoostOff); // Привязывает действие отключения ускорения

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASandboxCharacter::Jump); // Привязывает действие прыжка
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASandboxCharacter::StopJumping); // Привязывает действие остановки прыжка

    PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ASandboxCharacter::MoveForward); // Привязывает ось движения вперед/назад
    PlayerInputComponent->BindAxis("Move Right / Left", this, &ASandboxCharacter::MoveRight); // Привязывает ось движения влево/вправо

    //InputComponent->BindAction("Test", IE_Pressed, this, &ASandboxCharacter::Test);

    // Мы имеем 2 версии привязок вращения для обработки разных типов устройств
    // "turn" обрабатывает устройства, предоставляющие абсолютную дельту, такие как мышь.
    // "turnrate" для устройств, которые мы выбираем обрабатывать как скорость изменения, такие как аналоговый джойстик
    PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &ASandboxCharacter::AddControllerYawInput); // Привязывает ось вращения влево/вправо мышью
    //PlayerInputComponent->BindAxis("TurnRate", this, &ASandboxCharacter::TurnAtRate);
    PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &ASandboxCharacter::AddControllerPitchInput); // Привязывает ось вращения вверх/вниз мышью
    //PlayerInputComponent->BindAxis("LookUpRate", this, &ASandboxCharacter::LookUpAtRate);
}

bool ASandboxCharacter::CanMove() {
    // ASandboxPlayerController* C = Cast<ASandboxPlayerController>(GetController());
    // if (!C || C->IsGameInputBlocked()) {
    //     return false;
    // }
    return true; // Возвращает true, если персонаж может двигаться
}

void ASandboxCharacter::BoostOn() {
    if (!CanMove()) {
        return; // Возвращает, если персонаж не может двигаться
    }

    /*
    TArray<UVitalSystemComponent*> Components;
    GetComponents<UVitalSystemComponent>(Components);
    if (Components.Num() > 0) {
        UVitalSystemComponent* Vs = Components[0];
        if (Vs->CanBoost()) {
            GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
            return;
        }
    } else {
        GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
    }
    */
}

void ASandboxCharacter::BoostOff() {
    if (!CanMove()) {
        return; // Возвращает, если персонаж не может двигаться
    }

    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; // Устанавливает максимальную скорость ходьбы
}

void ASandboxCharacter::Jump() {
    if (!CanMove()) {
        return; // Возвращает, если персонаж не может двигаться
    }

    if (CurrentPlayerView == PlayerView::TOP_DOWN) {
        return; // Возвращает, если текущий вид игрока - вид сверху
    }

    if (IsDead()) {
        return; // Возвращает, если персонаж мертв
    }

    Super::Jump(); // Вызывает метод Jump родительского класса
}

void ASandboxCharacter::StopJumping() {
    if (IsDead()) {
        return; // Возвращает, если персонаж мертв
    }

    Super::StopJumping(); // Вызывает метод StopJumping родительского класса
}

void ASandboxCharacter::ZoomIn() {
    if (!CanMove()) {
        return; // Возвращает, если персонаж не может двигаться
    }

    if (GetCameraBoom() == nullptr) {
        return; // Возвращает, если камера на пружине не существует
    }

    if (CurrentPlayerView == PlayerView::FIRST_PERSON) {
        return; // Возвращает, если текущий вид игрока - вид от первого лица
    }

    if (GetCameraBoom()->TargetArmLength > MinZoom) {
        GetCameraBoom()->TargetArmLength -= ZoomStep; // Уменьшает длину пружины камеры
    } else {
        if (bEnableAutoSwitchView) {
            InitFirstPersonView(); // Инициализирует вид от первого лица
        }
    }

    // UE_LOG(LogVt, Log, TEXT("ZoomIn: %f"), GetCameraBoom()->TargetArmLength);
}

void ASandboxCharacter::ZoomOut() {
    if (!CanMove()) {
        return; // Возвращает, если персонаж не может двигаться
    }

    if (GetCameraBoom() == nullptr) return; // Возвращает, если камера на пружине не существует

    if (CurrentPlayerView == PlayerView::FIRST_PERSON) {
        if (bEnableAutoSwitchView) {
            InitThirdPersonView(); // Инициализирует вид от третьего лица
            return;
        }
    };

    float MZ = (CurrentPlayerView == PlayerView::TOP_DOWN) ? MaxZoomTopDown : MaxZoom; // Устанавливает максимальный зум

    if (GetCameraBoom()->TargetArmLength < MZ) {
        GetCameraBoom()->TargetArmLength += ZoomStep; // Увеличивает длину пружины камеры
    }

    // UE_LOG(LogVt, Log, TEXT("ZoomOut: %f"), GetCameraBoom()->TargetArmLength);
}

FVector ASandboxCharacter::GetThirdPersonViewCameraPos() {
    return FVector(0, 0, 64); // Возвращает позицию камеры для вида от третьего лица
}

FRotator ASandboxCharacter::GetTopDownViewCameraRot() {
    return FRotator(-50.f, 0.f, 0.f); // Возвращает вращение камеры для вида сверху
}

void ASandboxCharacter::InitTopDownView() {
    if (IsDead()) {
        return; // Возвращает, если персонаж мертв
    }

    // Настраивает движение персонажа
    GetCharacterMovement()->bOrientRotationToMovement = true; // Вращает персонажа в направлении движения
    GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f); // Устанавливает скорость вращения
    GetCharacterMovement()->bConstrainToPlane = true; // Ограничивает движение в плоскости
    GetCharacterMovement()->bSnapToPlaneAtStart = true; // Привязывает к плоскости при старте

    CameraBoom->SetUsingAbsoluteRotation(true); // Не позволяет пружине вращаться вместе с персонажем
    CameraBoom->SetRelativeRotation(GetTopDownViewCameraRot()); // Устанавливает вращение камеры
    CameraBoom->TargetArmLength = MaxZoomTopDown; // Устанавливает длину пружины камеры
    CameraBoom->bDoCollisionTest = false; // Не позволяет камере втягиваться при столкновении с уровнем
    CameraBoom->bUsePawnControlRotation = false; // Вращает пружину на основе контроллера
    CameraBoom->ProbeSize = 0; // Устанавливает размер пробы
    CameraBoom->SetRelativeLocation(FVector(0, 0, 0)); // Устанавливает положение камеры

    FirstPersonCamera->Deactivate(); // Деактивирует камеру от первого лица
    FollowCamera->Activate(); // Активирует камеру следования

    bUseControllerRotationYaw = false; // Отключает использование рыскания контроллера

    CurrentPlayerView = PlayerView::TOP_DOWN; // Устанавливает текущий вид игрока

    /*
    ASandboxPlayerController* C = Cast<ASandboxPlayerController>(GetController());
    if (C != nullptr) {
        C->ShowMouseCursor(true); // Показывает курсор мыши
    }
    */
}

void ASandboxCharacter::InitThirdPersonView() {
    // Настраивает движение персонажа
    GetCharacterMovement()->bOrientRotationToMovement = true; // Персонаж двигается в направлении ввода
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // Скорость вращения
    // GetCharacterMovement()->JumpZVelocity = 600.f;
    // GetCharacterMovement()->AirControl = 0.2f;

    CameraBoom->TargetArmLength = 300.0f; // Камера следует на этом расстоянии за персонажем
    CameraBoom->bUsePawnControlRotation = true; // Вращает пружину на основе контроллера
    CameraBoom->bDoCollisionTest = true; // Включает проверку столкновений
    CameraBoom->ProbeSize = 12; // Устанавливает размер пробы
    CameraBoom->SetRelativeLocation(GetThirdPersonViewCameraPos()); // Устанавливает положение камеры

    FirstPersonCamera->Deactivate(); // Деактивирует камеру от первого лица
    FollowCamera->Activate(); // Активирует камеру следования

    bUseControllerRotationYaw = false; // Отключает использование рыскания контроллера

    CurrentPlayerView = PlayerView::THIRD_PERSON; // Устанавливает текущий вид игрока

    /*
    ASandboxPlayerController* Controller = Cast<ASandboxPlayerController>(GetController());
    if (Controller) {
        Controller->ShowMouseCursor(false); // Скрывает курсор мыши
    }
    */
}

void ASandboxCharacter::InitFirstPersonView() {
    if (IsDead()) {
        return; // Возвращает, если персонаж мертв
    }

    // Настраивает движение персонажа
    GetCharacterMovement()->bOrientRotationToMovement = false; // Персонаж не ориентируется по направлению движения
    // GetCharacterMovement()->JumpZVelocity = 600.f;
    // GetCharacterMovement()->AirControl = 0.2f;

    FirstPersonCamera->Activate(); // Активирует камеру от первого лица
    FollowCamera->Deactivate(); // Деактивирует камеру следования

    bUseControllerRotationYaw = true; // Включает использование рыскания контроллера

    CurrentPlayerView = PlayerView::FIRST_PERSON; // Устанавливает текущий вид игрока

    /*
    ASandboxPlayerController* Controller = Cast<ASandboxPlayerController>(GetController());
    if (Controller) {
        Controller->ShowMouseCursor(false); // Скрывает курсор мыши
    }
    */
}

void ASandboxCharacter::AddControllerYawInput(float Val) {
    if (!CanMove()) {
        return; // Возвращает, если персонаж не может двигаться
    }

    // if (Controller->IsGameInputBlocked() && CurrentPlayerView != PlayerView::THIRD_PERSON) {
    //     return;
    // }

    if (CurrentPlayerView == PlayerView::TOP_DOWN) {
        return; // Возвращает, если текущий вид игрока - вид сверху
    }

    Super::AddControllerYawInput(Val); // Вызывает метод AddControllerYawInput родительского класса
}

void ASandboxCharacter::AddControllerPitchInput(float Val) {
    if (!CanMove()) {
        return; // Возвращает, если персонаж не может двигаться
    }

    /*
    ASandboxPlayerController* Controller = Cast<ASandboxPlayerController>(GetController());
    if (Controller->IsGameInputBlocked() && CurrentPlayerView != PlayerView::THIRD_PERSON) {
        // return;
    }
    */

    if (CurrentPlayerView == PlayerView::TOP_DOWN) {
        return; // Возвращает, если текущий вид игрока - вид сверху
    }

    Super::AddControllerPitchInput(Val); // Вызывает метод AddControllerPitchInput родительского класса
}

void ASandboxCharacter::TurnAtRate(float Rate) {
    if (!CanMove()) {
        return; // Возвращает, если персонаж не может двигаться
    }

    if (CurrentPlayerView == PlayerView::TOP_DOWN) {
        return; // Возвращает, если текущий вид игрока - вид сверху
    }

    // calculate delta for this frame from the rate information
    // AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASandboxCharacter::LookUpAtRate(float Rate) {
    if (!CanMove()) {
        return; // Возвращает, если персонаж не может двигаться
    }

    if (CurrentPlayerView == PlayerView::TOP_DOWN) {
        return; // Возвращает, если текущий вид игрока - вид сверху
    }

    // calculate delta for this frame from the rate information
    // AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASandboxCharacter::MoveForward(float Value) {
    if (!CanMove()) {
        return; // Возвращает, если персонаж не может двигаться
    }

    if (IsDead()) {
        return; // Возвращает, если персонаж мертв
    }

    if (CurrentPlayerView == PlayerView::THIRD_PERSON) {
        if (Value != 0.0f) {
            // find out which way is forward
            const FRotator Rotation = Controller->GetControlRotation();
            const FRotator YawRotation(0, Rotation.Yaw, 0);

            // get forward vector
            const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
            AddMovementInput(Direction, Value); // Добавляет движение в направлении
        }
    }

    if (CurrentPlayerView == PlayerView::FIRST_PERSON) {
        if (Value != 0.0f) {
            // add movement in that direction
            AddMovementInput(GetActorForwardVector(), Value); // Добавляет движение в направлении актера
        }
    }
}

void ASandboxCharacter::MoveRight(float Value) {
    if (!CanMove()) {
        return; // Возвращает, если персонаж не может двигаться
    }

    if (IsDead()) {
        return; // Возвращает, если персонаж мертв
    }

    if (CurrentPlayerView == PlayerView::THIRD_PERSON) {
        if (Value != 0.0f) {
            // find out which way is right
            const FRotator Rotation = Controller->GetControlRotation();
            const FRotator YawRotation(0, Rotation.Yaw, 0);

            // get right vector
            const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
            // add movement in that direction
            AddMovementInput(Direction, Value); // Добавляет движение в направлении
        }
    }

    if (CurrentPlayerView == PlayerView::FIRST_PERSON) {
        if (Value != 0.0f) {
            // add movement in that direction
            AddMovementInput(GetActorRightVector(), Value); // Добавляет движение в направлении актера
        }
    }
}

PlayerView ASandboxCharacter::GetSandboxPlayerView() {
    return CurrentPlayerView; // Возвращает текущий вид игрока
}

void ASandboxCharacter::SetSandboxPlayerView(PlayerView SandboxView) {
    CurrentPlayerView = SandboxView; // Устанавливает текущий вид игрока
}

/*
void ASandboxCharacter::Test() {
    if (!IsDead()) {
        Kill(); // Убивает персонажа
    } else {
        LiveUp(); // Оживляет персонажа
    }
}
*/

void ASandboxCharacter::OnDeath() {
    GetMesh()->SetSimulatePhysics(true); // Включает физическую симуляцию меша
    GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(TEXT("pelvis"), 1); // Устанавливает вес физической симуляции для всех частей меша ниже таза
}

void ASandboxCharacter::Kill() {
    if (!IsDead()) {
        if (CurrentPlayerView == PlayerView::FIRST_PERSON) {
            InitThirdPersonView(); // Инициализирует вид от третьего лица
        }

        InitialMeshTransform = GetMesh()->GetRelativeTransform(); // Сохраняет начальную трансформацию меша
        bIsDead = true; // Устанавливает флаг смерти

        OnDeath(); // Вызывает метод OnDeath
    }
}

void ASandboxCharacter::LiveUp() {
    if (IsDead()) {
        GetMesh()->SetSimulatePhysics(false); // Отключает физическую симуляцию меша
        bIsDead = false; // Сбрасывает флаг смерти
        GetMesh()->SetupAttachment(GetCapsuleComponent(), NAME_None); // Прикрепляет меш к капсуле
        GetMesh()->SetRelativeTransform(InitialMeshTransform); // Устанавливает начальную трансформацию меша
    }
}

int ASandboxCharacter::GetSandboxTypeId() {
    return SandboxTypeId; // Возвращает идентификатор типа Sandbox
}

FString ASandboxCharacter::GetSandboxPlayerUid() {
    return ""; // Возвращает пустую строку (заглушка)
}

void ASandboxCharacter::OnHit(class UPrimitiveComponent* HitComp, class AActor* Actor, class UPrimitiveComponent* Other, FVector Impulse, const FHitResult& HitResult) {
    /*
    float HitVelocity = GetCapsuleComponent()->GetComponentVelocity().Size();
    if (VitalSystemComponent != nullptr) {
        if (HitVelocity > VelocityHitThreshold) {
            const double Timestamp = FPlatformTime::Seconds();
            const double D = Timestamp - VelocityHitTimestamp;
            if (D > 0.5) {
                UE_LOG(LogTemp, Log, TEXT("HitVelocity -> %f"), HitVelocity);
                VelocityHitTimestamp = Timestamp;
                const float Damage = (HitVelocity - VelocityHitThreshold) * VelocityHitFactor;
                VitalSystemComponent->Damage(Damage);
            }
        }
    }
    */
}
