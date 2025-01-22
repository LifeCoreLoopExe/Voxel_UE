<<<<<<< Updated upstream
// Copyright blackw 2015-2020

#include "VitalSysWidget.h" // Подключение заголовочного файла для виджета VitalSys
#include "VitalSystemComponent.h" // Подключение заголовочного файла для компонента VitalSystem
#include "SandboxCharacter.h" // Подключение заголовочного файла для персонажа Sandbox

float USandboxVitalSysWidget::GetHealth() { // Метод для получения текущего значения здоровья
	return 0; // Возвращает 0, так как реализация еще не завершена
}

float USandboxVitalSysWidget::GetMaxHealth() { // Метод для получения максимального значения здоровья
	return 0; // Возвращает 0, так как реализация еще не завершена
}

UVitalSystemComponent* USandboxVitalSysWidget::GetVitalSystemComponent() { // Метод для получения компонента VitalSystem из владельца виджета
	if (GetOwningPlayer()->GetPawn()) { // Проверка, есть ли у игрока пешка (Pawn)
		TArray<UVitalSystemComponent*> Components; // Создание массива для хранения компонентов VitalSystem
		GetOwningPlayer()->GetPawn()->GetComponents<UVitalSystemComponent>(Components); // Получение всех компонентов VitalSystem из пешки

		for (UVitalSystemComponent* VitalSystemComponent : Components) { // Проход по каждому компоненту в массиве
			return VitalSystemComponent; // Возвращает первый найденный компонент VitalSystem
		}
	}

	return nullptr; // Если компоненты не найдены, возвращает nullptr
}

float USandboxVitalSysWidget::GetHealthInPercent() { // Метод для получения текущего здоровья в процентах относительно абсолютного максимума
	UVitalSystemComponent* VitalSystemComponent = GetVitalSystemComponent(); // Получение компонента VitalSystem
	if (VitalSystemComponent) { // Проверка, существует ли компонент
		const float Health = VitalSystemComponent->GetHealth(); // Получение текущего значения здоровья
		const float MaxHealth = VitalSystemComponent->GetMaxHealth(); // Получение максимального значения здоровья
		static const float AbsoluteMaxHealth = 1000.f; // Определение абсолютного максимума здоровья
		const float Res = Health / AbsoluteMaxHealth; // Вычисление процента здоровья относительно абсолютного максимума
		return Res; // Возвращает процент здоровья
	}
	
	return 1.f; // Если компонент не найден, возвращает 1 (100%)
}

float USandboxVitalSysWidget::GetStaminaInPercent() { // Метод для получения текущей выносливости в процентах относительно абсолютного максимума
	UVitalSystemComponent* VitalSystemComponent = GetVitalSystemComponent(); // Получение компонента VitalSystem
	if (VitalSystemComponent) { // Проверка, существует ли компонент
		const float Stamina = VitalSystemComponent->GetStamina(); // Получение текущего значения выносливости
		static const float AbsoluteMaxStamina = 1000.f; // Определение абсолютного максимума выносливости
		const float Res = Stamina / AbsoluteMaxStamina; // Вычисление процента выносливости относительно абсолютного максимума
		return Res; // Возвращает процент выносливости
	}
	
	return 0.f; // Если компонент не найден, возвращает 0 (0%)
}
=======
// Copyright blackw 2015-2020

#include "VitalSysWidget.h" // Подключение заголовочного файла для виджета VitalSys
#include "VitalSystemComponent.h" // Подключение заголовочного файла для компонента VitalSystem
#include "SandboxCharacter.h" // Подключение заголовочного файла для персонажа Sandbox

float USandboxVitalSysWidget::GetHealth() { // Метод для получения текущего значения здоровья
	return 0; // Возвращает 0, так как реализация еще не завершена
}

float USandboxVitalSysWidget::GetMaxHealth() { // Метод для получения максимального значения здоровья
	return 0; // Возвращает 0, так как реализация еще не завершена
}

UVitalSystemComponent* USandboxVitalSysWidget::GetVitalSystemComponent() { // Метод для получения компонента VitalSystem из владельца виджета
	if (GetOwningPlayer()->GetPawn()) { // Проверка, есть ли у игрока пешка (Pawn)
		TArray<UVitalSystemComponent*> Components; // Создание массива для хранения компонентов VitalSystem
		GetOwningPlayer()->GetPawn()->GetComponents<UVitalSystemComponent>(Components); // Получение всех компонентов VitalSystem из пешки

		for (UVitalSystemComponent* VitalSystemComponent : Components) { // Проход по каждому компоненту в массиве
			return VitalSystemComponent; // Возвращает первый найденный компонент VitalSystem
		}
	}

	return nullptr; // Если компоненты не найдены, возвращает nullptr
}

float USandboxVitalSysWidget::GetHealthInPercent() { // Метод для получения текущего здоровья в процентах относительно абсолютного максимума
	UVitalSystemComponent* VitalSystemComponent = GetVitalSystemComponent(); // Получение компонента VitalSystem
	if (VitalSystemComponent) { // Проверка, существует ли компонент
		const float Health = VitalSystemComponent->GetHealth(); // Получение текущего значения здоровья
		const float MaxHealth = VitalSystemComponent->GetMaxHealth(); // Получение максимального значения здоровья
		static const float AbsoluteMaxHealth = 1000.f; // Определение абсолютного максимума здоровья
		const float Res = Health / AbsoluteMaxHealth; // Вычисление процента здоровья относительно абсолютного максимума
		return Res; // Возвращает процент здоровья
	}
	
	return 1.f; // Если компонент не найден, возвращает 1 (100%)
}

float USandboxVitalSysWidget::GetStaminaInPercent() { // Метод для получения текущей выносливости в процентах относительно абсолютного максимума
	UVitalSystemComponent* VitalSystemComponent = GetVitalSystemComponent(); // Получение компонента VitalSystem
	if (VitalSystemComponent) { // Проверка, существует ли компонент
		const float Stamina = VitalSystemComponent->GetStamina(); // Получение текущего значения выносливости
		static const float AbsoluteMaxStamina = 1000.f; // Определение абсолютного максимума выносливости
		const float Res = Stamina / AbsoluteMaxStamina; // Вычисление процента выносливости относительно абсолютного максимума
		return Res; // Возвращает процент выносливости
	}
	
	return 0.f; // Если компонент не найден, возвращает 0 (0%)
}
>>>>>>> Stashed changes
