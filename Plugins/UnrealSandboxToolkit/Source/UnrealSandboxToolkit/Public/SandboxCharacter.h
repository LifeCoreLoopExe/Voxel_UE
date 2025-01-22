// Заполните информацию об авторских правах на странице описания настроек проекта.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/UMG/Public/UMG.h"
#include "SlateBasics.h"
#include "Perception/AIPerceptionComponent.h"
#include "SandboxCharacter.generated.h"

// Объявление класса компонента системы жизненных показателей
class UVitalSystemComponent;

// Перечисление для типов вида игрока
UENUM(BlueprintType)
enum class PlayerView : uint8 {
    TOP_DOWN = 0        UMETA(DisplayName = "Top Down"), // Вид сверху
    THIRD_PERSON = 1    UMETA(DisplayName = "Third Person"), // Вид от третьего лица
    FIRST_PERSON = 2    UMETA(DisplayName = "First Person") // Вид от первого лица
};

// Объявление интерфейса для песочницы
UINTERFACE(MinimalAPI, Blueprintable)
class USandboxCoreCharacter : public UInterface {
    GENERATED_BODY()
};

// Реализация интерфейса для песочницы
class ISandboxCoreCharacter {
    GENERATED_BODY()

public:
    // Виртуальные функции для получения и установки различных параметров персонажа
    virtual int GetSandboxTypeId() = 0;
    virtual FString GetSandboxPlayerUid() = 0;
    virtual float GetStaminaTickDelta() { return 0; }
    virtual void OnStaminaExhausted() { }
    virtual void Kill() { }
    virtual bool IsDead() { return false; }

    // Функция для получения параметра
    FString GetParam(const FString& Param) { return (BaseParamMap.Contains(Param)) ? BaseParamMap[Param] : TEXT(""); }

    // Функция для установки параметра
    void SetParam(const FString& Param, const FString& Val) { BaseParamMap.Add(Param, Val); OnSetSandboxBaseParam(); }

    // Виртуальная функция для обработки установки базового параметра
    virtual void OnSetSandboxBaseParam() { }

    // Функция для получения состояния
    int GetState() { return State; }

protected:
    // Состояние персонажа
    int State = 0;

private:
    // Карта базовых параметров
    TMap<FString, FString> BaseParamMap;
};

// Объявление класса персонажа песочницы
UCLASS()
class UNREALSANDBOXTOOLKIT_API ASandboxCharacter : public ACharacter, public ISandboxCoreCharacter {
    GENERATED_BODY()

public:
    // Конструктор
    ASandboxCharacter();

    // Функция, вызываемая при начале игры
    virtual void BeginPlay() override;

    // Функция, вызываемая каждый кадр
    virtual void Tick(float DeltaSeconds) override;

    // Функция для настройки ввода игрока
    virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

    // Идентификатор типа песочницы
    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Core Character")
    int SandboxTypeId = 0;

    // Компонент камеры следования
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;

    // Компонент камеры от первого лица
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FirstPersonCamera;

    // Компонент пружинного рычага камеры
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    // Функция для получения компонента пружинного рычага камеры
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

    // Функция для инициализации вида сверху
    UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character")
    void InitTopDownView();

    // Функция для инициализации вида от третьего лица
    UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character")
    void InitThirdPersonView();

    // Функция для инициализации вида от первого лица
    UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character")
    void InitFirstPersonView();

    // Функция для получения текущего вида игрока
    PlayerView GetSandboxPlayerView();

    // Функция для установки вида игрока
    void SetSandboxPlayerView(PlayerView SandboxView);

    // Начальный вид игрока
    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
    PlayerView InitialView = PlayerView::TOP_DOWN;

    // Флаг для автоматического переключения вида
    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
    bool bEnableAutoSwitchView = true;

    // Функция для проверки, мертв ли персонаж
    bool IsDead() { return bIsDead; }

    // Функция для убийства персонажа
    UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character")
    void Kill();

    // Функция для оживления персонажа
    UFUNCTION(BlueprintCallable, Category = "UnrealSandbox Character")
    void LiveUp();

    // Переопределение функции прыжка
    void Jump() override;

    // Переопределение функции остановки прыжка
    void StopJumping() override;

    // Максимальное увеличение
    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
    float MaxZoom;

    // Максимальное увеличение для вида сверху
    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
    float MaxZoomTopDown;

    // Минимальное увеличение
    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
    float MinZoom;

    // Шаг увеличения
    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
    float ZoomStep;

    // Скорость ходьбы
    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
    float WalkSpeed;

    // Скорость бега
    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
    float RunSpeed;

    // Длина цели взаимодействия
    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
    float InteractionTargetLength;

    // Порог скорости удара
    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
    float VelocityHitThreshold;

    // Фактор скорости удара
    UPROPERTY(EditAnywhere, Category = "UnrealSandbox Character")
    float VelocityHitFactor;

    // Временная метка скорости удара
    double VelocityHitTimestamp;

    // Функция для включения ускорения
    void BoostOn();

    // Функция для выключения ускорения
    void BoostOff();

    // Переопределение функции для получения идентификатора типа песочницы
    int GetSandboxTypeId() override;

    // Переопределение функции для получения идентификатора игрока песочницы
    FString GetSandboxPlayerUid() override;

    /*
    // Событие для получения урона
    UFUNCTION(BlueprintImplementableEvent, Category = "DmgSystem")
    void TakeDamage(float DamageLevel);
    */

    // Шаблонная функция для получения первого компонента по имени
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
    // Текущий вид игрока
    PlayerView CurrentPlayerView;

    // Флаг, указывающий, мертв ли персонаж
    bool bIsDead = false;

    // Начальное преобразование меша
    FTransform InitialMeshTransform;

    // Функция, вызываемая при попадании
    UFUNCTION()
    void OnHit(class UPrimitiveComponent* HitComp, class AActor* Actor, class UPrimitiveComponent* Other, FVector Impulse, const FHitResult & HitResult);

    // Компонент системы жизненных показателей
    // UPROPERTY()
    // UVitalSystemComponent* VitalSystemComponent;

protected:
    // Функция для увеличения
    void ZoomIn();

    // Функция для уменьшения
    void ZoomOut();

    // Виртуальная функция для обработки смерти
    virtual void OnDeath();

    // Функция для движения вперед
    void MoveForward(float Value);

    // Функция для движения вправо
    void MoveRight(float Value);

    // Функция для поворота на заданную скорость
    void TurnAtRate(float Rate);

    // Функция для поворота вверх/вниз на заданную скорость
    void LookUpAtRate(float Rate);

    // Переопределение функции для добавления ввода рыскания контроллера
    virtual void AddControllerYawInput(float Val) override;

    // Переопределение функции для добавления ввода тангажа контроллера
    virtual void AddControllerPitchInput(float Val) override;

    // Виртуальная функция для получения позиции камеры вида от третьего лица
    virtual FVector GetThirdPersonViewCameraPos();

    // Виртуальная функция для получения поворота камеры вида сверху
    virtual FRotator GetTopDownViewCameraRot();

    // Виртуальная функция для проверки, может ли персонаж двигаться
    virtual bool CanMove();
};
