// Copyright blackw 2015-2020

#include "VitalSystemComponent.h" // Подключение заголовочного файла для компонента VitalSystem
#include "SandboxCharacter.h" // Подключение заголовочного файла для персонажа Sandbox
#include "SandboxObject.h" // Подключение заголовочного файла для объектов Sandbox
#include "Net/UnrealNetwork.h" // Подключение заголовочного файла для сетевых функций Unreal Engine

#define STAMINA_CLC_FACTOR 0.1 // Определение фактора расчета выносливости
#define STAMINA_REDUCTION_FACTOR 0.25 // Определение фактора уменьшения выносливости

UVitalSystemComponent::UVitalSystemComponent() { // Конструктор компонента VitalSystem
	// bWantsBeginPlay = true; // Инициализация флага, который указывает, что компонент хочет начать игру (закомментировано)
	PrimaryComponentTick.bCanEverTick = false; // Отключение возможности вызова Tick для этого компонента

	Health = 100; // Установка начального значения здоровья
	MaxHealth = 100; // Установка максимального значения здоровья

	Stamina = 60; // Установка начального значения выносливости
	MaxStamina = 60; // Установка максимального значения выносливости
}

bool UVitalSystemComponent::IsOwnerAdmin() { // Метод для проверки, является ли владелец администратором
	return false; // Возвращает false, указывая, что владелец не является администратором
}

float UVitalSystemComponent::GetHealth() { // Метод для получения текущего значения здоровья
	return Health; // Возвращает текущее значение здоровья
}

float UVitalSystemComponent::GetMaxHealth() { // Метод для получения максимального значения здоровья
	return MaxHealth; // Возвращает максимальное значение здоровья
}

void UVitalSystemComponent::ChangeHealth(float Val) { // Метод для изменения значения здоровья на заданное значение
	Health += Val; // Увеличивает или уменьшает здоровье на Val

	if (Health <= 0) { 
		Health = 0; // Устанавливает здоровье в 0, если оно стало отрицательным
	}

	if (Health > MaxHealth) { 
		Health = MaxHealth; // Ограничивает здоровье максимальным значением
	}
}

void UVitalSystemComponent::DamageFromFall(float Velocity) { // Метод для расчета урона от падения на основе скорости падения
	auto EquipmentList = ASandboxObjectUtils::GetContainerContent(GetOwner(), TEXT("Equipment")); // Получает список оборудования владельца
	float DamageFallFactor = 1.f; // Инициализация коэффициента урона от падения

	for (auto* Item : EquipmentList) { // Проходит по каждому элементу в списке оборудования
		ASandboxSkeletalModule* Skm = Cast<ASandboxSkeletalModule>(Item); // Приводит элемент к типу ASandboxSkeletalModule
		if (Skm) { 
			DamageFallFactor *= Skm->GetAffectParam(TEXT("damage_fall_factor")); // Умножает коэффициент урона на параметр урона от падения из оборудования
		}
	}

	const float Dmg = Velocity * 100.f / 2000.f * DamageFallFactor; // Расчет урона от падения на основе скорости и коэффициента
	Damage(Dmg); // Наносит рассчитанный урон
}

void UVitalSystemComponent::Damage(float DamageVal) { // Метод для нанесения урона на заданное значение DamageVal
	if (IsOwnerAdmin()) { 
		return; // Если владелец администратор, выход из метода без нанесения урона
	}

	ChangeHealth(-DamageVal); // Уменьшает здоровье на величину DamageVal
	ChangeStamina(-DamageVal); // Уменьшает выносливость на величину DamageVal

	if (Health == 0) { 
		ISandboxCoreCharacter* SandboxCharacter = Cast<ISandboxCoreCharacter>(GetOwner()); // Приведение владельца к типу ISandboxCoreCharacter
		if (SandboxCharacter) {
			SandboxCharacter->Kill(); // Вызывает метод Kill, если персонаж существует и здоровье равно нулю
		}
	}
}

float UVitalSystemComponent::GetStamina() { // Метод для получения текущего значения выносливости
	return Stamina; // Возвращает текущее значение выносливости
}

float UVitalSystemComponent::GetMaxStamina() { // Метод для получения максимального значения выносливости
	return MaxStamina; // Возвращает максимальное значение выносливости
}

void UVitalSystemComponent::ChangeStamina(float Val) { // Метод для изменения значения выносливости на заданное значение Val
	if (IsOwnerAdmin() && Val < 0) { 
		return; // Если владелец администратор и Val отрицательное, выход из метода без изменения выносливости
	}

	Stamina += Val; // Увеличивает или уменьшает выносливость на Val

	if (Stamina <= 0) { 
		Stamina = 0; // Устанавливает выносливость в 0, если она стала отрицательной
	}

	if (Stamina > MaxStamina) { 
		Stamina = MaxStamina; // Ограничивает выносливость максимальным значением
	}
}

void UVitalSystemComponent::PerformTimer() { // Метод, выполняемый по таймеру для обновления состояния выносливости и здоровья
	ISandboxCoreCharacter* SandboxCharacter = Cast<ISandboxCoreCharacter>(GetOwner()); 
	if (SandboxCharacter) {
		auto EquipmentList = ASandboxObjectUtils::GetContainerContent(GetOwner(), TEXT("Equipment")); 

		float StaminaFactor = 1.f; 
		float RecoverStaminaFactor = 1.f;

		for (auto* Item : EquipmentList) {
			ASandboxSkeletalModule* Skm = Cast<ASandboxSkeletalModule>(Item); 
			if (Skm) {
				StaminaFactor *= Skm->GetAffectParam(TEXT("stamina_factor")); 
				RecoverStaminaFactor *= Skm->GetAffectParam(TEXT("recover_stamina_factor")); 
			}
		}

		const float D = -SandboxCharacter->GetStaminaTickDelta(); 
		if (D < 0) {
			const float TickStaminaReduction = D * StaminaFactor * STAMINA_CLC_FACTOR * STAMINA_REDUCTION_FACTOR; 
			ChangeStamina(TickStaminaReduction); 
		} else {
			if (!SandboxCharacter->IsDead()) {
				const float RecoverStamina = 1.f * RecoverStaminaFactor; 
				ChangeStamina(RecoverStamina); 
			}
		}

        if (Stamina == 0) {
            SandboxCharacter->OnStaminaExhausted(); // Вызывает метод OnStaminaExhausted, если выносливость равна нулю
        }
    }
}

bool UVitalSystemComponent::CheckStamina(float Val) { // Метод проверки, достаточно ли выносливости для выполнения действия с величиной Val
	return Stamina > Val; // Возвращает true, если текущая выносливость больше Val, иначе false
}

void UVitalSystemComponent::BeginPlay() { // Метод, вызываемый при начале игры для инициализации компонента
	Super::BeginPlay(); 
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &UVitalSystemComponent::PerformTimer, STAMINA_CLC_FACTOR, true); 
    // Настройка таймера для периодического вызова метода PerformTimer с указанным интервалом STAMINA_CLC_FACTOR
}

void UVitalSystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) { 

} 

void UVitalSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) { 
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction); 
} 

void UVitalSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const { 
    DOREPLIFETIME(UVitalSystemComponent, Health); 
    DOREPLIFETIME(UVitalSystemComponent, MaxHealth); 
    DOREPLIFETIME(UVitalSystemComponent, Stamina); 
    DOREPLIFETIME(UVitalSystemComponent, MaxStamina); 
    /* Настройка свойств для репликации по сети: здоровье и выносливость */
}
