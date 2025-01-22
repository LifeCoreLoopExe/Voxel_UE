// Copyright blackw 2015-2020

// Подключение заголовочных файлов
#include "VitalSysWidget.h"
#include "VitalSystemComponent.h"
#include "SandboxCharacter.h"

// Метод для получения текущего здоровья
float USandboxVitalSysWidget::GetHealth() {
    return 0; // Возвращает 0 (метод не реализован)
}

// Метод для получения максимального здоровья
float USandboxVitalSysWidget::GetMaxHealth() {
    return 0; // Возвращает 0 (метод не реализован)
}

// Метод для получения компонента системы жизненных показателей
UVitalSystemComponent* USandboxVitalSysWidget::GetVitalSystemComponent() {
    // Проверяем, есть ли у игрока персонаж
    if (GetOwningPlayer()->GetPawn()) {
        // Создаем массив для хранения компонентов системы жизненных показателей
        TArray<UVitalSystemComponent*> Components;
        // Получаем все компоненты системы жизненных показателей у персонажа игрока
        GetOwningPlayer()->GetPawn()->GetComponents<UVitalSystemComponent>(Components);
        // Проходим по всем компонентам и возвращаем первый найденный
        for (UVitalSystemComponent* VitalSystemComponent : Components) {
            return VitalSystemComponent;
        }
    }

    // Если компонент не найден, возвращаем nullptr
    return nullptr;
}

// Метод для получения здоровья в процентах
float USandboxVitalSysWidget::GetHealthInPercent() {
    // Получаем компонент системы жизненных показателей
    UVitalSystemComponent* VitalSystemComponent = GetVitalSystemComponent();
    if (VitalSystemComponent) {
        // Получаем текущее здоровье из компонента
        const float Health = VitalSystemComponent->GetHealth();
        // Получаем максимальное здоровье из компонента
        const float MaxHealth = VitalSystemComponent->GetMaxHealth();
        // Определяем абсолютное максимальное здоровье
        static const float AbsoluteMaxHealth = 1000.f;
        // Вычисляем здоровье в процентах от абсолютного максимального здоровья
        const float Res = Health / AbsoluteMaxHealth;
        // Возвращаем результат
        return Res;
    }

    // Если компонент не найден, возвращаем 1.0 (100%)
    return 1.f;
}

// Метод для получения выносливости в процентах
float USandboxVitalSysWidget::GetStaminaInPercent() {
    // Получаем компонент системы жизненных показателей
    UVitalSystemComponent* VitalSystemComponent = GetVitalSystemComponent();
    if (VitalSystemComponent) {
        // Получаем текущую выносливость из компонента
        const float Stamina = VitalSystemComponent->GetStamina();
        // Определяем абсолютное максимальное значение выносливости
        static const float AbsoluteMaxStamina = 1000.f;
        // Вычисляем выносливость в процентах от абсолютного максимального значения выносливости
        const float Res = Stamina / AbsoluteMaxStamina;
        // Возвращаем результат
        return Res;
    }

    // Если компонент не найден, возвращаем 0.0 (0%)
    return 0.f;
}
