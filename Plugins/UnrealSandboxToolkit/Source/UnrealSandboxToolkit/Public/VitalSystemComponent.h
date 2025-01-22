// Copyright blackw 2015-2020

#pragma once // Защита от множественного включения этого заголовочного файла

#include "Engine.h" // Подключение основного заголовочного файла движка Unreal Engine
#include "Components/ActorComponent.h" // Подключение заголовочного файла для компонентов акторов
#include "VitalSystemComponent.generated.h" // Генерация кода для этого заголовочного файла

// Класс компонента системы жизней, наследующий от UActorComponent
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent)) 
class UNREALSANDBOXTOOLKIT_API UVitalSystemComponent : public UActorComponent {
	GENERATED_BODY() // Генерация тела класса

public:
	// Переменные для хранения здоровья и выносливости
	UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health") 
	float Health; // Текущее здоровье

	UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health") 
	float MaxHealth; // Максимальное здоровье

	UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health") 
	float Stamina; // Текущая выносливость

	UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health") 
	float MaxStamina; // Максимальная выносливость

public:	
	UVitalSystemComponent(); // Конструктор класса

	virtual void BeginPlay() override; // Переопределение метода BeginPlay для инициализации при старте игры

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; // Переопределение метода EndPlay для завершения работы компонента
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override; 
    // Переопределение метода TickComponent для обновления состояния компонента каждый кадр

public:
	// Методы для работы с здоровьем и выносливостью
	float GetHealth(); // Метод для получения текущего значения здоровья

	float GetMaxHealth(); // Метод для получения максимального значения здоровья

	void ChangeHealth(float Val); // Метод для изменения значения здоровья на заданное значение Val

	void DamageFromFall(float Velocity); // Метод для расчета урона от падения на основе скорости падения

	void Damage(float DamageVal); // Метод для нанесения урона на заданное значение DamageVal

	float GetStamina(); // Метод для получения текущего значения выносливости

	float GetMaxStamina(); // Метод для получения максимального значения выносливости

	void ChangeStamina(float Val); // Метод для изменения значения выносливости на заданное значение Val

	bool CheckStamina(float Val); // Метод проверки, достаточно ли выносливости для выполнения действия с величиной Val

private:
	bool IsOwnerAdmin(); // Метод проверки, является ли владелец администратором

	FTimerHandle Timer; // Таймер для выполнения периодических действий

	void PerformTimer(); // Метод, выполняемый по таймеру
};
