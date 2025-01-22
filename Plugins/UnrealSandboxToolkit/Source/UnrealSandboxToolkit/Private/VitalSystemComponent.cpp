// Copyright blackw 2015-2020

#include "VitalSystemComponent.h" // Подключение заголовочного файла компонента жизненно важной системы
#include "SandboxCharacter.h" // Подключение заголовочного файла персонажа песочницы
#include "SandboxObject.h" // Подключение заголовочного файла объекта песочницы
#include "Net/UnrealNetwork.h" // Подключение заголовочного файла для работы с сетью в Unreal Engine

#define STAMINA_CLC_FACTOR 0.1 // Определение коэффициента для расчета выносливости
#define STAMINA_REDUCTION_FACTOR 0.25 // Определение коэффициента для уменьшения выносливости

UVitalSystemComponent::UVitalSystemComponent() {
    // bWantsBeginPlay = true; // Закомментированная строка, которая указывает, что компонент хочет вызывать BeginPlay
    PrimaryComponentTick.bCanEverTick = false; // Указывает, что компонент не будет тикать

    Health = 100; // Инициализация здоровья
    MaxHealth = 100; // Инициализация максимального здоровья

    Stamina = 60; // Инициализация выносливости
    MaxStamina = 60; // Инициализация максимальной выносливости
}

bool UVitalSystemComponent::IsOwnerAdmin() {
    return false; // Метод, который проверяет, является ли владелец админом (всегда возвращает false)
}

float UVitalSystemComponent::GetHealth() {
    return Health; // Метод, который возвращает текущее здоровье
}

float UVitalSystemComponent::GetMaxHealth() {
    return MaxHealth; // Метод, который возвращает максимальное здоровье
}

void UVitalSystemComponent::ChangeHealth(float Val){
    Health += Val; // Изменение здоровья на заданное значение

    if (Health <= 0) {
        Health = 0; // Если здоровье меньше или равно нулю, устанавливаем его в ноль
    }

    if (Health > MaxHealth) {
        Health = MaxHealth; // Если здоровье превышает максимальное значение, устанавливаем его в максимальное значение
    }
}

void UVitalSystemComponent::DamageFromFall(float Velocity) {
    auto EquipmentList = ASandboxObjectUtils::GetContainerContent(GetOwner(), TEXT("Equipment")); // Получение списка экипировки владельца
    float DamageFallFactor = 1.f; // Инициализация коэффициента урона от падения
    for (auto* Item : EquipmentList) {
        ASandboxSkeletalModule* Skm = Cast<ASandboxSkeletalModule>(Item); // Приведение элемента к типу ASandboxSkeletalModule
        if (Skm) {
            DamageFallFactor *= Skm->GetAffectParam(TEXT("damage_fall_factor")); // Умножение коэффициента урона от падения на параметр элемента
        }
    }

    const float Dmg = Velocity * 100.f / 2000.f * DamageFallFactor; // Расчет урона от падения
    Damage(Dmg); // Нанесение урона
}

void UVitalSystemComponent::Damage(float DamageVal) {
    if (IsOwnerAdmin()) {
        return; // Если владелец админ, выходим из метода
    }

    ChangeHealth(-DamageVal); // Уменьшение здоровья на величину урона
    ChangeStamina(-DamageVal); // Уменьшение выносливости на величину урона

    if (Health == 0) {
        ISandboxCoreCharacter* SandboxCharacter = Cast<ISandboxCoreCharacter>(GetOwner()); // Приведение владельца к типу ISandboxCoreCharacter
        if (SandboxCharacter) {
            SandboxCharacter->Kill(); // Убийство персонажа, если здоровье равно нулю
        }
    }
}

float UVitalSystemComponent::GetStamina() {
    return Stamina; // Метод, который возвращает текущую выносливость
}

float UVitalSystemComponent::GetMaxStamina() {
    return MaxStamina; // Метод, который возвращает максимальную выносливость
}

void UVitalSystemComponent::ChangeStamina(float Val) {
    if (IsOwnerAdmin() && Val < 0) {
        return; // Если владелец админ и значение отрицательное, выходим из метода
    }

    Stamina += Val; // Изменение выносливости на заданное значение

    if (Stamina <= 0) {
        Stamina = 0; // Если выносливость меньше или равна нулю, устанавливаем её в ноль
    }

    if (Stamina > MaxStamina) {
        Stamina = MaxStamina; // Если выносливость превышает максимальное значение, устанавливаем её в максимальное значение
    }
}

void UVitalSystemComponent::PerformTimer() {
    ISandboxCoreCharacter* SandboxCharacter = Cast<ISandboxCoreCharacter>(GetOwner()); // Приведение владельца к типу ISandboxCoreCharacter
    if (SandboxCharacter) {
        auto EquipmentList = ASandboxObjectUtils::GetContainerContent(GetOwner(), TEXT("Equipment")); // Получение списка экипировки владельца

        float StaminaFactor = 1.f; // Инициализация коэффициента выносливости
        float RecoverStaminaFactor = 1.f; // Инициализация коэффициента восстановления выносливости

        for (auto* Item : EquipmentList) {
            ASandboxSkeletalModule* Skm = Cast<ASandboxSkeletalModule>(Item); // Приведение элемента к типу ASandboxSkeletalModule
            if (Skm) {
                StaminaFactor *= Skm->GetAffectParam(TEXT("stamina_factor")); // Умножение коэффициента выносливости на параметр элемента
                RecoverStaminaFactor *= Skm->GetAffectParam(TEXT("recover_stamina_factor")); // Умножение коэффициента восстановления выносливости на параметр элемента
            }
        }

        const float D = -SandboxCharacter->GetStaminaTickDelta(); // Получение дельты выносливости
        if (D < 0) {
            const float TickStaminaReduction = D * StaminaFactor * STAMINA_CLC_FACTOR * STAMINA_REDUCTION_FACTOR; // Расчет уменьшения выносливости
            ChangeStamina(TickStaminaReduction); // Изменение выносливости
        } else {
            // восстановление выносливости
            if (!SandboxCharacter->IsDead()) {
                const float RecoverStamina = 1.f * RecoverStaminaFactor; // Расчет восстановления выносливости
                ChangeStamina(RecoverStamina); // Изменение выносливости
            }
        }

        if (Stamina == 0) {
            SandboxCharacter->OnStaminaExhausted(); // Вызов метода, когда выносливость исчерпана
        }
    }
}

bool UVitalSystemComponent::CheckStamina(float Val) {
    return Stamina > Val; // Метод, который проверяет, достаточно ли выносливости
}

void UVitalSystemComponent::BeginPlay() {
    Super::BeginPlay(); // Вызов метода BeginPlay родительского класса
    GetWorld()->GetTimerManager().SetTimer(Timer, this, &UVitalSystemComponent::PerformTimer, STAMINA_CLC_FACTOR, true); // Установка таймера для вызова метода PerformTimer
}

void UVitalSystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
    // Метод, который вызывается при завершении игры
}

void UVitalSystemComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) {
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction ); // Вызов метода TickComponent родительского класса
}

void UVitalSystemComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
    DOREPLIFETIME(UVitalSystemComponent, Health); // Репликация здоровья
    DOREPLIFETIME(UVitalSystemComponent, MaxHealth); // Репликация максимального здоровья
    DOREPLIFETIME(UVitalSystemComponent, Stamina); // Репликация выносливости
    DOREPLIFETIME(UVitalSystemComponent, MaxStamina); // Репликация максимальной выносливости
}
