// Copyright blackw 2015-2020

#include "VitalSysWidget.h" // Включает заголовочный файл для VitalSysWidget
#include "VitalSystemComponent.h" // Включает заголовочный файл для VitalSystemComponent
#include "SandboxCharacter.h" // Включает заголовочный файл для SandboxCharacter

float USandboxVitalSysWidget::GetHealth() {
    return 0; // Возвращает 0 (заглушка)
}

float USandboxVitalSysWidget::GetMaxHealth() {
    return 0; // Возвращает 0 (заглушка)
}

UVitalSystemComponent* USandboxVitalSysWidget::GetVitalSystemComponent() {
    if (GetOwningPlayer()->GetPawn()) { // Проверяет, есть ли у игрока аватар
        TArray<UVitalSystemComponent*> Components;
        GetOwningPlayer()->GetPawn()->GetComponents<UVitalSystemComponent>(Components); // Получает все компоненты VitalSystemComponent у аватара
        for (UVitalSystemComponent* VitalSystemComponent : Components) {
            return VitalSystemComponent; // Возвращает первый найденный компонент
        }
    }
    return nullptr; // Возвращает nullptr, если компонент не найден
}

float USandboxVitalSysWidget::GetHealthInPercent() {
    UVitalSystemComponent* VitalSystemComponent = GetVitalSystemComponent();
    if (VitalSystemComponent) {
        const float Health = VitalSystemComponent->GetHealth();
        const float MaxHealth = VitalSystemComponent->GetMaxHealth();
        static const float AbsoluteMaxHealth = 1000.f;
        const float Res = Health / AbsoluteMaxHealth;
        return Res; // Возвращает здоровье в процентах
    }
    return 1.f; // Возвращает 1.0, если компонент не найден
}

float USandboxVitalSysWidget::GetStaminaInPercent() {
    UVitalSystemComponent* VitalSystemComponent = GetVitalSystemComponent();
    if (VitalSystemComponent) {
        const float Stamina = VitalSystemComponent->GetStamina();
        static const float AbsoluteMaxStamina = 1000.f;
        const float Res = Stamina / AbsoluteMaxStamina;
        return Res; // Возвращает выносливость в процентах
    }
    return 0.f; // Возвращает 0.0, если компонент не найден
}
