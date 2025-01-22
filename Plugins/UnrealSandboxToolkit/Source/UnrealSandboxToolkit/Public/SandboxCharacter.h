// Fill out your copyright notice in the Description page of Project Settings. // Заполните уведомление об авторских правах на странице описания в настройках проекта.

#pragma once // Указание компилятору включить этот файл только один раз

#include "EngineMinimal.h" // Подключение минимального заголовочного файла движка Unreal Engine
#include "GameFramework/Character.h" // Подключение заголовочного файла для класса персонажа
#include "Runtime/UMG/Public/UMG.h" // Подключение заголовочного файла UMG для работы с пользовательским интерфейсом
#include "SlateBasics.h" // Подключение базовых компонентов Slate
#include "Perception/AIPerceptionComponent.h" // Подключение компонента восприятия AI
#include "SandboxCharacter.generated.h" // Генерация заголовочного файла для класса ASandboxCharacter

class UVitalSystemComponent; // Объявление класса UVitalSystemComponent

UENUM(BlueprintType) // Определение перечисления, доступного в Blueprints
enum class PlayerView : uint8 { // Определение перечисления PlayerView с типом uint8
	TOP_DOWN = 0		UMETA(DisplayName = "Top Down"), // Вид сверху, отображаемое имя "Top Down"
	THIRD_PERSON = 1	UMETA(DisplayName = "Third Person"), // Третий человек, отображаемое имя "Third Person"
	FIRST_PERSON = 2	UMETA(DisplayName = "First Person") // Первый человек, отображаемое имя "First Person"
};



UINTERFACE(MinimalAPI, Blueprintable) // Определение интерфейса, доступного в Blueprints
class USandboxCoreCharacter : public UInterface { // Определение класса интерфейса USandboxCoreCharacter
	GENERATED_BODY() // Генерация тела интерфейса
};

class ISandboxCoreCharacter { // Определение интерфейса ISandboxCoreCharacter
	GENERATED_BODY() // Генерация тела интерфейса

public:

	virtual int GetSandboxTypeId() = 0; // Чисто виртуальный метод для получения идентификатора типа песочницы

	virtual FString GetSandboxPlayerUid() = 0; // Чисто виртуальный метод для получения уникального идентификатора игрока

	virtual float GetStaminaTickDelta() { return 0; } // Метод для получения дельты выносливости (по умолчанию 0)

	virtual void OnStaminaExhausted() { } // Метод для обработки исчерпания выносливости (пустой по умолчанию)

	virtual void Kill() { } // Метод для убийства персонажа (пустой по умолчанию)

	virtual bool IsDead() { return false; } // Метод для проверки, мертв ли персонаж (по умолчанию возвращает false)

	FString GetParam(const FString& Param) { return (BaseParamMap.Contains(Param)) ? BaseParamMap[Param] : TEXT(""); } // Метод для получения параметра из карты параметров

	void SetParam(const FString& Param, const FString& Val) { BaseParamMap.Add(Param, Val); OnSetSandboxBaseParam(); } // Метод для установки параметра в карту и вызова метода обработки установки

	virtual void OnSetSandboxBaseParam() { } // Метод для обработки установки базового параметра песочницы (пустой по умолчанию)

	int GetState() { return State; } // Метод для получения состояния персонажа

protected:

	int State = 0; // Переменная состояния персонажа (по умолчанию 0)

private:

	TMap<FString, FString> BaseParamMap; // Карта базовых параметров (имя параметра -> значение параметра)

};



UCLASS() // Определение класса, доступного в Blueprints
class UNREALSANDBOXTOOLKIT_API ASandboxCharacter : public ACharacter, public ISandboxCoreCharacter { // Определение класса ASandboxCharacter, наследующего от ACharacter и ISandboxCoreCharacter
	GENERATED_BODY() // Генерация тела класса

public:

	ASandboxCharacter(); // Конструктор класса ASandboxCharacter

	virtual void BeginPlay() override; // Переопределение метода BeginPlay для инициализации при старте игры
	
	virtual void Tick(float DeltaSeconds) override; // Переопределение метода Tick для обновления состояния каждый кадр

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override; // Переопределение метода настройки ввода игрока

