// Заполните ваше уведомление о копирайте на странице описания настроек проекта.

#include "SandboxCharacter.h"
//#include "SandboxPlayerController.h"
//#include "VitalSystemComponent.h"


ASandboxCharacter::ASandboxCharacter() {
	VelocityHitThreshold = 1300; // Порог скорости удара
	VelocityHitFactor = 0.2f; // Фактор скорости удара
	VelocityHitTimestamp = 0; // Время последнего удара

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f); // Инициализация размера капсулы

	bUseControllerRotationPitch = false; // Не использовать вращение по оси Pitch от контроллера
	bUseControllerRotationYaw = false; // Не использовать вращение по оси Yaw от контроллера
	bUseControllerRotationRoll = false; // Не использовать вращение по оси Roll от контроллера

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom")); // Создание компонента SpringArm для камеры
	CameraBoom->SetupAttachment(RootComponent); // Подключение к корневому компоненту

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera")); // Создание компонента камеры
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Подключение камеры к концу SpringArm
	FollowCamera->bUsePawnControlRotation = false; // Камера не вращается относительно SpringArm
	FollowCamera->SetRelativeLocation(FVector(0, 0, 0)); // Установка позиции камеры

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera")); // Создание компонента для камеры от первого лица
	FirstPersonCamera->SetupAttachment(GetMesh(), TEXT("head")); // Подключение к мешу персонажа
	FirstPersonCamera->SetRelativeLocation(FVector(10.0f, 32.0f, 0.f)); // Установка позиции камеры
	FirstPersonCamera->SetRelativeRotation(FRotator(0, 90, -90)); // Установка вращения камеры
	FirstPersonCamera->bUsePawnControlRotation = true; // Камера вращается с контроллером

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASandboxCharacter::OnHit); // Обработка события удара

	// начальный вид
	CurrentPlayerView = PlayerView::TOP_DOWN; // Установка начального вида
	InitTopDownView(); // Инициализация верхнего вида

	PrimaryActorTick.bCanEverTick = true; // Разрешение тиков для актора
	PrimaryActorTick.bStartWithTickEnabled = true; // Начать с включенными тиками

	MaxZoom = 500; // Максимальное увеличение
	MaxZoomTopDown = 1200; // Максимальное увеличение для верхнего вида
	MinZoom = 100; // Минимальное увеличение
	ZoomStep = 50; // Шаг увеличения

	WalkSpeed = 200; // Скорость ходьбы
	RunSpeed = 600; // Скорость бега

	InteractionTargetLength = 200; // Длина цели взаимодействия

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; // Установка максимальной скорости ходьбы
}

void ASandboxCharacter::BeginPlay() {
	Super::BeginPlay(); // Вызов базового метода BeginPlay
	
	CurrentPlayerView = InitialView; // Установка текущего вида на начальный

	if (InitialView == PlayerView::TOP_DOWN) { // Проверка на верхний вид
		InitTopDownView(); // Инициализация верхнего вида
	}

	if (InitialView == PlayerView::THIRD_PERSON) { // Проверка на третий вид
		InitThirdPersonView(); // Инициализация третьего вида
	}

	if (InitialView == PlayerView::FIRST_PERSON) { // Проверка на первый вид
		InitFirstPersonView(); // Инициализация первого вида
	}

	/*
	TArray<UVitalSystemComponent*> Components; // Массив компонентов VitalSystem
	GetComponents<UVitalSystemComponent>(Components); // Получение всех компонентов VitalSystem

	for (UVitalSystemComponent* VitalSysCmp : Components) { // Перебор компонентов
		VitalSystemComponent = VitalSysCmp; // Установка компонента VitalSystem
		break; // Выход из цикла после первой итерации
	}
	*/
}

void ASandboxCharacter::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime ); // Вызов базового метода Tick

	if (IsDead()) { // Проверка на смерть
		FVector MeshLoc = GetMesh()->GetSocketLocation(TEXT("pelvis")); // Получение позиции меша
		//GetCapsuleComponent()->SetWorldLocation(MeshLoc - InitialMeshTransform.GetLocation()); // Установка позиции капсулы
	}
}

void ASandboxCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(InputComponent); // Вызов базового метода настройки ввода игрока

	//PlayerInputComponent->BindAction("ZoomIn", IE_Released, this, &ASandboxCharacter::ZoomIn); // Привязка действия увеличения
	//PlayerInputComponent->BindAction("ZoomOut", IE_Released, this, &ASandboxCharacter::ZoomOut); // Привязка действия уменьшения

	PlayerInputComponent->BindAction("Boost", IE_Pressed, this, &ASandboxCharacter::BoostOn); // Привязка действия ускорения
	PlayerInputComponent->BindAction("Boost", IE_Released, this, &ASandboxCharacter::BoostOff); // Привязка действия прекращения ускорения

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASandboxCharacter::Jump); // Привязка действия прыжка
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASandboxCharacter::StopJumping); // Привязка действия прекращения прыжка

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ASandboxCharacter::MoveForward); // Привязка оси движения вперед/назад
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ASandboxCharacter::MoveRight); // Привязка оси движения вправо/влево

	//InputComponent->BindAction("Test", IE_Pressed, this, &ASandboxCharacter::Test); // Привязка тестового действия

	// У нас есть 2 версии привязок поворота для обработки различных типов устройств по-разному
	// "turn" обрабатывает устройства, которые предоставляют абсолютный дельта, такие как мышь.
	// "turnrate" предназначен для устройств, которые мы выбираем рассматривать как скорость изменения, такие как аналоговый джойстик
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &ASandboxCharacter::AddControllerYawInput); // Привязка оси поворота
	//PlayerInputComponent->BindAxis("TurnRate", this, &ASandboxCharacter::TurnAtRate); // Привязка оси скорости поворота
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &ASandboxCharacter::AddControllerPitchInput); // Привязка оси взгляда вверх/вниз
	//PlayerInputComponent->BindAxis("LookUpRate", this, &ASandboxCharacter::LookUpAtRate); // Привязка оси скорости взгляда вверх
}


bool ASandboxCharacter::CanMove() {
	//ASandboxPlayerController* C = Cast<ASandboxPlayerController>(GetController()); // Привязка контроллера
	//if (!C || C->IsGameInputBlocked()) { // Проверка на блокировку ввода игры
	//	return false; // Невозможность движения
	//}

	return true; // Возможность движения
}

void ASandboxCharacter::BoostOn() {
	if (!CanMove()) { // Проверка на возможность движения
		return; // Возврат, если движение невозможно
	}

	/*
	TArray<UVitalSystemComponent*> Components; // Массив компонентов VitalSystem
	GetComponents<UVitalSystemComponent>(Components); // Получение всех компонентов VitalSystem
	if (Components.Num() > 0) { // Проверка на наличие компонентов
		UVitalSystemComponent* Vs = Components[0]; // Установка первого компонента
		if (Vs->CanBoost()) { // Проверка на возможность ускорения
			GetCharacterMovement()->MaxWalkSpeed = RunSpeed; // Установка скорости бега
			return; // Возврат
		}
	} else {
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed; // Установка скорости бега
	}
	*/
}

void ASandboxCharacter::BoostOff() {
	if (!CanMove()) { // Проверка на возможность движения
		return; // Возврат, если движение невозможно
	}

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; // Установка скорости ходьбы
}

void ASandboxCharacter::Jump() {
	if (!CanMove()) { // Проверка на возможность движения
		return; // Возврат, если движение невозможно
	}

	if (CurrentPlayerView == PlayerView::TOP_DOWN) { // Проверка на верхний вид
		return; // Возврат
	}

	if (IsDead()) { // Проверка на смерть
		return; // Возврат
	}

	Super::Jump(); // Вызов базового метода прыжка
}

void ASandboxCharacter::StopJumping() {
	if (IsDead()) { // Проверка на смерть
		return; // Возврат
	}

	Super::StopJumping(); // Вызов базового метода прекращения прыжка
}


void ASandboxCharacter::ZoomIn() {
	if (!CanMove()) { // Проверка на возможность движения
		return; // Возврат, если движение невозможно
	}

	if (GetCameraBoom() == NULL) { // Проверка на наличие CameraBoom
		return; // Возврат
	}

	if (CurrentPlayerView == PlayerView::FIRST_PERSON) { // Проверка на первый вид
		return; // Возврат
	}

	if (GetCameraBoom()->TargetArmLength > MinZoom) { // Проверка на максимальное увеличение
		GetCameraBoom()->TargetArmLength -= ZoomStep; // Уменьшение длины CameraBoom
	} else {
		if (bEnableAutoSwitchView) { // Проверка на автоматическое переключение вида
			InitFirstPersonView(); // Инициализация первого вида
		}
	}

	//UE_LOG(LogVt, Log, TEXT("ZoomIn: %f"), GetCameraBoom()->TargetArmLength); // Логирование увеличения
}

