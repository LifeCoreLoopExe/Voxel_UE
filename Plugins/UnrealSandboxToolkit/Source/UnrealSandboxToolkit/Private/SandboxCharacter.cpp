#include "SandboxCharacter.h"
//#include "SandboxPlayerController.h"
//#include "VitalSystemComponent.h"

ASandboxCharacter::ASandboxCharacter() {
    VelocityHitThreshold = 1300; // Порог скорости для определения удара
    VelocityHitFactor = 0.2f; // Множитель для расчета урона от удара
    VelocityHitTimestamp = 0; // Временная метка последнего удара

    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f); // Инициализация размера капсулы персонажа

    bUseControllerRotationPitch = false; // Отключение использования поворота контроллера по оси pitch
    bUseControllerRotationYaw = false; // Отключение использования поворота контроллера по оси yaw
    bUseControllerRotationRoll = false; // Отключение использования поворота контроллера по оси roll

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom")); // Создание компонента камеры на пружине
    CameraBoom->SetupAttachment(RootComponent); // Прикрепление камеры к корневому компоненту

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera")); // Создание компонента следящей камеры
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Прикрепление камеры к концу пружины
    FollowCamera->bUsePawnControlRotation = false; // Камера не вращается относительно пружины
    FollowCamera->SetRelativeLocation(FVector(0, 0, 0)); // Позиционирование камеры

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera")); // Создание компонента камеры от первого лица
    FirstPersonCamera->SetupAttachment(GetMesh(), TEXT("head")); // Прикрепление камеры к голове персонажа
    FirstPersonCamera->SetRelativeLocation(FVector(10.0f, 32.0f, 0.f)); // Позиционирование камеры
    FirstPersonCamera->SetRelativeRotation(FRotator(0, 90, -90)); // Поворот камеры
    FirstPersonCamera->bUsePawnControlRotation = true; // Камера использует поворот контроллера

    GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASandboxCharacter::OnHit); // Добавление обработчика события столкновения капсулы

    // начальный вид
    CurrentPlayerView = PlayerView::TOP_DOWN; // Установка начального вида камеры
    InitTopDownView(); // Инициализация вида сверху

    PrimaryActorTick.bCanEverTick = true; // Разрешение тика актера
    PrimaryActorTick.bStartWithTickEnabled = true; // Включение тика актера при старте

    MaxZoom = 500; // Максимальное увеличение камеры
    MaxZoomTopDown = 1200; // Максимальное увеличение камеры для вида сверху
    MinZoom = 100; // Минимальное увеличение камеры
    ZoomStep = 50; // Шаг изменения увеличения камеры

    WalkSpeed = 200; // Скорость ходьбы
    RunSpeed = 600; // Скорость бега

    InteractionTargetLength = 200; // Длина цели взаимодействия

    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; // Установка максимальной скорости ходьбы
}

void ASandboxCharacter::BeginPlay() {
    Super::BeginPlay(); // Вызов родительского метода BeginPlay

    CurrentPlayerView = InitialView; // Установка текущего вида камеры

    if (InitialView == PlayerView::TOP_DOWN) {
        InitTopDownView(); // Инициализация вида сверху
    }

    if (InitialView == PlayerView::THIRD_PERSON) {
        InitThirdPersonView(); // Инициализация вида от третьего лица
    }

    if (InitialView == PlayerView::FIRST_PERSON) {
        InitFirstPersonView(); // Инициализация вида от первого лица
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
    Super::Tick(DeltaTime); // Вызов родительского метода Tick

    if (IsDead()) {
        FVector MeshLoc = GetMesh()->GetSocketLocation(TEXT("pelvis")); // Получение позиции таза персонажа
        //GetCapsuleComponent()->SetWorldLocation(MeshLoc - InitialMeshTransform.GetLocation());
    }
}

void ASandboxCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
    Super::SetupPlayerInputComponent(InputComponent); // Вызов родительского метода SetupPlayerInputComponent

    //PlayerInputComponent->BindAction("ZoomIn", IE_Released, this, &ASandboxCharacter::ZoomIn);
    //PlayerInputComponent->BindAction("ZoomOut", IE_Released, this, &ASandboxCharacter::ZoomOut);

    PlayerInputComponent->BindAction("Boost", IE_Pressed, this, &ASandboxCharacter::BoostOn); // Привязка действия ускорения
    PlayerInputComponent->BindAction("Boost", IE_Released, this, &ASandboxCharacter::BoostOff); // Привязка действия отключения ускорения

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASandboxCharacter::Jump); // Привязка действия прыжка
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASandboxCharacter::StopJumping); // Привязка действия остановки прыжка

    PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ASandboxCharacter::MoveForward); // Привязка оси движения вперед/назад
    PlayerInputComponent->BindAxis("Move Right / Left", this, &ASandboxCharacter::MoveRight); // Привязка оси движения вправо/влево

    //InputComponent->BindAction("Test", IE_Pressed, this, &ASandboxCharacter::Test);

    // Мы имеем 2 версии привязок поворота для обработки разных типов устройств
    // "turn" обрабатывает устройства, предоставляющие абсолютное изменение, такие как мышь.
    // "turnrate" используется для устройств, которые мы выбираем для обработки как скорость изменения, такие как аналоговый джойстик
    PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &ASandboxCharacter::AddControllerYawInput); // Привязка оси поворота мышью вправо/влево
    //PlayerInputComponent->BindAxis("TurnRate", this, &ASandboxCharacter::TurnAtRate);
    PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &ASandboxCharacter::AddControllerPitchInput); // Привязка оси поворота мышью вверх/вниз
    //PlayerInputComponent->BindAxis("LookUpRate", this, &ASandboxCharacter::LookUpAtRate);
}

