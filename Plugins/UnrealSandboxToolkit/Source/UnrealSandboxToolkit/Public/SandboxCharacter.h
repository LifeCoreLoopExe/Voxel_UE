// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/UMG/Public/UMG.h"
#include "SlateBasics.h"
#include "Perception/AIPerceptionComponent.h"
#include "SandboxCharacter.generated.h"

// Предварительное объявление компонента жизненных показателей
class UVitalSystemComponent;

// Перечисление для различных режимов обзора камеры
UENUM(BlueprintType)
enum class PlayerView : uint8 {
	TOP_DOWN = 0		UMETA(DisplayName = "Top Down"),     // Вид сверху
	THIRD_PERSON = 1	UMETA(DisplayName = "Third Person"), // Вид от третьего лица
	FIRST_PERSON = 2	UMETA(DisplayName = "First Person")  // Вид от первого лица
};

// Интерфейс для базового функционала персонажа
UINTERFACE(MinimalAPI, Blueprintable)
class USandboxCoreCharacter : public UInterface {
	GENERATED_BODY()
};

// Основной интерфейс с базовыми методами для персонажа
class ISandboxCoreCharacter {
	GENERATED_BODY()

public:
	// Получить ID типа персонажа
	virtual int GetSandboxTypeId() = 0;

	// Получить уникальный ID игрока
	virtual FString GetSandboxPlayerUid() = 0;

	// Получить изменение выносливости за тик
	virtual float GetStaminaTickDelta() { return 0; }

	// Действие при истощении выносливости
	virtual void OnStaminaExhausted() { }

	// Убить персонажа
	virtual void Kill() { }

	// Проверить, мертв ли персонаж
	virtual bool IsDead() { return false; }

	// Получить значение параметра по ключу
	FString GetParam(const FString& Param) { return (BaseParamMap.Contains(Param)) ? BaseParamMap[Param] : TEXT(""); }

	// Установить значение параметра
	void SetParam(const FString& Param, const FString& Val) { BaseParamMap.Add(Param, Val); OnSetSandboxBaseParam(); }

	// Вызывается при установке базового параметра
	virtual void OnSetSandboxBaseParam() { }

	// Получить текущее состояние
	int GetState() { return State; }

protected:
	// Текущее состояние персонажа
	int State = 0;

private:
	// Карта базовых параметров персонажа
	TMap<FString, FString> BaseParamMap;
};

// Основной класс игрового персонажа
UCLASS()
class UNREALSANDBOXTOOLKIT_API ASandboxCharacter : public ACharacter, public ISandboxCoreCharacter {
	GENERATED_BODY()

public:
	// Конструктор
	ASandboxCharacter();

	// Вызывается при начале игры
	virtual void BeginPlay() override;
	
	// Вызывается каждый кадр
	virtual void Tick( float DeltaSeconds ) override;

	// Настройка пользовательского ввода
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// ID типа персонажа
	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Core Character")
	int SandboxTypeId = 0;

	// Компоненты камеры
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// Получить компонент пружинного рычага камеры
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// Методы инициализации разных режимов камеры
	UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character")
	void InitTopDownView();

	UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character")
	void InitThirdPersonView();

	UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character")
	void InitFirstPersonView();

	// Методы для работы с режимом обзора
	PlayerView GetSandboxPlayerView();
	void SetSandboxPlayerView(PlayerView SandboxView);

	// Начальный режим обзора
	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
	PlayerView InitialView = PlayerView::TOP_DOWN;

	// Включить автоматическое переключение вида
	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
	bool bEnableAutoSwitchView = true;

	// Методы для работы с состоянием жизни
	bool IsDead() { return bIsDead; }

	UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character")
	void Kill();

	UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character")
	void LiveUp();

	// Переопределение методов прыжка
	void Jump() override;
	void StopJumping() override;

	// Настройки камеры и движения
	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
	float MaxZoom;

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
	float MaxZoomTopDown;

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
	float MinZoom;

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
	float ZoomStep;

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
	float RunSpeed;

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
	float InteractionTargetLength;

	// Настройки обработки столкновений
	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
	float VelocityHitThreshold;

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
	float VelocityHitFactor;

	double VelocityHitTimestamp;

	// Методы для управления ускорением
	void BoostOn();
	void BoostOff();

	// Реализация методов интерфейса
	int GetSandboxTypeId() override;
	FString GetSandboxPlayerUid() override;

	// Шаблонный метод для поиска компонента по имени
	template<class T>
	T* GetFirstComponentByName(FString ComponentName) {
		TArray<T*> Components;
		GetComponents<T>(Components);
		for (T* Component : Components) {
			if (Component->GetName() == ComponentName)
				return Component;
		}
		return nullptr;
	}

private:
	// Текущий режим обзора
	PlayerView CurrentPlayerView;

	// Флаг смерти персонажа
	bool bIsDead = false;

	// Начальная трансформация меша
	FTransform InitialMeshTransform;

	// Обработчик столкновений
	UFUNCTION()
	void OnHit(class UPrimitiveComponent* HitComp, class AActor* Actor, class UPrimitiveComponent* Other, FVector Impulse, const FHitResult & HitResult);

protected:
	// Методы для управления камерой
	void ZoomIn();
	void ZoomOut();

	// Метод, вызываемый при смерти
	virtual void OnDeath();

	// Методы управления движением
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	// Переопределение методов управления камерой
	virtual void AddControllerYawInput(float Val) override;
	virtual void AddControllerPitchInput(float Val) override;

	// Методы для настройки камеры
	virtual FVector GetThirdPersonViewCameraPos();
	virtual FRotator GetTopDownViewCameraRot();

	// Проверка возможности движения
	virtual bool CanMove();
};
