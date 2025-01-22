// Fill out your copyright notice in the Description page of Project Settings.

#pragma once // Защита от множественного включения этого заголовочного файла

#include "EngineMinimal.h" // Подключение минимального заголовочного файла движка Unreal Engine
#include "GameFramework/Character.h" // Подключение заголовочного файла для класса персонажа
#include "Runtime/UMG/Public/UMG.h" // Подключение заголовочного файла для UMG (Unreal Motion Graphics)
#include "SlateBasics.h" // Подключение базовых компонентов Slate
#include "Perception/AIPerceptionComponent.h" // Подключение заголовочного файла для компонента восприятия AI
#include "SandboxCharacter.generated.h" // Генерация кода для этого заголовочного файла

class UVitalSystemComponent; // Предварительное объявление класса UVitalSystemComponent

// Перечисление для различных видов обзора игрока
UENUM(BlueprintType) 
enum class PlayerView : uint8 {
	TOP_DOWN = 0 UMETA(DisplayName = "Top Down"), // Вид сверху
	THIRD_PERSON = 1 UMETA(DisplayName = "Third Person"), // Третий человек
	FIRST_PERSON = 2 UMETA(DisplayName = "First Person") // Первый человек
};

// Интерфейс для основного персонажа песочницы
UINTERFACE(MinimalAPI, Blueprintable) 
class USandboxCoreCharacter : public UInterface { 
	GENERATED_BODY() 
};

class ISandboxCoreCharacter { 
	GENERATED_BODY()

public:
	virtual int GetSandboxTypeId() = 0; // Чисто виртуальный метод для получения идентификатора типа песочницы

	virtual FString GetSandboxPlayerUid() = 0; // Чисто виртуальный метод для получения уникального идентификатора игрока

	virtual float GetStaminaTickDelta() { return 0; } // Метод для получения изменения выносливости (по умолчанию 0)

	virtual void OnStaminaExhausted() { } // Метод для обработки события исчерпания выносливости

	virtual void Kill() { } // Метод для обработки смерти персонажа

	virtual bool IsDead() { return false; } // Метод для проверки, мертв ли персонаж (по умолчанию false)

	FString GetParam(const FString& Param) { return (BaseParamMap.Contains(Param)) ? BaseParamMap[Param] : TEXT(""); } 
    // Метод для получения параметра из карты параметров

	void SetParam(const FString& Param, const FString& Val) { BaseParamMap.Add(Param, Val); OnSetSandboxBaseParam(); } 
    // Метод для установки параметра в карте параметров и вызова обработчика

	virtual void OnSetSandboxBaseParam() { } // Метод, вызываемый при установке базового параметра песочницы

	int GetState() { return State; } // Метод для получения состояния персонажа

protected:
	int State = 0; // Переменная состояния персонажа

private:
	TMap<FString, FString> BaseParamMap; // Карта базовых параметров персонажа
};

// Класс персонажа песочницы
UCLASS() 
class UNREALSANDBOXTOOLKIT_API ASandboxCharacter : public ACharacter, public ISandboxCoreCharacter { 
	GENERATED_BODY()

public:
	ASandboxCharacter(); // Конструктор класса

	virtual void BeginPlay() override; // Переопределение метода BeginPlay для инициализации при старте игры
	