bool ASandboxCharacter::CanMove() {
    //ASandboxPlayerController* C = Cast<ASandboxPlayerController>(GetController());
    //if (!C || C->IsGameInputBlocked()) {
    //    return false;
    //}

    return true; // Возвращение true, если персонаж может двигаться
}

void ASandboxCharacter::BoostOn() {
    if (!CanMove()) {
        return; // Выход, если персонаж не может двигаться
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
        return; // Выход, если персонаж не может двигаться
    }

    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; // Установка максимальной скорости ходьбы
}

void ASandboxCharacter::Jump() {
    if (!CanMove()) {
        return; // Выход, если персонаж не может двигаться
    }

    if (CurrentPlayerView == PlayerView::TOP_DOWN) {
        return; // Выход, если текущий вид камеры - сверху
    }

    if (IsDead()) {
        return; // Выход, если персонаж мертв
    }

    Super::Jump(); // Вызов родительского метода Jump
}

void ASandboxCharacter::StopJumping() {
    if (IsDead()) {
        return; // Выход, если персонаж мертв
    }

    Super::StopJumping(); // Вызов родительского метода StopJumping
}

void ASandboxCharacter::ZoomIn() {
    if (!CanMove()) {
        return; // Выход, если персонаж не может двигаться
    }

    if (GetCameraBoom() == NULL) {
        return; // Выход, если камера на пружине не существует
    }

    if (CurrentPlayerView == PlayerView::FIRST_PERSON) {
        return; // Выход, если текущий вид камеры - от первого лица
    }

    if (GetCameraBoom()->TargetArmLength > MinZoom) {
        GetCameraBoom()->TargetArmLength -= ZoomStep; // Уменьшение длины пружины камеры
    } else {
        if (bEnableAutoSwitchView) {
            InitFirstPersonView(); // Переключение на вид от первого лица
        }
    }

    //UE_LOG(LogVt, Log, TEXT("ZoomIn: %f"), GetCameraBoom()->TargetArmLength);
}

void ASandboxCharacter::ZoomOut() {
    if (!CanMove()) {
        return; // Выход, если персонаж не может двигаться
    }

    if (GetCameraBoom() == NULL) return; // Выход, если камера на пружине не существует

    if (CurrentPlayerView == PlayerView::FIRST_PERSON) {
        if (bEnableAutoSwitchView) {
            InitThirdPersonView(); // Переключение на вид от третьего лица
            return;
        }
    };

    float MZ = (CurrentPlayerView == PlayerView::TOP_DOWN) ? MaxZoomTopDown : MaxZoom; // Определение максимального увеличения камеры

    if (GetCameraBoom()->TargetArmLength < MZ) {
        GetCameraBoom()->TargetArmLength += ZoomStep; // Увеличение длины пружины камеры
    }

    //UE_LOG(LogVt, Log, TEXT("ZoomOut: %f"), GetCameraBoom()->TargetArmLength);
}

FVector ASandboxCharacter::GetThirdPersonViewCameraPos() {
    return FVector(0, 0, 64); // Возвращение позиции камеры для вида от третьего лица
}

FRotator ASandboxCharacter::GetTopDownViewCameraRot() {
    return FRotator(-50.f, 0.f, 0.f); // Возвращение поворота камеры для вида сверху
}