void ASandboxCharacter::ZoomOut() {
	if (!CanMove()) { // Проверка на возможность движения
		return; // Возврат, если движение невозможно
	}

	if (GetCameraBoom() == NULL) return; // Проверка на наличие CameraBoom

	if (CurrentPlayerView == PlayerView::FIRST_PERSON) { // Проверка на первый вид
		if (bEnableAutoSwitchView) { // Проверка на автоматическое переключение вида
			InitThirdPersonView(); // Инициализация третьего вида
			return; // Возврат
		}
	};

	float MZ = (CurrentPlayerView == PlayerView::TOP_DOWN) ? MaxZoomTopDown : MaxZoom; // Установка максимального увеличения

	if (GetCameraBoom()->TargetArmLength < MZ) { // Проверка на максимальное увеличение
		GetCameraBoom()->TargetArmLength += ZoomStep; // Увеличение длины CameraBoom
	}

	//UE_LOG(LogVt, Log, TEXT("ZoomOut: %f"), GetCameraBoom()->TargetArmLength); // Логирование уменьшения
}

FVector ASandboxCharacter::GetThirdPersonViewCameraPos() {
	return FVector(0, 0, 64); // Позиция камеры для третьего вида
}

FRotator ASandboxCharacter::GetTopDownViewCameraRot() {
	return FRotator(-50.f, 0.f, 0.f); // Вращение камеры для верхнего вида
}

void ASandboxCharacter::InitTopDownView() {
	if (IsDead()) { // Проверка на смерть
		return; // Возврат
	}

	// Настройка движения персонажа
	GetCharacterMovement()->bOrientRotationToMovement = true; // Вращение персонажа в сторону движения
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f); // Установка скорости вращения
	GetCharacterMovement()->bConstrainToPlane = true; // Ограничение движения по плоскости
	GetCharacterMovement()->bSnapToPlaneAtStart = true; // Привязка к плоскости в начале

	CameraBoom->SetUsingAbsoluteRotation(true); // Не вращать CameraBoom при вращении персонажа

	CameraBoom->SetRelativeRotation(GetTopDownViewCameraRot()); // Установка вращения CameraBoom
	CameraBoom->TargetArmLength = MaxZoomTopDown; // Установка длины CameraBoom
	CameraBoom->bDoCollisionTest = false; // Не проверять столкновения для CameraBoom
	CameraBoom->bUsePawnControlRotation = false; // Вращать CameraBoom по контроллеру
	CameraBoom->ProbeSize = 0; // Установка размера проверки
	CameraBoom->SetRelativeLocation(FVector(0, 0, 0)); // Установка позиции CameraBoom

	FirstPersonCamera->Deactivate(); // Деактивация камеры от первого лица
	FollowCamera->Activate(); // Активация основной камеры

	bUseControllerRotationYaw = false; // Не использовать вращение по оси Yaw от контроллера

	CurrentPlayerView = PlayerView::TOP_DOWN; // Установка текущего вида на верхний

	/*
	ASandboxPlayerController* C = Cast<ASandboxPlayerController>(GetController()); // Привязка контроллера
	if (C != NULL) { // Проверка на наличие контроллера
		C->ShowMouseCursor(true); // Показать курсор мыши
	}
	*/
}

void ASandboxCharacter::InitThirdPersonView() {
	// Настройка движения персонажа
	GetCharacterMovement()->bOrientRotationToMovement = true; // Персонаж движется в направлении ввода
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // Установка скорости вращения
	//GetCharacterMovement()->JumpZVelocity = 600.f; // Установка скорости прыжка
	//GetCharacterMovement()->AirControl = 0.2f; // Установка контроля в воздухе

	CameraBoom->TargetArmLength = 300.0f; // Установка расстояния камеры за персонажем
	CameraBoom->bUsePawnControlRotation = true; // Вращение CameraBoom по контроллеру
	CameraBoom->bDoCollisionTest = true; // Проверка столкновений для CameraBoom
	CameraBoom->ProbeSize = 12; // Установка размера проверки
	CameraBoom->SetRelativeLocation(GetThirdPersonViewCameraPos()); // Установка позиции CameraBoom

	FirstPersonCamera->Deactivate(); // Деактивация камеры от первого лица
	FollowCamera->Activate(); // Активация основной камеры

	bUseControllerRotationYaw = false; // Не использовать вращение по оси Yaw от контроллера

	CurrentPlayerView = PlayerView::THIRD_PERSON; // Установка текущего вида на третий

	/*
	ASandboxPlayerController* Controller = Cast<ASandboxPlayerController>(GetController()); // Привязка контроллера
	if (Controller) { // Проверка на наличие контроллера
		Controller->ShowMouseCursor(false); // Скрыть курсор мыши
	}
	*/
}

