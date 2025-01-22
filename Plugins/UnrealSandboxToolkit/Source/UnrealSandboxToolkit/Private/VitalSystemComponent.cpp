// Copyright blackw 2015-2020

#include "VitalSystemComponent.h" // Подключаем заголовочный файл компонента системы жизненных показателей
#include "SandboxCharacter.h" // Подключаем заголовочный файл персонажа песочницы
#include "SandboxObject.h" // Подключаем заголовочный файл объектов песочницы
#include "Net/UnrealNetwork.h" // Подключаем заголовочный файл для сетевого взаимодействия

#define STAMINA_CLC_FACTOR 0.1 // Определяем коэффициент уменьшения выносливости
#define STAMINA_REDUCTION_FACTOR 0.25 // Определяем коэффициент уменьшения выносливости

UVitalSystemComponent::UVitalSystemComponent() { // Конструктор компонента жизненных показателей
	//	bWantsBeginPlay = true; // Устанавливаем флаг, что компонент хочет начать игру (закомментировано)
	PrimaryComponentTick.bCanEverTick = false; // Запрещаем компоненту обновляться каждый кадр

	Health = 100; // Устанавливаем начальное здоровье
	MaxHealth = 100; // Устанавливаем максимальное здоровье

	Stamina = 60; // Устанавливаем начальную выносливость
	MaxStamina = 60; // Устанавливаем максимальную выносливость
}

bool UVitalSystemComponent::IsOwnerAdmin() { // Проверяем, является ли владелец администратором
	return false; // Возвращаем false, т.е. владелец не администратор
}

float UVitalSystemComponent::GetHealth() { // Получаем текущее здоровье
	return Health; // Возвращаем текущее здоровье
}

float UVitalSystemComponent::GetMaxHealth() { // Получаем максимальное здоровье
	return MaxHealth; // Возвращаем максимальное здоровье
}

void UVitalSystemComponent::ChangeHealth(float Val){ // Изменяем здоровье на заданное значение
	Health += Val; // Увеличиваем или уменьшаем здоровье

	if (Health <= 0) { // Если здоровье меньше или равно нулю
		Health = 0; // Устанавливаем здоровье в 0
	}

	if (Health > MaxHealth) { // Если здоровье превышает максимальное
		Health = MaxHealth; // Устанавливаем здоровье на максимум
	}
}

void UVitalSystemComponent::DamageFromFall(float Velocity) { // Наносим урон от падения с заданной скоростью
	auto EquipmentList = ASandboxObjectUtils::GetContainerContent(GetOwner(), TEXT("Equipment")); // Получаем список экипировки владельца
	float DamageFallFactor = 1.f; // Начальный коэффициент урона от падения
	for (auto* Item : EquipmentList) { // Проходим по каждому предмету в списке оборудования
		ASandboxSkeletalModule* Skm = Cast<ASandboxSkeletalModule>(Item); // Приводим элемент к типу ASandboxSkeletalModule
		if (Skm) { // Если приведение прошло успешно
			DamageFallFactor *= Skm->GetAffectParam(TEXT("damage_fall_factor")); // Умножаем коэффициент урона на параметр урона от падения
		}
	}

	const float Dmg = Velocity * 100.f / 2000.f * DamageFallFactor; // Вычисляем урон от падения
	Damage(Dmg); // Наносим урон
}

void UVitalSystemComponent::Damage(float DamageVal) { // Наносим урон с заданным значением
	if (IsOwnerAdmin()) { // Если владелец является администратором
		return; // Прекращаем выполнение функции
	}

	ChangeHealth(-DamageVal); // Уменьшаем здоровье на величину урона
	ChangeStamina(-DamageVal); // Уменьшаем выносливость на величину урона

	if (Health == 0) { // Если здоровье равно нулю
		ISandboxCoreCharacter* SandboxCharacter = Cast<ISandboxCoreCharacter>(GetOwner()); // Приводим владельца к типу ISandboxCoreCharacter
		if (SandboxCharacter) { // Если приведение прошло успешно
			SandboxCharacter->Kill(); // Вызываем метод убийства персонажа
		}
	}
}

float UVitalSystemComponent::GetStamina() { // Получаем текущее значение выносливости
	return Stamina; // Возвращаем текущее значение выносливости
}

float UVitalSystemComponent::GetMaxStamina() { // Получаем максимальное значение выносливости
	return MaxStamina; // Возвращаем максимальное значение выносливости
}

