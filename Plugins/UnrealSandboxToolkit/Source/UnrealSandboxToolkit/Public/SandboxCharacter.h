// Fill out your copyright notice in the Description page of Project Settings.

#pragma once // Включает файл только один раз

#include "EngineMinimal.h" // Включает минимальные функции движка
#include "GameFramework/Character.h" // Включает класс Character
#include "Runtime/UMG/Public/UMG.h" // Включает основные функции UMG
#include "SlateBasics.h" // Включает базовые функции Slate
#include "Perception/AIPerceptionComponent.h" // Включает компонент восприятия AI
#include "SandboxCharacter.generated.h" // Включает сгенерированный заголовочный файл для SandboxCharacter

class UVitalSystemComponent; // Предварительное объявление класса UVitalSystemComponent

UENUM(BlueprintType) // Объявление перечисления, доступного для Blueprint
enum class PlayerView : uint8 {
    TOP_DOWN = 0 UMETA(DisplayName = "Top Down"), // Вид сверху
    THIRD_PERSON = 1 UMETA(DisplayName = "Third Person"), // Вид от третьего лица
    FIRST_PERSON = 2 UMETA(DisplayName = "First Person") // Вид от первого лица
};

UINTERFACE(MinimalAPI, Blueprintable) // Объявление интерфейса, доступного для Blueprint
class USandboxCoreCharacter : public UInterface {
    GENERATED_BODY() // Макрос для генерации тела интерфейса
};

class ISandboxCoreCharacter {
    GENERATED_BODY() // Макрос для генерации тела класса

public:
    virtual int GetSandboxTypeId() = 0; // Метод для получения идентификатора типа Sandbox

    virtual FString GetSandboxPlayerUid() = 0; // Метод для получения уникального идентификатора игрока Sandbox

    virtual float GetStaminaTickDelta() { return 0; } // Метод для получения дельты тика выносливости

    virtual void OnStaminaExhausted() { } // Метод для обработки исчерпания выносливости

    virtual void Kill() { } // Метод для убийства персонажа

    virtual bool IsDead() { return false; } // Метод для проверки, мертв ли персонаж

    FString GetParam(const FString& Param) { return (BaseParamMap.Contains(Param)) ? BaseParamMap[Param] : TEXT(""); } // Метод для получения параметра

    void SetParam(const FString& Param, const FString& Val) { BaseParamMap.Add(Param, Val); OnSetSandboxBaseParam(); } // Метод для установки параметра

    virtual void OnSetSandboxBaseParam() { } // Метод для обработки установки базового параметра Sandbox

    int GetState() { return State; } // Метод для получения состояния

protected:
    int State = 0; // Состояние персонажа

private:
    TMap<FString, FString> BaseParamMap; // Карта базовых параметров
};


UCLASS() // Объявление класса, доступного для рефлексии UObject
class UNREALSANDBOXTOOLKIT_API ASandboxCharacter : public ACharacter, public ISandboxCoreCharacter { // Объявление класса ASandboxCharacter, наследующегося от ACharacter и ISandboxCoreCharacter
    GENERATED_BODY() // Макрос для генерации тела класса

public:
    ASandboxCharacter(); // Конструктор

    virtual void BeginPlay() override; // Метод, вызываемый при начале игры

    virtual void Tick(float DeltaSeconds) override; // Метод, вызываемый каждый тик

    virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override; // Метод для настройки компонента ввода игрока

    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Core Character") // Свойство, доступное для редактирования в редакторе
    int SandboxTypeId = 0; // Идентификатор типа Sandbox

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true")) // Свойство, доступное для чтения в Blueprint
    class UCameraComponent* FollowCamera; // Камера следования

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true")) // Свойство, доступное для чтения в Blueprint
    class UCameraComponent* FirstPersonCamera; // Камера от первого лица

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true")) // Свойство, доступное для чтения в Blueprint
    class USpringArmComponent* CameraBoom; // Компонент камеры на пружине

    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; } // Метод для получения компонента камеры на пружине

    UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character") // Функция, доступная для вызова из Blueprint
    void InitTopDownView(); // Метод для инициализации вида сверху

    UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character") // Функция, доступная для вызова из Blueprint
    void InitThirdPersonView(); // Метод для инициализации вида от третьего лица

    UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character") // Функция, доступная для вызова из Blueprint
    void InitFirstPersonView(); // Метод для инициализации вида от первого лица

    PlayerView GetSandboxPlayerView(); // Метод для получения текущего вида игрока

    void SetSandboxPlayerView(PlayerView SandboxView); // Метод для установки вида игрока

    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") // Свойство, доступное для редактирования в редакторе
    PlayerView InitialView = PlayerView::TOP_DOWN; // Начальный вид игрока

    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") // Свойство, доступное для редактирования в редакторе
    bool bEnableAutoSwitchView = true; // Флаг автоматического переключения вида

    bool IsDead() { return bIsDead; } // Метод для проверки, мертв ли персонаж

    UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character") // Функция, доступная для вызова из Blueprint
    void Kill(); // Метод для убийства персонажа

    UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character") // Функция, доступная для вызова из Blueprint
    void LiveUp(); // Метод для оживления персонажа

    void Jump() override; // Метод для прыжка

    void StopJumping() override; // Метод для остановки прыжка

    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") // Свойство, доступное для редактирования в редакторе
    float MaxZoom; // Максимальный зум

    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") // Свойство, доступное для редактирования в редакторе
    float MaxZoomTopDown; // Максимальный зум для вида сверху

    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") // Свойство, доступное для редактирования в редакторе
    float MinZoom; // Минимальный зум

    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") // Свойство, доступное для редактирования в редакторе
    float ZoomStep; // Шаг зума

    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") // Свойство, доступное для редактирования в редакторе
    float WalkSpeed; // Скорость ходьбы

    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") // Свойство, доступное для редактирования в редакторе
    float RunSpeed; // Скорость бега

    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") // Свойство, доступное для редактирования в редакторе
    float InteractionTargetLength; // Длина цели взаимодействия

    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") // Свойство, доступное для редактирования в редакторе
    float VelocityHitThreshold; // Порог удара по скорости

    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character") // Свойство, доступное для редактирования в редакторе
    float VelocityHitFactor; // Фактор удара по скорости

    double VelocityHitTimestamp; // Временная метка удара по скорости

    void BoostOn(); // Метод для включения ускорения

    void BoostOff(); // Метод для выключения ускорения

    int GetSandboxTypeId() override; // Метод для получения идентификатора типа Sandbox

    FString GetSandboxPlayerUid() override; // Метод для получения уникального идентификатора игрока Sandbox

    template<class T>
    T* GetFirstComponentByName(FString ComponentName) { // Шаблонный метод для получения первого компонента по имени
        TArray<T*> Components;
        GetComponents<T>(Components);
        for (T* Component : Components) {
            if (Component->GetName() == ComponentName)
                return Component;
        }
        return nullptr;
    }
};

private:
    PlayerView CurrentPlayerView; // Текущий вид игрока

    bool bIsDead = false; // Флаг, указывающий, мертв ли персонаж

    FTransform InitialMeshTransform; // Начальное преобразование меша

    UFUNCTION()
    void OnHit(class UPrimitiveComponent* HitComp, class AActor* Actor, class UPrimitiveComponent* Other, FVector Impulse, const FHitResult & HitResult); // Метод для обработки попадания

    // UPROPERTY()
    // UVitalSystemComponent* VitalSystemComponent;

protected:
    void ZoomIn(); // Метод для увеличения зума

    void ZoomOut(); // Метод для уменьшения зума

    virtual void OnDeath(); // Метод для обработки смерти персонажа

    // void Test();

    /** Called for forwards/backward input */
    void MoveForward(float Value); // Метод для движения вперед/назад

    /** Called for side to side input */
    void MoveRight(float Value); // Метод для движения влево/вправо

    /**
     * Called via input to turn at a given rate.
     * @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
     */
    void TurnAtRate(float Rate); // Метод для поворота с заданной скоростью

    /**
     * Called via input to turn look up/down at a given rate.
     * @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
     */
    void LookUpAtRate(float Rate); // Метод для поворота вверх/вниз с заданной скоростью

    virtual void AddControllerYawInput(float Val) override; // Метод для добавления ввода рыскания контроллера

    virtual void AddControllerPitchInput(float Val) override; // Метод для добавления ввода тангажа контроллера

    virtual FVector GetThirdPersonViewCameraPos(); // Метод для получения позиции камеры в виде от третьего лица

    virtual FRotator GetTopDownViewCameraRot(); // Метод для получения вращения камеры в виде сверху

    virtual bool CanMove(); // Метод для проверки, может ли персонаж двигаться
};