	virtual void Tick(float DeltaSeconds) override; // Переопределение метода Tick для обновления состояния персонажа каждый кадр

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override; 
    // Переопределение метода для настройки ввода игрока

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Core Character") 
	int SandboxTypeId = 0; // Идентификатор типа песочницы (редактируемый в редакторе)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true")) 
	class UCameraComponent* FollowCamera; // Камера слежения за персонажем

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true")) 
	class UCameraComponent* FirstPersonCamera; // Камера первого лица

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true")) 
	class USpringArmComponent* CameraBoom; // Компонент пружинного рычага для управления расстоянием камеры

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; } // Встроенный метод для получения компонента пружинного рычага

	UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character") 
	void InitTopDownView(); // Метод инициализации вида сверху

	UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character") 
	void InitThirdPersonView(); // Метод инициализации третьего лица

	UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character") 
	void InitFirstPersonView(); // Метод инициализации первого лица

	PlayerView GetSandboxPlayerView(); // Метод для получения текущего вида игрока

	void SetSandboxPlayerView(PlayerView SandboxView); // Метод для установки вида игрока

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 
	PlayerView InitialView = PlayerView::TOP_DOWN; // Начальный вид игрока (по умолчанию сверху)

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 
	bool bEnableAutoSwitchView = true; // Флаг автоматического переключения видов

	bool IsDead() { return bIsDead; } // Метод проверки состояния смерти персонажа

	UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character") 
	void Kill(); // Метод обработки смерти персонажа

	UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character") 
	void LiveUp(); // Метод восстановления жизни персонажа

	void Jump() override; // Переопределение метода прыжка 

	void StopJumping() override; // Переопределение метода остановки прыжка 

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 
	float MaxZoom; // Максимальное приближение камеры 

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 
	float MaxZoomTopDown; // Максимальное приближение камеры в режиме сверху 

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 
	float MinZoom; // Минимальное удаление камеры 

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 
	float ZoomStep; // Шаг изменения зума 

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 
	float WalkSpeed; // Скорость ходьбы персонажа 

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 
	float RunSpeed; // Скорость бега персонажа 

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 
	float InteractionTargetLength; // Длина взаимодействия с объектами 

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 
	float VelocityHitThreshold; // Порог скорости удара 

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 
	float VelocityHitFactor; // Коэффициент воздействия скорости удара 

	double VelocityHitTimestamp; // Временная метка удара по скорости 

	void BoostOn();  // Метод активации ускорения 

	void BoostOff();  // Метод деактивации ускорения 

	int GetSandboxTypeId() override;  // Переопределение метода получения идентификатора типа песочницы 

	FString GetSandboxPlayerUid() override;  // Переопределение метода получения уникального идентификатора игрока 

	template<class T>
	T* GetFirstComponentByName(FString ComponentName) {  // Шаблонный метод для получения первого компонента по имени
		TArray<T*> Components;
		GetComponents<T>(Components);  // Получение всех компонентов указанного типа
		for (T* Component : Components) {
			if (Component->GetName() == ComponentName)  // Сравнение имени компонента с заданным именем
				return Component;
		}

		return nullptr;  // Возвращает nullptr если компонент не найден
	}

private:
	PlayerView CurrentPlayerView;  // Текущий вид игрока 

	bool bIsDead = false;  // Флаг состояния смерти персонажа 

	FTransform InitialMeshTransform;  // Начальная трансформация меша персонажа 

	UFUNCTION()
	void OnHit(class UPrimitiveComponent* HitComp, class AActor* Actor, class UPrimitiveComponent* Other, FVector Impulse, const FHitResult & HitResult);  
    /* Обработчик события удара */

	//UPROPERTY()
	//UVitalSystemComponent* VitalSystemComponent;

protected:
	void ZoomIn();  /* Метод увеличения зума */
	
	void ZoomOut(); /* Метод уменьшения зума */

	virtual void OnDeath(); /* Обработчик события смерти */

	/** Called for forwards/backward input */
	void MoveForward(float Value); /* Обработчик ввода вперед/назад */

	/** Called for side to side input */
	void MoveRight(float Value); /* Обработчик ввода вправо/влево */

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate); /* Обработчик ввода поворота с заданной скоростью */

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate); /* Обработчик ввода взгляда вверх/вниз с заданной скоростью */

	virtual void AddControllerYawInput(float Val) override; /* Переопределение метода добавления угла поворота контроллера */

	virtual void AddControllerPitchInput(float Val) override; /* Переопределение метода добавления угла наклона контроллера */

	virtual FVector GetThirdPersonViewCameraPos(); /* Получение позиции камеры третьего лица */

	virtual FRotator GetTopDownViewCameraRot(); /* Получение угла поворота камеры сверху */

	virtual bool CanMove(); /* Проверка возможности движения */
};
