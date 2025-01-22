// Авторские права blackw 2015-2020

#pragma once // Директива препроцессора для предотвращения множественного включения заголовочного файла

#include "Engine.h" // Подключение основного движка Unreal Engine
#include "Components/ActorComponent.h" // Подключение базового класса компонента актера
#include "VitalSystemComponent.generated.h" // Генерация метаданных для редактора Unreal Engine

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent)) // Макрос для определения пользовательского класса, который можно создавать в Blueprint
class UNREALSANDBOXTOOLKIT_API UVitalSystemComponent : public UActorComponent { // Объявление класса UVitalSystemComponent, наследуемого от UActorComponent
    GENERATED_BODY() // Макрос для генерации тела класса

public:

    UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health") // Макрос для определения свойства, которое реплицируется и редактируется в редакторе
    float Health; // Здоровье

    UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health") // Макрос для определения свойства, которое реплицируется и редактируется в редакторе
    float MaxHealth; // Максимальное здоровье

    UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health") // Макрос для определения свойства, которое реплицируется и редактируется в редакторе
    float Stamina; // Выносливость

    UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health") // Макрос для определения свойства, которое реплицируется и редактируется в редакторе
    float MaxStamina; // Максимальная выносливость

public:
    UVitalSystemComponent(); // Конструктор класса

    virtual void BeginPlay() override; // Метод, вызываемый при начале игры

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; // Метод, вызываемый при завершении игры

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override; // Метод, вызываемый каждый тик игры

public:

    float GetHealth(); // Метод для получения текущего здоровья

    float GetMaxHealth(); // Метод для получения максимального здоровья

    void ChangeHealth(float Val); // Метод для изменения здоровья

    void DamageFromFall(float Velocity); // Метод для нанесения урона от падения

    void Damage(float DamageVal); // Метод для нанесения урона

    float GetStamina(); // Метод для получения текущей выносливости

    float GetMaxStamina(); // Метод для получения максимальной выносливости

    void ChangeStamina(float Val); // Метод для изменения выносливости

    bool CheckStamina(float Val); // Метод для проверки выносливости

private:

    bool IsOwnerAdmin(); // Метод для проверки, является ли владелец администратором

    FTimerHandle Timer; // Дескриптор таймера

    void PerformTimer(); // Метод для выполнения действий таймера

};
