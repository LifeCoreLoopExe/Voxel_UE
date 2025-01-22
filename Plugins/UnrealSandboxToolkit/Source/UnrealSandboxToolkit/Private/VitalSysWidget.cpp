// Copyright blackw 2015-2020

#include "VitalSysWidget.h" // Подключаем заголовочный файл VitalSysWidget.h
#include "VitalSystemComponent.h" // Подключаем заголовочный файл VitalSystemComponent.h
#include "SandboxCharacter.h" // Подключаем заголовочный файл SandboxCharacter.h

float USandboxVitalSysWidget::GetHealth() { // Определяем метод GetHealth, возвращающий здоровье
	return 0; // Возвращаем 0, так как пока не реализовано
}

float USandboxVitalSysWidget::GetMaxHealth() { // Определяем метод GetMaxHealth, возвращающий максимальное здоровье
	return 0; // Возвращаем 0, так как пока не реализовано
}

UVitalSystemComponent* USandboxVitalSysWidget::GetVitalSystemComponent() { // Определяем метод, возвращающий компонент VitalSystem
	if (GetOwningPlayer()->GetPawn()) { // Проверяем, есть ли у игрока персонаж (Pawn)
		TArray<UVitalSystemComponent*> Components; // Создаем массив для компонентов VitalSystem
		GetOwningPlayer()->GetPawn()->GetComponents<UVitalSystemComponent>(Components); // Получаем все компоненты VitalSystem у персонажа
		for (UVitalSystemComponent* VitalSystemComponent : Components) { // Проходим по всем найденным компонентам
			return VitalSystemComponent; // Возвращаем первый найденный компонент
		}
	}

	return nullptr; // Если компонент не найден, возвращаем nullptr
}

float USandboxVitalSysWidget::GetHealthInPercent() { // Определяем метод, возвращающий здоровье в процентах

	UVitalSystemComponent* VitalSystemComponent = GetVitalSystemComponent(); // Получаем компонент VitalSystem
	if (VitalSystemComponent) { // Если компонент найден
		const float Health = VitalSystemComponent->GetHealth(); // Получаем текущее здоровье
		const float MaxHealth = VitalSystemComponent->GetMaxHealth(); // Получаем максимальное здоровье
		static const float AbsoluteMaxHealth = 1000.f; // Определяем абсолютное максимальное здоровье
		const float Res = Health / AbsoluteMaxHealth; // Вычисляем процент здоровья
		return Res; // Возвращаем процент здоровья
	}
	
	return 1.f; // Если компонент не найден, возвращаем 1 (100%)
}

float USandboxVitalSysWidget::GetStaminaInPercent() { // Определяем метод, возвращающий выносливость в процентах
	
	UVitalSystemComponent* VitalSystemComponent = GetVitalSystemComponent(); // Получаем компонент VitalSystem
	if (VitalSystemComponent) { // Если компонент найден
		const float Stamina = VitalSystemComponent->GetStamina(); // Получаем текущую выносливость
		static const float AbsoluteMaxStamina = 1000.f; // Определяем абсолютное максимальное значение выносливости
		const float Res = Stamina / AbsoluteMaxStamina; // Вычисляем процент выносливости
		return Res; // Возвращаем процент выносливости
	}
	
	return 0.f; // Если компонент не найден, возвращаем 0 (0%)
}
