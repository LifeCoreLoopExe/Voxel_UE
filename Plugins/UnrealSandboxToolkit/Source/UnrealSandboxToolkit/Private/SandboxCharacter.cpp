// Заполните свое уведомление об авторских правах на странице "Описание" в настройках проекта.

#include "SandboxCharacter.h"
//#include "SandboxPlayerController.h"
//#include "VitalSystemComponent.h"

// Конструктор для ASandboxCharacter
ASandboxCharacter::ASandboxCharacter() {
    VelocityHitThreshold = 1300; // Порог скорости для удара
    VelocityHitFactor = 0.2f; // Фактор удара по скорости
    VelocityHitTimestamp = 0; // Временная метка удара по скорости

    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f); // Инициализация размера капсулы

    bUseControllerRotationPitch = false; // Не использовать поворот контроллера по тангажу
    bUseControllerRotationYaw = false; // Не использовать поворот контроллера по рысканью
    bUseControllerRotationRoll = false; // Не использовать поворот контроллера по крену

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom")); // Создание компонента камеры
    CameraBoom->SetupAttachment(RootComponent); // Прикрепление камеры к корневому компоненту

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera")); // Создание следующей камеры
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Прикрепление камеры к концу бума и позволение буму настраиваться для соответствия ориентации контроллера
    FollowCamera->bUsePawnControlRotation = false; // Камера не вращается относительно руки
    FollowCamera->SetRelativeLocation(FVector(0, 0, 0)); // Позиционирование камеры

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera")); // Создание камеры от первого лица
    FirstPersonCamera->SetupAttachment(GetMesh(), TEXT("head")); // Прикрепление камеры к голове персонажа
    FirstPersonCamera->SetRelativeLocation(FVector(10.0f, 32.0f, 0.f)); // Позиционирование камеры
    FirstPersonCamera->SetRelativeRotation(FRotator(0, 90, -90)); // Поворот камеры
    FirstPersonCamera->bUsePawnControlRotation = true; // Использовать поворот контроллера персонажа

    GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASandboxCharacter::OnHit); // Добавление обработчика удара

    // начальный вид
    CurrentPlayerView = PlayerView::TOP_DOWN; // Установка начального вида
    InitTopDownView(); // Инициализация вида сверху

    PrimaryActorTick.bCanEverTick = true; // Разрешение тиков для этого актора
    PrimaryActorTick.bStartWithTickEnabled = true; // Начало с включенными тиками

    MaxZoom = 500; // Максимальное увеличение
    MaxZoomTopDown = 1200; // Максимальное увеличение для вида сверху
    MinZoom = 100; // Минимальное увеличение
    ZoomStep = 50; // Шаг увеличения

    WalkSpeed = 200; // Скорость ходьбы
    RunSpeed = 600; // Скорость бега

    InteractionTargetLength = 200; // Длина цели взаимодействия

    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; // Установка максимальной скорости ходьбы
}