void ASandboxCharacter::InitTopDownView() {
    if (IsDead()) {
        return; // Выход, если персонаж мертв
    }

    // Настройка движения персонажа
    GetCharacterMovement()->bOrientRotationToMovement = true; // Поворот персонажа в направлении движения
    GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f); // Скорость поворота персонажа
    GetCharacterMovement()->bConstrainToPlane = true; // Ограничение движения персонажа в плоскости
    GetCharacterMovement()->bSnapToPlaneAtStart = true; // Привязка персонажа к плоскости при старте

    CameraBoom->SetUsingAbsoluteRotation(true); // Отключение использования абсолютного поворота камеры

    CameraBoom->SetRelativeRotation(GetTopDownViewCameraRot()); // Установка поворота камеры для вида сверху
    CameraBoom->TargetArmLength = MaxZoomTopDown; // Установка длины пружины камеры
    CameraBoom->bDoCollisionTest = false; // Отключение проверки столкновений камеры
    CameraBoom->bUsePawnControlRotation = false; // Отключение использования поворота контроллера камеры
    CameraBoom->ProbeSize = 0; // Установка размера пробы камеры
    CameraBoom->SetRelativeLocation(FVector(0, 0, 0)); // Установка позиции камеры

    FirstPersonCamera->Deactivate(); // Деактивация камеры от первого лица
    FollowCamera->Activate(); // Активация следящей камеры

    bUseControllerRotationYaw = false; // Отключение использования поворота контроллера по оси yaw

    CurrentPlayerView = PlayerView::TOP_DOWN; // Установка текущего вида камеры - сверху

    /*
    ASandboxPlayerController* C = Cast<ASandboxPlayerController>(GetController());
    if (C != NULL) {
        C->ShowMouseCursor(true);
    }
    */
}

void ASandboxCharacter::InitThirdPersonView() {
    // Настройка движения персонажа
    GetCharacterMovement()->bOrientRotationToMovement = true; // Персонаж движется в направлении ввода
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // Скорость поворота персонажа
    //GetCharacterMovement()->JumpZVelocity = 600.f;
    //GetCharacterMovement()->AirControl = 0.2f;

    CameraBoom->TargetArmLength = 300.0f; // Камера следует на этом расстоянии за персонажем
    CameraBoom->bUsePawnControlRotation = true; // Поворот пружины на основе контроллера
    CameraBoom->bDoCollisionTest = true; // Включение проверки столкновений камеры
    CameraBoom->ProbeSize = 12; // Установка размера пробы камеры
    CameraBoom->SetRelativeLocation(GetThirdPersonViewCameraPos()); // Установка позиции камеры

    FirstPersonCamera->Deactivate(); // Деактивация камеры от первого лица
    FollowCamera->Activate(); // Активация следящей камеры

    bUseControllerRotationYaw = false; // Отключение использования поворота контроллера по оси yaw

    CurrentPlayerView = PlayerView::THIRD_PERSON; // Установка текущего вида камеры - от третьего лица

    /*
    ASandboxPlayerController* Controller = Cast<ASandboxPlayerController>(GetController());
    if (Controller) {
        Controller->ShowMouseCursor(false);
    }
    */
}

void ASandboxCharacter::InitFirstPersonView() {
    if (IsDead()) {
        return; // Выход, если персонаж мертв
    }

    // Настройка движения персонажа
    GetCharacterMovement()->bOrientRotationToMovement = false; // Персонаж движется в направлении ввода
    //GetCharacterMovement()->JumpZVelocity = 600.f;
    //GetCharacterMovement()->AirControl = 0.2f;

    FirstPersonCamera->Activate(); // Активация камеры от первого лица
    FollowCamera->Deactivate(); // Деактивация следящей камеры

    bUseControllerRotationYaw = true; // Включение использования поворота контроллера по оси yaw

    CurrentPlayerView = PlayerView::FIRST_PERSON; // Установка текущего вида камеры - от первого лица

    /*
    ASandboxPlayerController* Controller = Cast<ASandboxPlayerController>(GetController());
    if (Controller) {
        Controller->ShowMouseCursor(false);
    }
    */
}

void ASandboxCharacter::AddControllerYawInput(float Val) {
    if (!CanMove()) {
        return; // Выход, если персонаж не может двигаться
    }

    //if (Controller->IsGameInputBlocked() && CurrentPlayerView != PlayerView::THIRD_PERSON) {
        //return;
    //}

    if (CurrentPlayerView == PlayerView::TOP_DOWN) {
        return; // Выход, если текущий вид камеры - сверху
    }

    Super::AddControllerYawInput(Val); // Вызов родительского метода AddControllerYawInput

}

void ASandboxCharacter::AddControllerPitchInput(float Val) {
    if (!CanMove()) {
        return; // Выход, если персонаж не может двигаться
    }

    /*
    ASandboxPlayerController* Controller = Cast<ASandboxPlayerController>(GetController());
    if (Controller->IsGameInputBlocked() && CurrentPlayerView != PlayerView::THIRD_PERSON) {
        //return;
    }
    */

    if (CurrentPlayerView == PlayerView::TOP_DOWN){
        return; // Выход, если текущий вид камеры - сверху
    }

    Super::AddControllerPitchInput(Val); // Вызов родительского метода AddControllerPitchInput
}

