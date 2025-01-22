// Copyright blackw 2015-2020

#include "SandboxCharacter.h"

// Конструктор (настройка персонажа при создании)
ASandboxCharacter::ASandboxCharacter() {
    // Настройки столкновений (капсула-тело персонажа)
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f); // Размер как у бочки
    
    // Камера-удочка (SpringArm) - держит камеру на расстоянии
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent); // Прикрепляем к корню
    
    // Основная камера следования
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    FollowCamera->SetupAttachment(CameraBoom); // Камера на конце удочки
    
    // Камера от первого лица (внутри головы)
    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetMesh(), TEXT("head")); // Прикрепляем к кости головы
    
    // Настройки передвижения
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; // Скорость ходьбы как у человека
    WalkSpeed = 200;  // 7.2 км/ч
    RunSpeed = 600;   // 21.6 км/ч (супер-бег!)
}

// Начало игры (первый кадр)
void ASandboxCharacter::BeginPlay() {
    Super::BeginPlay();
    
    // Выбор стартового вида камеры
    switch(InitialView) {
        case PlayerView::TOP_DOWN:    // Вид сверху как в стратегиях
            InitTopDownView(); break;
        case PlayerView::THIRD_PERSON: // Вид за спиной как в GTA
            InitThirdPersonView(); break;
        case PlayerView::FIRST_PERSON: // Вид из глаз как в CS:GO
            InitFirstPersonView(); break;
    }
}

// Обработка кадра (вызывается постоянно)
void ASandboxCharacter::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    
    // Если персонаж мертв - смещаем капсулу к анимации
    if(IsDead()) {
        FVector MeshLoc = GetMesh()->GetSocketLocation(TEXT("pelvis"));
        //... (логика смещения)
    }
}

// Настройка управления (кнопки и оси)
void ASandboxCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
    Super::SetupPlayerInputComponent(InputComponent);
    
    // Привязка действий:
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASandboxCharacter::Jump); // Пробел
    PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ASandboxCharacter::MoveForward); // W/S
    PlayerInputComponent->BindAxis("Move Right / Left", this, &ASandboxCharacter::MoveRight); // A/D
    
    // Вращение камеры
    PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &ASandboxCharacter::AddControllerYawInput); // Мышь X
    PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &ASandboxCharacter::AddControllerPitchInput); // Мышь Y
}

// Движение вперед/назад
void ASandboxCharacter::MoveForward(float Value) {
    if(!CanMove() || IsDead()) return;
    
    // Для третьего лица:
    if(CurrentPlayerView == PlayerView::THIRD_PERSON) {
        // Двигаемся относительно направления камеры
        const FRotator Rotation = Controller->GetControlRotation();
        AddMovementInput(FRotationMatrix(FRotator(0, Rotation.Yaw, 0)).GetUnitAxis(EAxis::X), Value);
    }
    
    // Для первого лица:
    if(CurrentPlayerView == PlayerView::FIRST_PERSON) {
        AddMovementInput(GetActorForwardVector(), Value); // Движение куда смотрит голова
    }
}

// Смерть персонажа
void ASandboxCharacter::OnDeath() {
    GetMesh()->SetSimulatePhysics(true); // Труп падает как тряпичная кукла
    GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(TEXT("pelvis"), 1); // Реалистичное падение
    
    if(CurrentPlayerView == PlayerView::FIRST_PERSON) {
        InitThirdPersonView(); // Переключаем вид чтобы видеть свое тело
    }
}

// Переключение видов камеры
void ASandboxCharacter::InitFirstPersonView() {
    FirstPersonCamera->Activate(); // Включаем камеру в голове
    FollowCamera->Deactivate();    // Выключаем камеру сзади
    bUseControllerRotationYaw = true; // Поворот камеры = поворот персонажа
}

void ASandboxCharacter::InitThirdPersonView() {
    CameraBoom->TargetArmLength = 300.f; // Камера на расстоянии 3 метра
    FollowCamera->Activate(); // Включаем камеру-удочку
}

void ASandboxCharacter::InitTopDownView() {
    CameraBoom->SetRelativeRotation(FRotator(-50.f, 0, 0)); // Камера смотрит сверху
    CameraBoom->TargetArmLength = 1200.f; // Высота как у карты
}

// Управление зумом (приближение/отдаление)
void ASandboxCharacter::ZoomIn() {
    if(CurrentPlayerView == PlayerView::FIRST_PERSON) return;
    
    if(CameraBoom->TargetArmLength > MinZoom) {
        CameraBoom->TargetArmLength -= ZoomStep; // Приближаем камеру
    } else {
        InitFirstPersonView(); // Автопереход в первый человек
    }
}

void ASandboxCharacter::ZoomOut() {
    if(CameraBoom->TargetArmLength < MaxZoom) {
        CameraBoom->TargetArmLength += ZoomStep; // Отдаляем камеру
    }
}