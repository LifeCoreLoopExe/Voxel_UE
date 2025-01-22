// Copyright blackw 2015-2020

#include "VitalSystemComponent.h"
#include "SandboxCharacter.h"
#include "SandboxObject.h"
#include "Net/UnrealNetwork.h"

// Константы для расчета выносливости
#define STAMINA_CLC_FACTOR 0.1
#define STAMINA_REDUCTION_FACTOR 0.25

// Конструктор компонента системы жизнеобеспечения
UVitalSystemComponent::UVitalSystemComponent() {
	// Отключаем автоматический вызов BeginPlay
	//	bWantsBeginPlay = true;
	// Отключаем автоматическое обновление компонента каждый кадр
	PrimaryComponentTick.bCanEverTick = false;

	// Устанавливаем начальные значения здоровья
	Health = 100;
	MaxHealth = 100;

	// Устанавливаем начальные значения выносливости
	Stamina = 60;
	MaxStamina = 60;
}

// Проверка, является ли владелец администратором
bool UVitalSystemComponent::IsOwnerAdmin() {
	return false;
}

// Получение текущего значения здоровья
float UVitalSystemComponent::GetHealth() {
	return Health;
}

// Получение максимального значения здоровья
float UVitalSystemComponent::GetMaxHealth() {
	return MaxHealth;
}

// Изменение значения здоровья
void UVitalSystemComponent::ChangeHealth(float Val){
	Health += Val;

	// Проверяем, чтобы здоровье не ушло в минус
	if (Health <= 0) { 
		Health = 0; 
	}

	// Проверяем, чтобы здоровье не превысило максимум
	if (Health > MaxHealth) { 
		Health = MaxHealth; 
	}
}

// Получение урона от падения
void UVitalSystemComponent::DamageFromFall(float Velocity) {
	// Получаем список экипированных предметов
	auto EquipmentList = ASandboxObjectUtils::GetContainerContent(GetOwner(), TEXT("Equipment"));
	float DamageFallFactor = 1.f;
	
	// Проверяем все предметы на наличие модификаторов урона от падения
	for (auto* Item : EquipmentList) {
		ASandboxSkeletalModule* Skm = Cast<ASandboxSkeletalModule>(Item);
		if (Skm) {
			DamageFallFactor *= Skm->GetAffectParam(TEXT("damage_fall_factor"));
		}
	}

	// Рассчитываем и наносим урон от падения
	const float Dmg = Velocity * 100.f / 2000.f * DamageFallFactor;
	Damage(Dmg);
}

// Получение урона
void UVitalSystemComponent::Damage(float DamageVal) {
	// Админы не получают урон
	if (IsOwnerAdmin()) {
		return; 
	}

	// Уменьшаем здоровье и выносливость
	ChangeHealth(-DamageVal);
	ChangeStamina(-DamageVal);

	// Если здоровье закончилось - убиваем персонажа
	if (Health == 0) {
		ISandboxCoreCharacter* SandboxCharacter = Cast<ISandboxCoreCharacter>(GetOwner());
		if (SandboxCharacter) {
			SandboxCharacter->Kill();
		}
	}
}

// Получение текущего значения выносливости
float UVitalSystemComponent::GetStamina() {
	return Stamina;
}

// Получение максимального значения выносливости
float UVitalSystemComponent::GetMaxStamina() {
	return MaxStamina;
}

// Изменение значения выносливости
void UVitalSystemComponent::ChangeStamina(float Val) {
	// Админы не теряют выносливость
	if (IsOwnerAdmin() && Val < 0) { 
		return; 
	}

	Stamina += Val;

	// Проверяем границы выносливости
	if (Stamina <= 0) { 
		Stamina = 0; 
	}

	if (Stamina > MaxStamina) { 
		Stamina = MaxStamina;
	}
}

// Периодическое обновление состояния
void UVitalSystemComponent::PerformTimer() {
	ISandboxCoreCharacter* SandboxCharacter = Cast<ISandboxCoreCharacter>(GetOwner());
	if (SandboxCharacter) {
		// Получаем список экипировки
		auto EquipmentList = ASandboxObjectUtils::GetContainerContent(GetOwner(), TEXT("Equipment"));

		float StaminaFactor = 1.f;
		float RecoverStaminaFactor = 1.f;

		// Проверяем модификаторы выносливости от экипировки
		for (auto* Item : EquipmentList) {
			ASandboxSkeletalModule* Skm = Cast<ASandboxSkeletalModule>(Item);
			if (Skm) {
				StaminaFactor *= Skm->GetAffectParam(TEXT("stamina_factor"));
				RecoverStaminaFactor *= Skm->GetAffectParam(TEXT("recover_stamina_factor"));
			}
		}

		// Получаем изменение выносливости
		const float D = -SandboxCharacter->GetStaminaTickDelta();
		if (D < 0) {
			// Уменьшаем выносливость при активности
			const float TickStaminaReduction = D * StaminaFactor * STAMINA_CLC_FACTOR * STAMINA_REDUCTION_FACTOR;
			ChangeStamina(TickStaminaReduction);
		} else {
			// Восстанавливаем выносливость если персонаж жив
			if (!SandboxCharacter->IsDead()) {
				const float RecoverStamina = 1.f * RecoverStaminaFactor;
				ChangeStamina(RecoverStamina);
			}
		}

		// Если выносливость на нуле - вызываем событие истощения
		if (Stamina == 0) {
			SandboxCharacter->OnStaminaExhausted();
		}
	}
}

// Проверка достаточности выносливости
bool UVitalSystemComponent::CheckStamina(float Val) {
	return Stamina > Val;
}

// Инициализация компонента
void UVitalSystemComponent::BeginPlay() {
	Super::BeginPlay();
	// Запускаем таймер обновления состояния
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &UVitalSystemComponent::PerformTimer, STAMINA_CLC_FACTOR, true);
}

// Завершение работы компонента
void UVitalSystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {

}

// Обновление компонента каждый кадр (не используется)
void UVitalSystemComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) {
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

// Настройка сетевой репликации переменных
void UVitalSystemComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	// Регистрируем переменные для синхронизации по сети
	DOREPLIFETIME(UVitalSystemComponent, Health);
	DOREPLIFETIME(UVitalSystemComponent, MaxHealth);
	DOREPLIFETIME(UVitalSystemComponent, Stamina);
	DOREPLIFETIME(UVitalSystemComponent, MaxStamina);
}