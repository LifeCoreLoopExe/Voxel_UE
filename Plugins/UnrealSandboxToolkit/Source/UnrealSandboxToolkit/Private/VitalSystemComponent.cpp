// Copyright blackw 2015-2020

#include "VitalSystemComponent.h" // Включает заголовочный файл для VitalSystemComponent
#include "SandboxCharacter.h" // Включает заголовочный файл для SandboxCharacter
#include "SandboxObject.h" // Включает заголовочный файл для SandboxObject
#include "Net/UnrealNetwork.h" // Включает заголовочный файл для сетевых функций

#define STAMINA_CLC_FACTOR 0.1 // Определяет фактор расчета выносливости
#define STAMINA_REDUCTION_FACTOR 0.25 // Определяет фактор снижения выносливости

UVitalSystemComponent::UVitalSystemComponent() {
    // bWantsBeginPlay = true;
    PrimaryComponentTick.bCanEverTick = false; // Отключает тикирование компонента

    Health = 100; // Устанавливает начальное здоровье
    MaxHealth = 100; // Устанавливает максимальное здоровье

    Stamina = 60; // Устанавливает начальную выносливость
    MaxStamina = 60; // Устанавливает максимальную выносливость
}

bool UVitalSystemComponent::IsOwnerAdmin() {
    return false; // Возвращает false (заглушка)
}

float UVitalSystemComponent::GetHealth() {
    return Health; // Возвращает текущее здоровье
}

float UVitalSystemComponent::GetMaxHealth() {
    return MaxHealth; // Возвращает максимальное здоровье
}

void UVitalSystemComponent::ChangeHealth(float Val) {
    Health += Val; // Изменяет здоровье

    if (Health <= 0) {
        Health = 0; // Ограничивает здоровье минимальным значением
    }

    if (Health > MaxHealth) {
        Health = MaxHealth; // Ограничивает здоровье максимальным значением
    }
}

void UVitalSystemComponent::DamageFromFall(float Velocity) {
    auto EquipmentList = ASandboxObjectUtils::GetContainerContent(GetOwner(), TEXT("Equipment")); // Получает список экипировки
    float DamageFallFactor = 1.f;
    for (auto* Item : EquipmentList) {
        ASandboxSkeletalModule* Skm = Cast<ASandboxSkeletalModule>(Item);
        if (Skm) {
            DamageFallFactor *= Skm->GetAffectParam(TEXT("damage_fall_factor")); // Учитывает фактор урона от падения
        }
    }

    const float Dmg = Velocity * 100.f / 2000.f * DamageFallFactor; // Рассчитывает урон от падения
    Damage(Dmg); // Наносит урон
}

void UVitalSystemComponent::Damage(float DamageVal) {
    if (IsOwnerAdmin()) {
        return; // Если владелец администратор, выходит из метода
    }

    ChangeHealth(-DamageVal); // Уменьшает здоровье
    ChangeStamina(-DamageVal); // Уменьшает выносливость

    if (Health == 0) {
        ISandboxCoreCharacter* SandboxCharacter = Cast<ISandboxCoreCharacter>(GetOwner());
        if (SandboxCharacter) {
            SandboxCharacter->Kill(); // Убивает персонажа, если здоровье равно 0
        }
    }
}

float UVitalSystemComponent::GetStamina() {
    return Stamina; // Возвращает текущую выносливость
}

float UVitalSystemComponent::GetMaxStamina() {
    return MaxStamina; // Возвращает максимальную выносливость
}

void UVitalSystemComponent::ChangeStamina(float Val) {
    if (IsOwnerAdmin() && Val < 0) {
        return; // Если владелец администратор и значение отрицательное, выходит из метода
    }

    Stamina += Val; // Изменяет выносливость

    if (Stamina <= 0) {
        Stamina = 0; // Ограничивает выносливость минимальным значением
    }

    if (Stamina > MaxStamina) {
        Stamina = MaxStamina; // Ограничивает выносливость максимальным значением
    }
}

void UVitalSystemComponent::PerformTimer() {
    ISandboxCoreCharacter* SandboxCharacter = Cast<ISandboxCoreCharacter>(GetOwner());
    if (SandboxCharacter) {
        auto EquipmentList = ASandboxObjectUtils::GetContainerContent(GetOwner(), TEXT("Equipment")); // Получает список экипировки

        float StaminaFactor = 1.f;
        float RecoverStaminaFactor = 1.f;

        for (auto* Item : EquipmentList) {
            ASandboxSkeletalModule* Skm = Cast<ASandboxSkeletalModule>(Item);
            if (Skm) {
                StaminaFactor *= Skm->GetAffectParam(TEXT("stamina_factor")); // Учитывает фактор выносливости
                RecoverStaminaFactor *= Skm->GetAffectParam(TEXT("recover_stamina_factor")); // Учитывает фактор восстановления выносливости
            }
        }

        const float D = -SandboxCharacter->GetStaminaTickDelta();
        if (D < 0) {
            const float TickStaminaReduction = D * StaminaFactor * STAMINA_CLC_FACTOR * STAMINA_REDUCTION_FACTOR; // Рассчитывает снижение выносливости
            ChangeStamina(TickStaminaReduction);
        } else {
            // Восстанавливает выносливость
            if (!SandboxCharacter->IsDead()) {
                const float RecoverStamina = 1.f * RecoverStaminaFactor; // Рассчитывает восстановление выносливости
                ChangeStamina(RecoverStamina);
            }
        }

        if (Stamina == 0) {
            SandboxCharacter->OnStaminaExhausted(); // Вызывает метод при исчерпании выносливости
        }
    }
}

bool UVitalSystemComponent::CheckStamina(float Val) {
    return Stamina > Val; // Проверяет, достаточно ли выносливости
}

void UVitalSystemComponent::BeginPlay() {
    Super::BeginPlay();
    GetWorld()->GetTimerManager().SetTimer(Timer, this, &UVitalSystemComponent::PerformTimer, STAMINA_CLC_FACTOR, true); // Устанавливает таймер
}

void UVitalSystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
    // Метод для завершения игры (пустой)
}

void UVitalSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction); // Вызывает метод TickComponent родительского класса
}

void UVitalSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    DOREPLIFETIME(UVitalSystemComponent, Health); // Реплицирует свойство Health
    DOREPLIFETIME(UVitalSystemComponent, MaxHealth); // Реплицирует свойство MaxHealth
    DOREPLIFETIME(UVitalSystemComponent, Stamina); // Реплицирует свойство Stamina
    DOREPLIFETIME(UVitalSystemComponent, MaxStamina); // Реплицирует свойство MaxStamina
}
