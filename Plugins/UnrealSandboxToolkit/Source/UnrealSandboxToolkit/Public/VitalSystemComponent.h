// Copyright blackw 2015-2020 // Авторские права blackw 2015-2020

#pragma once // Указание компилятору включить этот файл только один раз

#include "Engine.h" // Подключение основного заголовочного файла движка Unreal Engine
#include "Components/ActorComponent.h" // Подключение заголовочного файла для компонентов акторов
#include "VitalSystemComponent.generated.h" // Генерация заголовочного файла для класса UVitalSystemComponent


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) ) // Определение класса UVitalSystemComponent, доступного в Blueprints
class UNREALSANDBOXTOOLKIT_API UVitalSystemComponent : public UActorComponent { // Определение класса UVitalSystemComponent, наследующего от UActorComponent
	GENERATED_BODY() // Генерация тела класса

public:

	UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health") // Свойство Health, реплицируемое и редактируемое в редакторе в категории "Sandbox Health"
	float Health; // Текущее здоровье

	UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health") // Свойство MaxHealth, реплицируемое и редактируемое в редакторе в категории "Sandbox Health"
	float MaxHealth; // Максимальное здоровье

	UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health") // Свойство Stamina, реплицируемое и редактируемое в редакторе в категории "Sandbox Health"
	float Stamina; // Текущая выносливость

	UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health") // Свойство MaxStamina, реплицируемое и редактируемое в редакторе в категории "Sandbox Health"
	float MaxStamina; // Максимальная выносливость

public:	
	UVitalSystemComponent(); // Конструктор класса UVitalSystemComponent

	virtual void BeginPlay() override; // Переопределение метода BeginPlay для инициализации при старте игры

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; // Переопределение метода EndPlay для обработки завершения игры
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override; // Переопределение метода TickComponent для обновления состояния компонента каждый кадр

public:

	float GetHealth(); // Метод для получения текущего здоровья

	float GetMaxHealth(); // Метод для получения максимального здоровья

	void ChangeHealth(float Val); // Метод для изменения здоровья на заданное значение

	void DamageFromFall(float Velocity); // Метод для обработки урона от падения с заданной скоростью

	void Damage(float DamageVal); // Метод для нанесения урона объекту

	float GetStamina(); // Метод для получения текущей выносливости

	float GetMaxStamina(); // Метод для получения максимальной выносливости

	void ChangeStamina(float Val); // Метод для изменения выносливости на заданное значение

	bool CheckStamina(float Val); // Метод для проверки достаточности выносливости на заданное значение

private:

	bool IsOwnerAdmin(); // Метод проверки, является ли владелец администратором

	FTimerHandle Timer; // Дескриптор таймера

	void PerformTimer(); // Метод выполнения действий по таймеру
};
