// Copyright blackw 2015-2020

// Подключаем необходимые заголовочные файлы
#include "VitalSysWidget.h"        // Заголовок для нашего виджета
#include "VitalSystemComponent.h"   // Компонент системы жизненных показателей
#include "SandboxCharacter.h"       // Класс персонажа

// Получить текущее здоровье (пока что не реализовано)
float USandboxVitalSysWidget::GetHealth() {
	return 0;
}

// Получить максимальное здоровье (пока что не реализовано)
float USandboxVitalSysWidget::GetMaxHealth() {
	return 0;
}

// Найти и вернуть компонент системы жизненных показателей у персонажа
UVitalSystemComponent* USandboxVitalSysWidget::GetVitalSystemComponent() {
	// Проверяем, есть ли у игрока управляемый персонаж
	if (GetOwningPlayer()->GetPawn()) {
		// Создаем массив для хранения компонентов
		TArray<UVitalSystemComponent*> Components;
		// Получаем все компоненты типа UVitalSystemComponent
		GetOwningPlayer()->GetPawn()->GetComponents<UVitalSystemComponent>(Components);
		// Возвращаем первый найденный компонент (если есть)
		for (UVitalSystemComponent* VitalSystemComponent : Components) {
			return VitalSystemComponent;
		}
	}

	// Если ничего не нашли, возвращаем nullptr
	return nullptr;
}

// Получить здоровье в процентах от максимального значения
float USandboxVitalSysWidget::GetHealthInPercent() {
	// Получаем компонент жизненных показателей
	UVitalSystemComponent* VitalSystemComponent = GetVitalSystemComponent();
	if (VitalSystemComponent) {
		// Получаем текущее здоровье
		const float Health = VitalSystemComponent->GetHealth();
		const float MaxHealth = VitalSystemComponent->GetMaxHealth();
		// Максимально возможное значение здоровья
		static const float AbsoluteMaxHealth = 1000.f;
		// Вычисляем процент здоровья
		const float Res = Health / AbsoluteMaxHealth;
		return Res;
	}
	
	// Если компонент не найден, возвращаем 100%
	return 1.f;
}

// Получить выносливость в процентах от максимального значения
float USandboxVitalSysWidget::GetStaminaInPercent() {
	// Получаем компонент жизненных показателей
	UVitalSystemComponent* VitalSystemComponent = GetVitalSystemComponent();
	if (VitalSystemComponent) {
		// Получаем текущую выносливость
		const float Stamina = VitalSystemComponent->GetStamina();
		// Максимально возможное значение выносливости
		static const float AbsoluteMaxStamina = 1000.f;
		// Вычисляем процент выносливости
		const float Res = Stamina / AbsoluteMaxStamina;
		return Res;
	}
	
	// Если компонент не найден, возвращаем 0%
	return 0.f;
}