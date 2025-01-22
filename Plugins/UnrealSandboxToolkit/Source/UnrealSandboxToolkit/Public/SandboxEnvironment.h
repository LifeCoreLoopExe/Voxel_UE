// Fill out your copyright notice in the Description page of Project Settings.

#pragma once // Защита от множественного включения этого заголовочного файла

#include "Engine.h" // Подключение основного заголовочного файла движка Unreal Engine
#include "GameFramework/Actor.h" // Подключение заголовочного файла для класса Actor
#include "SandboxEnvironment.generated.h" // Генерация кода для этого заголовочного файла

// Структура для хранения информации о времени в игре
struct TSandboxGameTime {
	int days; // Количество дней
	int hours; // Часы
	int minutes; // Минуты
	int seconds; // Секунды

	int month; // Месяц
	int year; // Год
};

// Класс для управления окружением в песочнице, наследующий от AActor
UCLASS() 
class UNREALSANDBOXTOOLKIT_API ASandboxEnvironment : public AActor {
	GENERATED_BODY() // Генерация тела класса
	
public:	
	ASandboxEnvironment(); // Конструктор класса

	virtual void BeginPlay() override; // Переопределение метода BeginPlay для инициализации при старте игры
	
	virtual void Tick(float DeltaSeconds) override; // Переопределение метода Tick для обновления состояния окружения каждый кадр

	// Объекты освещения и эффектов окружения
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	ADirectionalLight* DirectionalLightSource; // Направленный источник света

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	ASkyLight* SkyLight; // Небесный свет

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	AExponentialHeightFog* GlobalFog; // Глобальный эффект тумана

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	AStaticMeshActor* CaveSphere; // Сфера пещеры (статический меш)

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	AAmbientSound* AmbientSound; // Фоновый звук

	//UPROPERTY(EditAnywhere, Category = "Sandbox")
	//UCurveFloat* DayNightCycleSkyLightCurve; // Кривая для освещения в цикле день-ночь (закомментировано)

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	UCurveFloat* HeightCurve; // Кривая высоты (для управления высотой)

	//UPROPERTY(EditAnywhere, Category = "Sandbox Cave")
	//float CaveSkyLightRatio; // Соотношение небесного света в пещере (закомментировано)

	UPROPERTY(EditAnywhere, Category = "Sandbox Cave")
	float CaveSkyLightIntensity; // Интенсивность небесного света в пещере

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	UCurveFloat* CaveSunLightCurve; // Кривая для солнечного света в пещере

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	UCurveFloat* GlobalFogDensityCurve; // Кривая плотности глобального тумана

	//UPROPERTY(EditAnywhere, Category = "Sandbox")
	//UCurveFloat* GlobalFogOpacityCurve; // Кривая непрозрачности глобального тумана (закомментировано)

	UPROPERTY(EditAnywhere, Replicated, Category = "Sandbox") 
	float TimeSpeed; // Скорость времени в игре

	UPROPERTY(EditAnywhere, Category = "Sandbox Cave")
	float CaveFogDensity; // Плотность тумана в пещере

	//UPROPERTY(EditAnywhere, Category = "Sandbox Cave")
	//float CaveFogOpacity; // Непрозрачность тумана в пещере (закомментировано)

	//UPROPERTY(EditAnywhere, Category = "Sandbox Cave")
	//FLinearColor CaveFogInscatteringColor; // Цвет рассеивания тумана в пещере (закомментировано)

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
	bool bEnableDayNightCycle; // Флаг включения цикла день-ночь

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
	int InitialYear = 2016; // Начальный год

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
	int InitialMonth = 6; // Начальный месяц

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
	int InitialDay = 10; // Начальный день

	//UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
	//int InitialHour = 12; // Начальный час (закомментировано)

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
	int TimeZone; // Часовой пояс

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
	float Lat; // Широта

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
	float Lng; // Долгота

	UPROPERTY(Replicated) 
	double RealTimeOffset = 0; // Смещение реального времени

	float ClcGameTime(float RealServerTime); // Метод для расчета игрового времени на основе реального времени

	TSandboxGameTime ClcLocalGameTime(float RealServerTime); // Метод для расчета локального игрового времени на основе реального времени

	TSandboxGameTime ClcGameTimeOfDay(float RealServerTime, bool bAccordingTimeZone); 
    // Метод для расчета игрового времени суток на основе реального времени и часового пояса

	TSandboxGameTime ClcGameTimeOfDay(); // Метод для расчета игрового времени суток без параметров

	void SetTimeOffset(float time); // Метод для установки смещения времени

	double GetNewTimeOffset(); // Метод для получения нового смещения времени

	virtual void UpdatePlayerPosition(FVector Pos, APlayerController* Controller); 
    // Метод для обновления позиции игрока в зависимости от параметров окружения

	void SetCaveMode(bool bCaveModeEnabled); // Метод для установки режима пещеры

	bool IsCaveMode(); // Метод проверки режима пещеры

	bool IsNight() const; // Метод проверки ночного времени 

	UFUNCTION(BlueprintCallable, Category = "Sandbox") 
	FString GetCurrentTimeAsString(); 
    // Метод для получения текущего времени как строки
	
protected:
	virtual float ClcHeightFactor() const; 
    // Виртуальный метод для расчета фактора высоты (может быть переопределен в подклассах)

private:
	bool bIsNight = false;  // Флаг ночного времени 

	bool bCaveMode = false;  // Флаг режима пещеры 

	float LastTime;  // Переменная для хранения последнего времени 

	float InitialFogOpacity;  // Начальная непрозрачность тумана 

	float InitialFogDensity;  // Начальная плотность тумана 

	void PerformDayNightCycle(); 
    /* Метод для выполнения цикла день-ночь */

	FVector PlayerPos;  /* Позиция игрока */

	float InitialSkyIntensity;  /* Начальная интенсивность неба */

	float InitialSunIntensity;  /* Начальная интенсивность солнечного света */
};