// Вызывается, когда игра начинается или когда объект создается
void ASandboxCharacter::BeginPlay() {
    Super::BeginPlay(); // Вызов функции BeginPlay родительского класса

    CurrentPlayerView = InitialView; // Установка текущего вида

    if (InitialView == PlayerView::TOP_DOWN) { // Если начальный вид - вид сверху
        InitTopDownView(); // Инициализация вида сверху
    }

    if (InitialView == PlayerView::THIRD_PERSON) { // Если начальный вид - вид от третьего лица
        InitThirdPersonView(); // Инициализация вида от третьего лица
    }

    if (InitialView == PlayerView::FIRST_PERSON) { // Если начальный вид - вид от первого лица
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

// Вызывается каждый кадр
void ASandboxCharacter::Tick(float DeltaTime) {
    Super::Tick(DeltaTime); // Вызов функции Tick родительского класса

    if (IsDead()) { // Если персонаж мертв
        FVector MeshLoc = GetMesh()->GetSocketLocation(TEXT("pelvis")); // Получение местоположения таза
        //GetCapsuleComponent()->SetWorldLocation(MeshLoc - InitialMeshTransform.GetLocation());
    }
}

// Настройка ввода игрока
void ASandboxCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
    Super::SetupPlayerInputComponent(InputComponent); // Вызов функции SetupPlayerInputComponent родительского класса

    //PlayerInputComponent->BindAction("ZoomIn", IE_Released, this, &ASandboxCharacter::ZoomIn);
    //PlayerInputComponent->BindAction("ZoomOut", IE_Released, this, &ASandboxCharacter::ZoomOut);

    PlayerInputComponent->BindAction("Boost", IE_Pressed, this, &ASandboxCharacter::BoostOn); // Привязка действия ускорения
    PlayerInputComponent->BindAction("Boost", IE_Released, this, &ASandboxCharacter::BoostOff); // Привязка действия отключения ускорения

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASandboxCharacter::Jump); // Привязка действия прыжка
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASandboxCharacter::StopJumping); // Привязка действия остановки прыжка

    PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ASandboxCharacter::MoveForward); // Привязка оси движения вперед/назад
    PlayerInputComponent->BindAxis("Move Right / Left", this, &ASandboxCharacter::MoveRight); // Привязка оси движения вправо/влево

    //InputComponent->BindAction("Test", IE_Pressed, this, &ASandboxCharacter::Test);

    // У нас есть 2 версии привязок поворота для обработки разных типов устройств
    // "turn" обрабатывает устройства, предоставляющие абсолютную дельту, такие как мышь.
    // "turnrate" предназначен для устройств, которые мы выбираем для обработки как скорость изменения, такие как аналоговый джойстик
    PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &ASandboxCharacter::AddControllerYawInput); // Привязка оси поворота мышью вправо/влево
    //PlayerInputComponent->BindAxis("TurnRate", this, &ASandboxCharacter::TurnAtRate);
    PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &ASandboxCharacter::AddControllerPitchInput); // Привязка оси поворота мышью вверх/вниз
    //PlayerInputComponent->BindAxis("LookUpRate", this, &ASandboxCharacter::LookUpAtRate);
}

// Проверка, может ли персонаж двигаться
bool ASandboxCharacter::CanMove() {
    //ASandboxPlayerController* C = Cast<ASandboxPlayerController>(GetController());
    //if (!C || C->IsGameInputBlocked()) {
    //	return false;
    //}

    return true; // По умолчанию вернуть true
}