void UVitalSystemComponent::ChangeStamina(float Val) { // Изменяем выносливость на заданное значение
	if (IsOwnerAdmin() && Val < 0) { // Если владелец администратор и значение уменьшения выносливости отрицательное
		return; // Прекращаем выполнение функции
	}

	Stamina += Val; // Увеличиваем или уменьшаем выносливость

	if (Stamina <= 0) { // Если выносливость меньше или равна нулю
		Stamina = 0; // Устанавливаем выносливость в 0
	}

	if (Stamina > MaxStamina) { // Если выносливость превышает максимальную
		Stamina = MaxStamina; // Устанавливаем выносливость на максимум
	}
}

void UVitalSystemComponent::PerformTimer() { // Выполняем действия каждый тик таймера
	ISandboxCoreCharacter* SandboxCharacter = Cast<ISandboxCoreCharacter>(GetOwner()); // Приводим владельца к типу ISandboxCoreCharacter
	if (SandboxCharacter) { // Если приведение прошло успешно
		auto EquipmentList = ASandboxObjectUtils::GetContainerContent(GetOwner(), TEXT("Equipment")); // Получаем список экипировки владельца

		float StaminaFactor = 1.f; // Начальный коэффициент выносливости
		float RecoverStaminaFactor = 1.f; // Начальный коэффициент восстановления выносливости

		for (auto* Item : EquipmentList) { // Проходим по каждому предмету в списке оборудования
			ASandboxSkeletalModule* Skm = Cast<ASandboxSkeletalModule>(Item); // Приводим элемент к типу ASandboxSkeletalModule
			if (Skm) { // Если приведение прошло успешно
				StaminaFactor *= Skm->GetAffectParam(TEXT("stamina_factor")); // Умножаем коэффициент выносливости на параметр выносливости
				RecoverStaminaFactor *= Skm->GetAffectParam(TEXT("recover_stamina_factor")); // Умножаем коэффициент восстановления выносливости на параметр восстановления выносливости
			}
		}

		const float D = -SandboxCharacter->GetStaminaTickDelta(); // Получаем изменение выносливости за тик
		if (D < 0) { // Если выносливость уменьшается
			const float TickStaminaReduction = D * StaminaFactor * STAMINA_CLC_FACTOR * STAMINA_REDUCTION_FACTOR; // Вычисляем уменьшение выносливости за тик
			ChangeStamina(TickStaminaReduction); // Изменяем выносливость
		} else { // В противном случае
			// recover stamina
			if (!SandboxCharacter->IsDead()) { // Если персонаж не мертв
				const float RecoverStamina = 1.f * RecoverStaminaFactor; // Вычисляем восстановление выносливости
				ChangeStamina(RecoverStamina); // Изменяем выносливость
			}
		}

		if (Stamina == 0) { // Если выносливость равна нулю
			SandboxCharacter->OnStaminaExhausted(); // Вызываем метод, когда выносливость исчерпана
		}
	}
}

bool UVitalSystemComponent::CheckStamina(float Val) { // Проверяем, достаточно ли выносливости
	return Stamina > Val; // Возвращаем true, если выносливости больше, чем заданное значение
}

void UVitalSystemComponent::BeginPlay() { // Метод, вызываемый в начале игры
	Super::BeginPlay(); // Вызываем метод родительского класса
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &UVitalSystemComponent::PerformTimer, STAMINA_CLC_FACTOR, true); // Устанавливаем таймер для выполнения метода PerformTimer
}

void UVitalSystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) { // Метод, вызываемый при завершении игры
	// Здесь можно добавить код для очистки, если это необходимо
}

void UVitalSystemComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) { // Метод, вызываемый каждый кадр
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction ); // Вызываем метод родительского класса
}

void UVitalSystemComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const { // Метод для репликации свойств
	DOREPLIFETIME(UVitalSystemComponent, Health); // Устанавливаем репликацию для здоровья
	DOREPLIFETIME(UVitalSystemComponent, MaxHealth); // Устанавливаем репликацию для максимального здоровья
	DOREPLIFETIME(UVitalSystemComponent, Stamina); // Устанавливаем репликацию для выносливости
	DOREPLIFETIME(UVitalSystemComponent, MaxStamina); // Устанавливаем репликацию для максимальной выносливости
}
