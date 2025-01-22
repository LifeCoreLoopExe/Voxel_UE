// Fill out your copyright notice in the Description page of Project Settings. // Заполните уведомление об авторских правах на странице описания в настройках проекта.

#pragma once // Указание компилятору включить этот файл только один раз

#include "Engine.h" // Подключение основного заголовочного файла движка Unreal Engine
#include "GameFramework/Actor.h" // Подключение заголовочного файла для класса AActor
#include "SandboxEnvironment.generated.h" // Генерация заголовочного файла для класса ASandboxEnvironment

struct TSandboxGameTime { // Определение структуры для хранения времени в игре
	int days; // Количество дней
	int hours; // Количество часов
	int minutes; // Количество минут
	int seconds; // Количество секунд

	int month; // Месяц
	int year; // Год
};


UCLASS() // Определение класса ASandboxEnvironment, доступного в Blueprints
class UNREALSANDBOXTOOLKIT_API ASandboxEnvironment : public AActor // Определение класса ASandboxEnvironment, наследующего от AActor
{
	GENERATED_BODY() // Генерация тела класса
	
public:	
	ASandboxEnvironment(); // Конструктор класса ASandboxEnvironment

	virtual void BeginPlay() override; // Переопределение метода BeginPlay для инициализации при старте игры
	
	virtual void Tick(float DeltaSeconds) override; // Переопределение метода Tick для обновления состояния каждый кадр

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, редактируемое в редакторе, в категории "Sandbox"
	ADirectionalLight* DirectionalLightSource; // Источник направленного света

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, редактируемое в редакторе, в категории "Sandbox"
	ASkyLight* SkyLight; // Источник света для неба

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, редактируемое в редакторе, в категории "Sandbox"
	AExponentialHeightFog* GlobalFog; // Глобальный экспоненциальный туман

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, редактируемое в редакторе, в категории "Sandbox"
	AStaticMeshActor* CaveSphere; // Сфера пещеры (статический меш)

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, редактируемое в редакторе, в категории "Sandbox"
	AAmbientSound* AmbientSound; // Фоновый звук

	//UPROPERTY(EditAnywhere, Category = "Sandbox") 
	//UCurveFloat* DayNightCycleSkyLightCurve; // Закомментированное свойство для кривой освещения неба в цикле день-ночь

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, редактируемое в редакторе, в категории "Sandbox"
	UCurveFloat* HeightCurve; // Кривая высоты

	//UPROPERTY(EditAnywhere, Category = "Sandbox Cave") 
	//float CaveSkyLightRatio; // Закомментированное свойство для соотношения света неба пещеры

	UPROPERTY(EditAnywhere, Category = "Sandbox Cave") // Свойство, редактируемое в редакторе, в категории "Sandbox Cave"
	float CaveSkyLightIntensity; // Интенсивность света неба пещеры

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, редактируемое в редакторе, в категории "Sandbox"
	UCurveFloat* CaveSunLightCurve; // Кривая солнечного света пещеры

	UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, редактируемое в редакторе, в категории "Sandbox"
	UCurveFloat* GlobalFogDensityCurve; // Кривая плотности глобального тумана

	//UPROPERTY(EditAnywhere, Category = "Sandbox") 
	//UCurveFloat* GlobalFogOpacityCurve; // Закомментированное свойство для кривой непрозрачности глобального тумана

	UPROPERTY(EditAnywhere, Replicated, Category = "Sandbox") // Свойство с репликацией (синхронизация между клиентом и сервером), редактируемое в редакторе
	float TimeSpeed; // Скорость времени

	UPROPERTY(EditAnywhere, Category = "Sandbox Cave") // Свойство плотности тумана пещеры
	float CaveFogDensity; 

	//UPROPERTY(EditAnywhere, Category = "Sandbox Cave")
	//float CaveFogOpacity;  // Закомментированное свойство для непрозрачности тумана пещеры

	//UPROPERTY(EditAnywhere, Category = "Sandbox Cave")
	//FLinearColor CaveFogInscatteringColor;  // Закомментированное свойство для цвета рассеивания тумана пещеры

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle") // Свойство для включения цикла день-ночь
	bool bEnableDayNightCycle; 

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")  // Начальный год
	int InitialYear = 2016;

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")  // Начальный месяц
	int InitialMonth = 6;

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")  // Начальный день
	int InitialDay = 10;

	//UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
	//int InitialHour = 12;  // Закомментированное свойство для начального часа 

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")  // Часовой пояс
	int TimeZone;

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")  // Широта
	float Lat;

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")  // Долгота
	float Lng;

	UPROPERTY(Replicated)  // Свойство с репликацией (синхронизация между клиентом и сервером)
	double RealTimeOffset = 0;  // Смещение реального времени 

	float ClcGameTime(float RealServerTime);  // Метод для вычисления игрового времени на основе реального серверного времени

	TSandboxGameTime ClcLocalGameTime(float RealServerTime);  // Метод для вычисления локального игрового времени на основе реального серверного времени

	TSandboxGameTime ClcGameTimeOfDay(float RealServerTime, bool bAccordingTimeZone);  // Метод для вычисления игрового времени суток с учетом часового пояса

	TSandboxGameTime ClcGameTimeOfDay();  // Метод для вычисления игрового времени суток без параметров 

	void SetTimeOffset(float time);  // Метод для установки смещения времени 

	double GetNewTimeOffset();  // Метод для получения нового смещения времени 

	virtual void UpdatePlayerPosition(FVector Pos, APlayerController* Controller);  // Виртуальный метод обновления позиции игрока 

	void SetCaveMode(bool bCaveModeEnabled);  // Метод для установки режима пещеры 

	bool IsCaveMode();  // Метод проверки активен ли режим пещеры 

	bool IsNight() const;  // Метод проверки является ли сейчас ночь 

	UFUNCTION(BlueprintCallable, Category = "Sandbox")  // Метод доступный из Blueprints 
	FString GetCurrentTimeAsString();  // Метод получения текущего времени как строки
	
protected:

	virtual float ClcHeightFactor() const;  // Виртуальный метод вычисления фактора высоты 

private:

	bool bIsNight = false;  // Флаг является ли сейчас ночь 

	bool bCaveMode = false;  // Флаг активен ли режим пещеры 

	float LastTime;  // Последнее время 

	float InitialFogOpacity;  // Начальная непрозрачность тумана 

	float InitialFogDensity;  // Начальная плотность тумана 

	void PerformDayNightCycle();  // Метод выполнения цикла день-ночь 

	//FLinearColor FogColor;    // Закомментированное свойство цвета тумана 

	FVector PlayerPos;  // Позиция игрока 

	float InitialSkyIntensity;  // Начальная интенсивность неба 

	float InitialSunIntensity;  // Начальная интенсивность солнца 
	
};
