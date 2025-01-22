// Copyright blackw 2015-2020

#pragma once  // Защита от повторного включения заголовочного файла

// Подключаем необходимые заголовочные файлы
#include "Engine.h"
#include "Components/ActorComponent.h"
#include "VitalSystemComponent.generated.h"

// Объявляем класс компонента, который можно добавлять к актерам в редакторе
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALSANDBOXTOOLKIT_API UVitalSystemComponent : public UActorComponent {
	GENERATED_BODY() // Макрос Unreal Engine для поддержки рефлексии

public:
	// Здоровье персонажа (можно менять в редакторе, синхронизируется по сети)
	UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health")
	float Health;

	// Максимальное здоровье персонажа
	UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health")
	float MaxHealth;

	// Выносливость персонажа
	UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health")
	float Stamina;

	// Максимальная выносливость персонажа
	UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health")
	float MaxStamina;

public:	
	// Конструктор компонента
	UVitalSystemComponent();

	// Вызывается при старте игры
	virtual void BeginPlay() override;

	// Вызывается при завершении работы компонента
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	// Вызывается каждый кадр
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

public:
	// Получить текущее здоровье
	float GetHealth();

	// Получить максимальное здоровье
	float GetMaxHealth();

	// Изменить количество здоровья на заданное значение
	void ChangeHealth(float Val);

	// Получить урон от падения
	void DamageFromFall(float Velocity);

	// Получить урон заданной величины
	void Damage(float DamageVal);

	// Получить текущую выносливость
	float GetStamina();

	// Получить максимальную выносливость
	float GetMaxStamina();

	// Изменить количество выносливости на заданное значение
	void ChangeStamina(float Val);

	// Проверить, достаточно ли выносливости
	bool CheckStamina(float Val);

private:
	// Проверка, является ли владелец администратором
	bool IsOwnerAdmin();

	// Таймер для периодических действий
	FTimerHandle Timer;

	// Функция, выполняемая по таймеру
	void PerformTimer();

};