// Включение ускорения
void ASandboxCharacter::BoostOn() {
    if (!CanMove()) { // Если персонаж не может двигаться
        return;
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

// Отключение ускорения
void ASandboxCharacter::BoostOff() {
    if (!CanMove()) { // Если персонаж не может двигаться
        return;
    }

    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; // Установка максимальной скорости ходьбы
}

// Прыжок
void ASandboxCharacter::Jump() {
    if (!CanMove()) { // Если персонаж не может двигаться
        return;
    }

    if (CurrentPlayerView == PlayerView::TOP_DOWN) { // Если текущий вид - вид сверху
        return;
    }

    if (IsDead()) { // Если персонаж мертв
        return;
    }

    Super::Jump(); // Вызов функции Jump родительского класса
}

// Остановка прыжка
void ASandboxCharacter::StopJumping() {
    if (IsDead()) { // Если персонаж мертв
        return;
    }

    Super::StopJumping(); // Вызов функции StopJumping родительского класса
}

// Увеличение
void ASandboxCharacter::ZoomIn() {
    if (!CanMove()) { // Если персонаж не может двигаться
        return;
    }

    if (GetCameraBoom() == NULL) { // Если камера не существует
        return;
    }

    if (CurrentPlayerView == PlayerView::FIRST_PERSON) { // Если текущий вид - вид от первого лица
        return;
    }

    if (GetCameraBoom()->TargetArmLength > MinZoom) { // Если текущая длина руки камеры больше минимального увеличения
        GetCameraBoom()->TargetArmLength -= ZoomStep; // Уменьшение длины руки камеры
    } else {
        if (bEnableAutoSwitchView) { // Если разрешено автоматическое переключение вида
            InitFirstPersonView(); // Инициализация вида от первого лица
        }
    }

    //UE_LOG(LogVt, Log, TEXT("ZoomIn: %f"), GetCameraBoom()->TargetArmLength);
}

// Уменьшение
void ASandboxCharacter::ZoomOut() {
    if (!CanMove()) { // Если персонаж не может двигаться
        return;
    }

    if (GetCameraBoom() == NULL) return; // Если камера не существует

    if (CurrentPlayerView == PlayerView::FIRST_PERSON) { // Если текущий вид - вид от первого лица
        if (bEnableAutoSwitchView) { // Если разрешено автоматическое переключение вида
            InitThirdPersonView(); // Инициализация вида от третьего лица
            return;
        }
    };

    float MZ = (CurrentPlayerView == PlayerView::TOP_DOWN) ? MaxZoomTopDown : MaxZoom; // Установка максимального увеличения в зависимости от текущего вида

    if (GetCameraBoom()->TargetArmLength < MZ) { // Если текущая длина руки камеры меньше максимального увеличения
        GetCameraBoom()->TargetArmLength += ZoomStep; // Увеличение длины руки камеры
    }

    //UE_LOG(LogVt, Log, TEXT("ZoomOut: %f"), GetCameraBoom()->TargetArmLength);
}

// Получение позиции камеры для вида от третьего лица
FVector ASandboxCharacter::GetThirdPersonViewCameraPos() {
    return FVector(0, 0, 64); // Возврат позиции камеры
}

// Получение поворота камеры для вида сверху
FRotator ASandboxCharacter::GetTopDownViewCameraRot() {
    return FRotator(-50.f, 0.f, 0.f); // Возврат поворота камеры
}

// Инициализация вида сверху
void ASandboxCharacter::InitTopDownView() {
    if (IsDead()) { // Если персонаж мертв
        return;
    }

    // Настройка движения персонажа
    GetCharacterMovement()->bOrientRotationToMovement = true; // Поворот персонажа к направлению движения
    GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f); // Скорость поворота
    GetCharacterMovement()->bConstrainToPlane = true; // Ограничение движения в плоскости
    GetCharacterMovement()->bSnapToPlaneAtStart = true; // Привязка к плоскости в начале

    CameraBoom->SetUsingAbsoluteRotation(true); // Не использовать абсолютный поворот для руки камеры

    CameraBoom->SetRelativeRotation(GetTopDownViewCameraRot()); // Установка относительного поворота камеры
    CameraBoom->TargetArmLength = MaxZoomTopDown; // Установка длины руки камеры
    CameraBoom->bDoCollisionTest = false; // Не проводить тест столкновения
    CameraBoom->bUsePawnControlRotation = false; // Не использовать поворот контроллера персонажа
    CameraBoom->ProbeSize = 0; // Размер зонда
    CameraBoom->SetRelativeLocation(FVector(0, 0, 0)); // Установка относительного положения камеры

    FirstPersonCamera->Deactivate(); // Деактивация камеры от первого лица
    FollowCamera->Activate(); // Активация следующей камеры

    bUseControllerRotationYaw = false; // Не использовать поворот контроллера по рысканью

    CurrentPlayerView = PlayerView::TOP_DOWN; // Установка текущего вида как вид сверху

    /*
    ASandboxPlayerController* C = Cast<ASandboxPlayerController>(GetController());
    if (C != NULL) {
        C->ShowMouseCursor(true);
    }
    */
}

// Инициализация вида от третьего лица
void ASandboxCharacter::InitThirdPersonView() {
    // Настройка движения персонажа
    GetCharacterMovement()->bOrientRotationToMovement = true; // Персонаж движется в направлении ввода
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // Скорость поворота
    //GetCharacterMovement()->JumpZVelocity = 600.f;
    //GetCharacterMovement()->AirControl = 0.2f;

    CameraBoom->TargetArmLength = 300.0f; // Камера следует на этом расстоянии позади персонажа
    CameraBoom->bUsePawnControlRotation = true; // Поворот руки на основе контроллера
    CameraBoom->bDoCollisionTest = true; // Проведение теста столкновения
    CameraBoom->ProbeSize = 12; // Размер зонда
    CameraBoom->SetRelativeLocation(GetThirdPersonViewCameraPos()); // Установка относительного положения камеры

    FirstPersonCamera->Deactivate(); // Деактивация камеры от первого лица
    FollowCamera->Activate(); // Активация следующей камеры

    bUseControllerRotationYaw = false; // Не использовать поворот контроллера по рысканью

    CurrentPlayerView = PlayerView::THIRD_PERSON; // Установка текущего вида как вид от третьего лица

    /*
    ASandboxPlayerController* Controller = Cast<ASandboxPlayerController>(GetController());
    if (Controller) {
        Controller->ShowMouseCursor(false);
    }
    */
}

// Инициализация вида от первого лица
void ASandboxCharacter::InitFirstPersonView() {
    if (IsDead()) { // Если персонаж мертв
        return;
    }

    // Настройка движения персонажа
    GetCharacterMovement()->bOrientRotationToMovement = false; // Персонаж движется в направлении ввода
    //GetCharacterMovement()->JumpZVelocity = 600.f;
    //GetCharacterMovement()->AirControl = 0.2f;

    FirstPersonCamera->Activate(); // Активация камеры от первого лица
    FollowCamera->Deactivate(); // Деактивация следующей камеры

    bUseControllerRotationYaw = true; // Использовать поворот контроллера по рысканью

    CurrentPlayerView = PlayerView::FIRST_PERSON; // Установка текущего вида как вид от первого лица

    /*
    ASandboxPlayerController* Controller = Cast<ASandboxPlayerController>(GetController());
    if (Controller) {
        Controller->ShowMouseCursor(false);
    }
    */
}

// Добавление ввода контроллера по рысканью
void ASandboxCharacter::AddControllerYawInput(float Val) {
    if (!CanMove()) { // Если персонаж не может двигаться
        return;
    }

    //if (Controller->IsGameInputBlocked() && CurrentPlayerView != PlayerView::THIRD_PERSON) {
        //return;
    //}

    if (CurrentPlayerView == PlayerView::TOP_DOWN) { // Если текущий вид - вид сверху
        return;
    }

    Super::AddControllerYawInput(Val); // Вызов функции AddControllerYawInput родительского класса

}

// Добавление ввода контроллера по тангажу
void ASandboxCharacter::AddControllerPitchInput(float Val) {
    if (!CanMove()) { // Если персонаж не может двигаться
        return;
    }

    /*
    ASandboxPlayerController* Controller = Cast<ASandboxPlayerController>(GetController());
    if (Controller->IsGameInputBlocked() && CurrentPlayerView != PlayerView::THIRD_PERSON) {
        //return;
    }
    */

    if (CurrentPlayerView == PlayerView::TOP_DOWN){ // Если текущий вид - вид сверху
        return;
    }

    Super::AddControllerPitchInput(Val); // Вызов функции AddControllerPitchInput родительского класса
}

// Поворот с определенной скоростью
void ASandboxCharacter::TurnAtRate(float Rate) {
    if (!CanMove()) { // Если персонаж не может двигаться
        return;
    }

    if (CurrentPlayerView == PlayerView::TOP_DOWN) { // Если текущий вид - вид сверху
        return;
    }

    // calculate delta for this frame from the rate information
    //AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

// Поднятие взгляда с определенной скоростью
void ASandboxCharacter::LookUpAtRate(float Rate) {
    if (!CanMove()) { // Если персонаж не может двигаться
        return;
    }

    if (CurrentPlayerView == PlayerView::TOP_DOWN) { // Если текущий вид - вид сверху
        return;
    }

    // calculate delta for this frame from the rate information
    //AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

// Движение вперед
void ASandboxCharacter::MoveForward(float Value) {
    if (!CanMove()) { // Если персонаж не может двигаться
        return;
    }

    if (IsDead()) { // Если персонаж мертв
        return;
    };

    if (CurrentPlayerView == PlayerView::THIRD_PERSON) { // Если текущий вид - вид от третьего лица
        if (Value != 0.0f) {
            // определить, какое направление вперед
            const FRotator Rotation = Controller->GetControlRotation(); // Получение поворота контроллера
            const FRotator YawRotation(0, Rotation.Yaw, 0); // Получение поворота по рысканью

            // получить вектор направления вперед
            const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // Получение единичного вектора оси X
            AddMovementInput(Direction, Value); // Добавление ввода движения
        }
    }

    if (CurrentPlayerView == PlayerView::FIRST_PERSON) { // Если текущий вид - вид от первого лица
        if (Value != 0.0f) {
            // добавить движение в этом направлении
            AddMovementInput(GetActorForwardVector(), Value); // Добавление ввода движения
        }
    }
}

// Движение вправо
void ASandboxCharacter::MoveRight(float Value) {
    if (!CanMove()) { // Если персонаж не может двигаться
        return;
    }

    if (IsDead()) { // Если персонаж мертв
        return;
    };

    if (CurrentPlayerView == PlayerView::THIRD_PERSON) { // Если текущий вид - вид от третьего лица
        if (Value != 0.0f) {
            // определить, какое направление вправо
            const FRotator Rotation = Controller->GetControlRotation(); // Получение поворота контроллера
            const FRotator YawRotation(0, Rotation.Yaw, 0); // Получение поворота по рысканью

            // получить вектор направления вправо
            const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); // Получение единичного вектора оси Y
            // добавить движение в этом направлении
            AddMovementInput(Direction, Value); // Добавление ввода движения
        }
    }

    if (CurrentPlayerView == PlayerView::FIRST_PERSON) { // Если текущий вид - вид от первого лица
        if (Value != 0.0f) {
            // добавить движение в этом направлении
            AddMovementInput(GetActorRightVector(), Value); // Добавление ввода движения
        }
    }
}

// Получение текущего вида игрока
PlayerView ASandboxCharacter::GetSandboxPlayerView() {
    return CurrentPlayerView; // Возврат текущего вида
}

// Установка текущего вида игрока
void ASandboxCharacter::SetSandboxPlayerView(PlayerView SandboxView) {
    CurrentPlayerView = SandboxView; // Установка текущего вида
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

// Обработка смерти персонажа
void ASandboxCharacter::OnDeath() {
    GetMesh()->SetSimulatePhysics(true); // Включение симуляции физики для сетки
    GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(TEXT("pelvis"), 1); // Установка веса смешивания физики для всех тел ниже таза
}

// Убийство персонажа
void ASandboxCharacter::Kill() {
    if (!IsDead()) { // Если персонаж не мертв
        if (CurrentPlayerView == PlayerView::FIRST_PERSON) { // Если текущий вид - вид от первого лица
            InitThirdPersonView(); // Инициализация вида от третьего лица
        }

        InitialMeshTransform = GetMesh()->GetRelativeTransform(); // Получение начального преобразования сетки
        bIsDead = true; // Установка флага смерти

        OnDeath(); // Вызов функции OnDeath
    }
}

// Воскрешение персонажа
void ASandboxCharacter::LiveUp() {
    if (IsDead()) { // Если персонаж мертв
        GetMesh()->SetSimulatePhysics(false); // Отключение симуляции физики для сетки
        bIsDead = false; // Сброс флага смерти
        GetMesh()->SetupAttachment(GetCapsuleComponent(), NAME_None); // Установка прикрепления сетки
        GetMesh()->SetRelativeTransform(InitialMeshTransform); // Установка начального преобразования сетки
    }
}

// Получение идентификатора типа песочницы
int ASandboxCharacter::GetSandboxTypeId() {
    return SandboxTypeId; // Возврат идентификатора типа песочницы
}

// Получение идентификатора пользователя игрока песочницы
FString ASandboxCharacter::GetSandboxPlayerUid() {
    return ""; // Возврат пустой строки
}

// Обработка удара
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