void ASandboxCharacter::InitFirstPersonView() {
	if (IsDead()) { // Проверка на смерть
		return; // Возврат
	}

	// Настройка движения персонажа
	GetCharacterMovement()->bOrientRotationToMovement = false; // Персонаж движется в направлении ввода
	//GetCharacterMovement()->JumpZVelocity = 600.f; // Установка скорости прыжка
	//GetCharacterMovement()->AirControl = 0.2f; // Установка контроля в воздухе

	FirstPersonCamera->Activate(); // Активация камеры от первого лица
	FollowCamera->Deactivate(); // Деактивация основной камеры

	bUseControllerRotationYaw = true; // Использовать вращение по оси Yaw от контроллера

	CurrentPlayerView = PlayerView::FIRST_PERSON; // Установка текущего вида на первый

	/*
	ASandboxPlayerController* Controller = Cast<ASandboxPlayerController>(GetController()); // Привязка контроллера
	if (Controller) { // Проверка на наличие контроллера
		Controller->ShowMouseCursor(false); // Скрыть курсор мыши
	}
	*/
}

void ASandboxCharacter::AddControllerYawInput(float Val) {
	if (!CanMove()) { // Проверка на возможность движения
		return; // Возврат
	}

	//if (Controller->IsGameInputBlocked() && CurrentPlayerView != PlayerView::THIRD_PERSON) { // Проверка на блокировку ввода игры
		//return; // Возврат
	//}

	if (CurrentPlayerView == PlayerView::TOP_DOWN) { // Проверка на верхний вид
		return; // Возврат
	}

	Super::AddControllerYawInput(Val); // Вызов базового метода добавления вращения по оси Yaw
}

void ASandboxCharacter::AddControllerPitchInput(float Val) {
	if (!CanMove()) { // Проверка на возможность движения
		return; // Возврат
	}

	/*
	ASandboxPlayerController* Controller = Cast<ASandboxPlayerController>(GetController()); // Привязка контроллера
	if (Controller->IsGameInputBlocked() && CurrentPlayerView != PlayerView::THIRD_PERSON) { // Проверка на блокировку ввода игры
		//return; // Возврат
	}
	*/

	if (CurrentPlayerView == PlayerView::TOP_DOWN){ // Проверка на верхний вид
		return; // Возврат
	}

	Super::AddControllerPitchInput(Val); // Вызов базового метода добавления вращения по оси Pitch
}

void ASandboxCharacter::TurnAtRate(float Rate) {
	if (!CanMove()) { // Проверка на возможность движения
		return; // Возврат
	}

	if (CurrentPlayerView == PlayerView::TOP_DOWN) { // Проверка на верхний вид
		return; // Возврат
	}

	// вычислить дельту за этот кадр на основе информации о скорости
	//AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds()); // Добавление вращения по оси Yaw
}

void ASandboxCharacter::LookUpAtRate(float Rate) {
	if (!CanMove()) { // Проверка на возможность движения
		return; // Возврат
	}

	if (CurrentPlayerView == PlayerView::TOP_DOWN) { // Проверка на верхний вид
		return; // Возврат
	}

	// вычислить дельту за этот кадр на основе информации о скорости
	//AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds()); // Добавление вращения по оси Pitch
}


void ASandboxCharacter::MoveForward(float Value) {
	if (!CanMove()) { // Проверка на возможность движения
		return; // Возврат
	}

	if (IsDead()) { // Проверка на смерть
		return; // Возврат
	};

	if (CurrentPlayerView == PlayerView::THIRD_PERSON) { // Проверка на третий вид
		if (Value != 0.0f) { // Проверка на ненулевое значение
			// выяснить, в каком направлении вперед
			const FRotator Rotation = Controller->GetControlRotation(); // Получение вращения контроллера
			const FRotator YawRotation(0, Rotation.Yaw, 0); // Установка вращения по оси Yaw

			// получить вектор вперед
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // Получение вектора вперед
			AddMovementInput(Direction, Value); // Добавление движения в этом направлении
		}
	}

	if (CurrentPlayerView == PlayerView::FIRST_PERSON) { // Проверка на первый вид
		if (Value != 0.0f) { // Проверка на ненулевое значение
			// добавление движения в этом направлении
			AddMovementInput(GetActorForwardVector(), Value); // Добавление движения вперед
		}
	}
}