	//UPROPERTY(EditAnywhere, Category = "UnrealSandbox Core Character")
	//int SandboxTypeId = 0; // Закомментированное свойство идентификатора типа песочницы

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Core Character") // Свойство, редактируемое в редакторе, в категории "UnrealSandbox Core Character"
	int SandboxTypeId = 0; // Идентификатор типа песочницы



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true")) // Свойство камеры, доступное только для чтения в Blueprints
	class UCameraComponent* FollowCamera; // Камера слежения

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true")) // Свойство камеры первого лица, доступное только для чтения в Blueprints
	class UCameraComponent* FirstPersonCamera; // Камера первого лица

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true")) // Свойство пружинного рычага камеры, доступное только для чтения в Blueprints
	class USpringArmComponent* CameraBoom; // Компонент пружинного рычага камеры

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; } // Метод для получения компонента пружинного рычага камеры

	UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character") // Метод, доступный из Blueprints, в категории "UnrealSandbox Character"
	void InitTopDownView(); // Метод инициализации вида сверху

	UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character") // Метод, доступный из Blueprints, в категории "UnrealSandbox Character"
	void InitThirdPersonView(); // Метод инициализации третьего лица

	UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character") // Метод, доступный из Blueprints, в категории "UnrealSandbox Character"
	void InitFirstPersonView(); // Метод инициализации первого лица

	PlayerView GetSandboxPlayerView(); // Метод для получения текущего вида игрока из песочницы

	void SetSandboxPlayerView(PlayerView SandboxView); // Метод для установки вида игрока из песочницы

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") // Свойство начального вида игрока из песочницы
	PlayerView InitialView = PlayerView::TOP_DOWN; // Начальный вид игрока (по умолчанию - вид сверху)

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") // Свойство для включения автоматической смены вида
	bool bEnableAutoSwitchView = true; // Флаг автоматической смены вида (по умолчанию true)

	bool IsDead() { return bIsDead; } // Метод проверки мертв ли персонаж (возвращает значение bIsDead)

	UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character") // Метод, доступный из Blueprints, в категории "UnrealSandbox Character"
	void Kill(); // Метод убийства персонажа

	UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character") // Метод, доступный из Blueprints, в категории "UnrealSandbox Character"
	void LiveUp(); // Метод восстановления персонажа

	void Jump() override; // Переопределение метода прыжка

	void StopJumping() override; // Переопределение метода остановки прыжка

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") // Свойство максимального зума
	float MaxZoom; 

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 	// Свойство максимального зума сверху
	float MaxZoomTopDown;

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 	// Свойство минимального зума
	float MinZoom;

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 	// Шаг зума
	float ZoomStep;

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 	// Скорость ходьбы персонажа
	float WalkSpeed;

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 	// Скорость бега персонажа
	float RunSpeed;

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 	// Длина целевого взаимодействия
	float InteractionTargetLength;

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 	// Порог скорости удара
	float VelocityHitThreshold;

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") 	// Фактор скорости удара
	float VelocityHitFactor;

	double VelocityHitTimestamp; 	// Временная метка удара по скорости 

	void BoostOn(); 	// Включение ускорения 

	void BoostOff();  	// Выключение ускорения 

	int GetSandboxTypeId() override;  	// Переопределение метода получения идентификатора типа песочницы 

	FString GetSandboxPlayerUid() override;  	// Переопределение метода получения уникального идентификатора игрока 

	/*
	UFUNCTION(BlueprintImplementableEvent, Category = "DmgSystem")
	void TakeDamage(float DamageLevel); 
	*/  	// Закомментированный метод обработки урона 

	template<class T>  	// Шаблонный метод для получения первого компонента по имени 
	T* GetFirstComponentByName(FString ComponentName) {
		TArray<T*> Components;  	// Массив компонентов типа T 
		GetComponents<T>(Components);  	// Получение всех компонентов типа T 
		for (T* Component : Components) {  	// Цикл по всем компонентам 
			if (Component->GetName() == ComponentName)  	// Если имя компонента совпадает с заданным 
				return Component;  	// Возврат найденного компонента 
		}

		return nullptr;  	// Возврат nullptr если компонент не найден 
	}

private:
	PlayerView CurrentPlayerView;  	// Текущий вид игрока 

	bool bIsDead = false;  	// Флаг мертв ли персонаж 

	FTransform InitialMeshTransform;  	// Начальная трансформация меша 

	UFUNCTION()  	// Объявление функции обработки столкновения 
	void OnHit(class UPrimitiveComponent* HitComp, class AActor* Actor, class UPrimitiveComponent* Other, FVector Impulse, const FHitResult & HitResult);

	//UPROPERTY()
	//UVitalSystemComponent* VitalSystemComponent;    // Закомментированное свойство системы жизней 

protected:
	void ZoomIn();    // Метод увеличения зума 

	void ZoomOut();    // Метод уменьшения зума 

	virtual void OnDeath();    // Виртуальный метод обработки смерти персонажа 

	//void Test();    // Закомментированный тестовый метод 

    /** Called for forwards/backward input */  
    void MoveForward(float Value);    // Метод обработки ввода вперед/назад 

    /** Called for side to side input */  
    void MoveRight(float Value);    // Метод обработки ввода влево/вправо 

    /**
    * Called via input to turn at a given rate.
    * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate 
    */
    void TurnAtRate(float Rate);    // Метод обработки поворота с заданной скоростью 

    /**
    * Called via input to turn look up/down at a given rate.
    * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate 
    */
    void LookUpAtRate(float Rate);    // Метод обработки взгляда вверх/вниз с заданной скоростью 

    virtual void AddControllerYawInput(float Val) override;    // Переопределение метода добавления угла поворота по оси Yaw 

    virtual void AddControllerPitchInput(float Val) override;    // Переопределение метода добавления угла поворота по оси Pitch 

    virtual FVector GetThirdPersonViewCameraPos();    // Виртуальный метод получения позиции камеры третьего лица 

    virtual FRotator GetTopDownViewCameraRot();    // Виртуальный метод получения вращения камеры сверху 

    virtual bool CanMove();    // Виртуальный метод проверки возможности движения 
};
