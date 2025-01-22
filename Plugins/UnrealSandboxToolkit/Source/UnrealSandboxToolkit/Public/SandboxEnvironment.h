// Этот файл содержит заголовочный класс для управления окружением в игре (день/ночь, освещение, туман и т.д.)

#pragma once

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "SandboxEnvironment.generated.h"

// Структура для хранения игрового времени
// Содержит дни, часы, минуты, секунды, месяц и год
struct TSandboxGameTime {
	int days;      // Количество дней
	int hours;     // Количество часов
	int minutes;   // Количество минут
	int seconds;   // Количество секунд

	int month;     // Номер месяца
	int year;      // Год
};


// Основной класс для управления окружением, наследуется от базового класса Actor
UCLASS()
class UNREALSANDBOXTOOLKIT_API ASandboxEnvironment : public AActor
{
	GENERATED_BODY()
	
public:	
	// Конструктор класса
	ASandboxEnvironment();

	// Функции, которые вызываются при старте игры и каждый кадр
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	// Ссылки на основные компоненты освещения и окружения
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	ADirectionalLight* DirectionalLightSource;    // Основной источник направленного света (солнце)

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	ASkyLight* SkyLight;                         // Освещение неба

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	AExponentialHeightFog* GlobalFog;            // Глобальный туман

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	AStaticMeshActor* CaveSphere;                // Сфера для пещер

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	AAmbientSound* AmbientSound;                 // Фоновый звук

	// Различные кривые для настройки эффектов
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	UCurveFloat* HeightCurve;                    // Кривая высоты

	// Настройки для пещер
	UPROPERTY(EditAnywhere, Category = "Sandbox Cave")
	float CaveSkyLightIntensity;                 // Интенсивность освещения в пещерах

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	UCurveFloat* CaveSunLightCurve;              // Кривая освещения в пещерах

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	UCurveFloat* GlobalFogDensityCurve;          // Кривая плотности тумана

	// Настройки скорости времени и тумана в пещерах
	UPROPERTY(EditAnywhere, Replicated, Category = "Sandbox")
	float TimeSpeed;                             // Скорость течения времени

	UPROPERTY(EditAnywhere, Category = "Sandbox Cave")
	float CaveFogDensity;                        // Плотность тумана в пещерах

	// Настройки цикла день/ночь
	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
	bool bEnableDayNightCycle;                   // Включить/выключить смену дня и ночи

	// Начальные настройки даты
	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
	int InitialYear = 2016;                      // Начальный год

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
	int InitialMonth = 6;                        // Начальный месяц

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
	int InitialDay = 10;                         // Начальный день

	// Географические настройки
	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
	int TimeZone;                                // Часовой пояс

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
	float Lat;                                   // Широта

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
	float Lng;                                   // Долгота

	UPROPERTY(Replicated)
	double RealTimeOffset = 0;                   // Смещение реального времени

	// Функции для работы со временем
	float ClcGameTime(float RealServerTime);                                    // Расчет игрового времени
	TSandboxGameTime ClcLocalGameTime(float RealServerTime);                   // Расчет локального игрового времени
	TSandboxGameTime ClcGameTimeOfDay(float RealServerTime, bool bAccordingTimeZone); // Расчет времени суток
	TSandboxGameTime ClcGameTimeOfDay();                                       // Получение текущего времени суток

	// Функции управления
	void SetTimeOffset(float time);              // Установка смещения времени
	double GetNewTimeOffset();                   // Получение нового смещения времени
	virtual void UpdatePlayerPosition(FVector Pos, APlayerController* Controller); // Обновление позиции игрока
	void SetCaveMode(bool bCaveModeEnabled);     // Включение/выключение режима пещеры
	bool IsCaveMode();                           // Проверка режима пещеры
	bool IsNight() const;                        // Проверка, ночь ли сейчас

	// Функция для получения текущего времени в виде строки (доступна из Blueprint)
	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	FString GetCurrentTimeAsString();
	
protected:
	// Защищенные функции
	virtual float ClcHeightFactor() const;       // Расчет фактора высоты

private:
	// Приватные переменные
	bool bIsNight = false;                       // Флаг ночного времени
	bool bCaveMode = false;                      // Флаг режима пещеры
	float LastTime;                              // Последнее время
	float InitialFogOpacity;                     // Начальная прозрачность тумана
	float InitialFogDensity;                     // Начальная плотность тумана
	void PerformDayNightCycle();                 // Выполнение цикла день/ночь
	FVector PlayerPos;                           // Позиция игрока
	float InitialSkyIntensity;                   // Начальная интенсивность неба
	float InitialSunIntensity;                   // Начальная интенсивность солнца
};