void ASandboxCharacter::MoveRight(float Value) {
	if (!CanMove()) { // Проверка на возможность движения
		return; // Возврат
	}

	if (IsDead()) { // Проверка на смерть
		return; // Возврат
	};

	if (CurrentPlayerView == PlayerView::THIRD_PERSON) { // Проверка на третий вид
		if (Value != 0.0f) { // Проверка на ненулевое значение
			// выяснить, в каком направлении вправо
			const FRotator Rotation = Controller->GetControlRotation(); // Получение вращения контроллера
			const FRotator YawRotation(0, Rotation.Yaw, 0); // Установка вращения по оси Yaw

			// получить вектор вправо 
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); // Получение вектора вправо
			// добавление движения в этом направлении
			AddMovementInput(Direction, Value); // Добавление движения вправо
		}
	}

	if (CurrentPlayerView == PlayerView::FIRST_PERSON) { // Проверка на первый вид
		if (Value != 0.0f) { // Проверка на ненулевое значение
			// добавление движения в этом направлении
			AddMovementInput(GetActorRightVector(), Value); // Добавление движения вправо
		}
	}
}

PlayerView ASandboxCharacter::GetSandboxPlayerView() {
	return CurrentPlayerView; // Возврат текущего вида игрока
}

void ASandboxCharacter::SetSandboxPlayerView(PlayerView SandboxView) {
	CurrentPlayerView = SandboxView; // Установка текущего вида игрока
}

/*
void ASandboxCharacter::Test() {
	if(!IsDead()) { // Проверка на смерть
		Kill(); // Убийство персонажа
	} else {
		LiveUp(); // Восстановление персонажа
	}
}
*/

void ASandboxCharacter::OnDeath() {
	GetMesh()->SetSimulatePhysics(true); // Включение физики для меша
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(TEXT("pelvis"), 1); // Установка веса физики для всех тел ниже "pelvis"
}

void ASandboxCharacter::Kill() {
	if (!IsDead()) { // Проверка на смерть
		if (CurrentPlayerView == PlayerView::FIRST_PERSON) { // Проверка на первый вид
			InitThirdPersonView(); // Инициализация третьего вида
		}

		InitialMeshTransform = GetMesh()->GetRelativeTransform(); // Получение начальной трансформа меша
		bIsDead = true; // Установка состояния смерти

		OnDeath(); // Вызов метода смерти
	}
}

void ASandboxCharacter::LiveUp() {
	if (IsDead()) { // Проверка на смерть
		GetMesh()->SetSimulatePhysics(false); // Выключение физики для меша
		bIsDead = false; // Установка состояния живым
		GetMesh()->SetupAttachment(GetCapsuleComponent(), NAME_None); // Подключение меша к капсуле
		GetMesh()->SetRelativeTransform(InitialMeshTransform); // Установка начальной трансформа меша
	}
}

int ASandboxCharacter::GetSandboxTypeId() {
	return SandboxTypeId; // Возврат идентификатора типа песочницы
}

FString ASandboxCharacter::GetSandboxPlayerUid() {
	return ""; // Возврат строки с UID игрока
}

void ASandboxCharacter::OnHit(class UPrimitiveComponent* HitComp, class AActor* Actor, class UPrimitiveComponent* Other, FVector Impulse, const FHitResult & HitResult) {
	/*
	float HitVelocity = GetCapsuleComponent()->GetComponentVelocity().Size(); // Получение скорости удара
	if (VitalSystemComponent != nullptr) { // Проверка на наличие компонента VitalSystem
		if (HitVelocity > VelocityHitThreshold) { // Проверка на превышение порога скорости удара
			const double Timestamp = FPlatformTime::Seconds(); // Получение текущего времени
			const double D = Timestamp - VelocityHitTimestamp; // Вычисление времени с момента последнего удара
			if (D > 0.5) { // Проверка времени
				UE_LOG(LogTemp, Log, TEXT("HitVelocity -> %f"), HitVelocity); // Логирование скорости удара
				VelocityHitTimestamp = Timestamp; // Обновление времени последнего удара
				const float Damage = (HitVelocity - VelocityHitThreshold) * VelocityHitFactor; // Расчет урона
				VitalSystemComponent->Damage(Damage); // Нанесение урона
			}

		}
	}
	*/
}