void ASandboxCharacter::TurnAtRate(float Rate) {
    if (!CanMove()) {
        return; // Выход, если персонаж не может двигаться
    }

    if (CurrentPlayerView == PlayerView::TOP_DOWN) {
        return; // Выход, если текущий вид камеры - сверху
    }

    // расчет дельты для этого кадра из информации о скорости
    //AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASandboxCharacter::LookUpAtRate(float Rate) {
    if (!CanMove()) {
        return; // Выход, если персонаж не может двигаться
    }

    if (CurrentPlayerView == PlayerView::TOP_DOWN) {
        return; // Выход, если текущий вид камеры - сверху
    }

    // расчет дельты для этого кадра из информации о скорости
    //AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASandboxCharacter::MoveForward(float Value) {
    if (!CanMove()) {
        return; // Выход, если персонаж не может двигаться
    }

    if (IsDead()) {
        return; // Выход, если персонаж мертв
    };

    if (CurrentPlayerView == PlayerView::THIRD_PERSON) {
        if (Value != 0.0f) {
            // определение направления вперед
            const FRotator Rotation = Controller->GetControlRotation(); // Получение поворота контроллера
            const FRotator YawRotation(0, Rotation.Yaw, 0); // Получение поворота по оси yaw

            // получение вектора направления
            const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // Получение единичного вектора направления по оси X
            AddMovementInput(Direction, Value); // Добавление ввода движения в этом направлении
        }
    }

    if (CurrentPlayerView == PlayerView::FIRST_PERSON) {
        if (Value != 0.0f) {
            // добавление движения в этом направлении
            AddMovementInput(GetActorForwardVector(), Value); // Добавление ввода движения в направлении вперед
        }
    }
}

void ASandboxCharacter::MoveRight(float Value) {
    if (!CanMove()) {
        return; // Выход, если персонаж не может двигаться
    }

    if (IsDead()) {
        return; // Выход, если персонаж мертв
    };

    if (CurrentPlayerView == PlayerView::THIRD_PERSON) {
        if (Value != 0.0f) {
            // определение направления вправо
            const FRotator Rotation = Controller->GetControlRotation(); // Получение поворота контроллера
            const FRotator YawRotation(0, Rotation.Yaw, 0); // Получение поворота по оси yaw

            // получение вектора направления
            const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); // Получение единичного вектора направления по оси Y
            // добавление движения в этом направлении
            AddMovementInput(Direction, Value); // Добавление ввода движения в направлении вправо
        }
    }

    if (CurrentPlayerView == PlayerView::FIRST_PERSON) {
        if (Value != 0.0f) {
            // добавление движения в этом направлении
            AddMovementInput(GetActorRightVector(), Value); // Добавление ввода движения в направлении вправо
        }
    }
}

PlayerView ASandboxCharacter::GetSandboxPlayerView() {
    return CurrentPlayerView; // Возвращение текущего вида камеры
}

void ASandboxCharacter::SetSandboxPlayerView(PlayerView SandboxView) {
    CurrentPlayerView = SandboxView; // Установка текущего вида камеры
}

/*
void ASandboxCharacter::Test() {
    if(!IsDead()) {
        Kill();
    } else {
        LiveUp();
    }
}
*/

void ASandboxCharacter::OnDeath() {
    GetMesh()->SetSimulatePhysics(true); // Включение симуляции физики для меша
    GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(TEXT("pelvis"), 1); // Установка веса физики для всех частей меша ниже таза
}

void ASandboxCharacter::Kill() {
    if (!IsDead()) {
        if (CurrentPlayerView == PlayerView::FIRST_PERSON) {
            InitThirdPersonView(); // Переключение на вид от третьего лица
        }

        InitialMeshTransform = GetMesh()->GetRelativeTransform(); // Сохранение начального преобразования меша
        bIsDead = true; // Установка флага смерти

        OnDeath(); // Вызов метода OnDeath
    }
}

void ASandboxCharacter::LiveUp() {
    if (IsDead()) {
        GetMesh()->SetSimulatePhysics(false); // Отключение симуляции физики для меша
        bIsDead = false; // Сброс флага смерти
        GetMesh()->SetupAttachment(GetCapsuleComponent(), NAME_None); // Прикрепление меша к капсуле
        GetMesh()->SetRelativeTransform(InitialMeshTransform); // Установка начального преобразования меша
    }
}

int ASandboxCharacter::GetSandboxTypeId() {
    return SandboxTypeId; // Возвращение идентификатора типа песочницы
}

FString ASandboxCharacter::GetSandboxPlayerUid() {
    return ""; // Возвращение пустой строки для идентификатора пользователя песочницы
}

void ASandboxCharacter::OnHit(class UPrimitiveComponent* HitComp, class AActor* Actor, class UPrimitiveComponent* Other, FVector Impulse, const FHitResult & HitResult) {
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