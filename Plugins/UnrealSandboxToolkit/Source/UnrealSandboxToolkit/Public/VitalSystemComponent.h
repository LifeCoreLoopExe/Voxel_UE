// Copyright blackw 2015-2020

#pragma once // Включает файл только один раз

#include "Engine.h" // Включает основные функции движка
#include "Components/ActorComponent.h" // Включает компонент актера
#include "VitalSystemComponent.generated.h" // Включает сгенерированный заголовочный файл для VitalSystemComponent

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent)) // Объявление класса, доступного для рефлексии UObject и Blueprint
class UNREALSANDBOXTOOLKIT_API UVitalSystemComponent : public UActorComponent { // Объявление класса UVitalSystemComponent, наследующегося от UActorComponent
    GENERATED_BODY() // Макрос для генерации тела класса

public:
    UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health") // Свойство, доступное для редактирования в редакторе и репликации
    float Health; // Текущее здоровье

    UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health") // Свойство, доступное для редактирования в редакторе и репликации
    float MaxHealth; // Максимальное здоровье

    UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health") // Свойство, доступное для редактирования в редакторе и репликации
    float Stamina; // Текущая выносливость

    UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health") // Свойство, доступное для редактирования в редакторе и репликации
    float MaxStamina; // Максимальная выносливость

public:
    UVitalSystemComponent(); // Конструктор

    virtual void BeginPlay() override; // Метод, вызываемый при начале игры

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; // Метод, вызываемый при завершении игры

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override; // Метод, вызываемый каждый тик

public:
    float GetHealth(); // Метод для получения текущего здоровья

    float GetMaxHealth(); // Метод для получения максимального здоровья

    void ChangeHealth(float Val); // Метод для изменения здоровья

    void DamageFromFall(float Velocity); // Метод для получения урона от падения

    void Damage(float DamageVal); // Метод для получения урона

    float GetStamina(); // Метод для получения текущей выносливости

    float GetMaxStamina(); // Метод для получения максимальной выносливости

    void ChangeStamina(float Val); // Метод для изменения выносливости

    bool CheckStamina(float Val); // Метод для проверки выносливости

private:
    bool IsOwnerAdmin(); // Метод для проверки, является ли владелец администратором

    FTimerHandle Timer; // Дескриптор таймера

    void PerformTimer(); // Метод для выполнения таймера
};
