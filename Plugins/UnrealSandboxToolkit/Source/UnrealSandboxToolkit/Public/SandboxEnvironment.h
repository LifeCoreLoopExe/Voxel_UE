// Fill out your copyright notice in the Description page of Project Settings.

#pragma once // Включает файл только один раз

#include "Engine.h" // Включает основные функции движка
#include "GameFramework/Actor.h" // Включает класс Actor
#include "SandboxEnvironment.generated.h" // Включает сгенерированный заголовочный файл для SandboxEnvironment

struct TSandboxGameTime { // Структура для хранения игрового времени
    int days; // Дни
    int hours; // Часы
    int minutes; // Минуты
    int seconds; // Секунды
    int month; // Месяц
    int year; // Год
};

UCLASS() // Объявление класса, доступного для рефлексии UObject
class UNREALSANDBOXTOOLKIT_API ASandboxEnvironment : public AActor { // Объявление класса ASandboxEnvironment, наследующегося от AActor
    GENERATED_BODY() // Макрос для генерации тела класса

public:
    ASandboxEnvironment(); // Конструктор

    virtual void BeginPlay() override; // Метод, вызываемый при начале игры

    virtual void Tick(float DeltaSeconds) override; // Метод, вызываемый каждый тик

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе
    ADirectionalLight* DirectionalLightSource; // Источник направленного света

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе
    ASkyLight* SkyLight; // Свет неба

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе
    AExponentialHeightFog* GlobalFog; // Глобальный туман

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе
    AStaticMeshActor* CaveSphere; // Сфера пещеры

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе
    AAmbientSound* AmbientSound; // Фоновый звук

    // UPROPERTY(EditAnywhere, Category = "Sandbox")
    // UCurveFloat* DayNightCycleSkyLightCurve;

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе
    UCurveFloat* HeightCurve; // Кривая высоты

    // UPROPERTY(EditAnywhere, Category = "Sandbox Cave")
    // float CaveSkyLightRatio;

    UPROPERTY(EditAnywhere, Category = "Sandbox Cave") // Свойство, доступное для редактирования в редакторе
    float CaveSkyLightIntensity; // Интенсивность света в пещере

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе
    UCurveFloat* CaveSunLightCurve; // Кривая солнечного света в пещере

    UPROPERTY(EditAnywhere, Category = "Sandbox") // Свойство, доступное для редактирования в редакторе
    UCurveFloat* GlobalFogDensityCurve; // Кривая плотности глобального тумана

    // UPROPERTY(EditAnywhere, Category = "Sandbox")
    // UCurveFloat* GlobalFogOpacityCurve;

    UPROPERTY(EditAnywhere, Replicated, Category = "Sandbox") // Свойство, доступное для редактирования и репликации
    float TimeSpeed; // Скорость времени

    UPROPERTY(EditAnywhere, Category = "Sandbox Cave") // Свойство, доступное для редактирования в редакторе
    float CaveFogDensity; // Плотность тумана в пещере

    // UPROPERTY(EditAnywhere, Category = "Sandbox Cave")
    // float CaveFogOpacity;

    // UPROPERTY(EditAnywhere, Category = "Sandbox Cave")
    // FLinearColor CaveFogInscatteringColor;

    UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle") // Свойство, доступное для редактирования в редакторе
    bool bEnableDayNightCycle; // Флаг включения цикла день/ночь

    UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle") // Свойство, доступное для редактирования в редакторе
    int InitialYear = 2016; // Начальный год

    UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle") // Свойство, доступное для редактирования в редакторе
    int InitialMonth = 6; // Начальный месяц

    UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle") // Свойство, доступное для редактирования в редакторе
    int InitialDay = 10; // Начальный день

    // UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
    // int InitialHour = 12;

    UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle") // Свойство, доступное для редактирования в редакторе
    int TimeZone; // Часовой пояс

    UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle") // Свойство, доступное для редактирования в редакторе
    float Lat; // Широта

    UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle") // Свойство, доступное для редактирования в редакторе
    float Lng; // Долгота

    UPROPERTY(Replicated) // Свойство, доступное для репликации
    double RealTimeOffset = 0; // Смещение реального времени

    float ClcGameTime(float RealServerTime); // Метод для расчета игрового времени

    TSandboxGameTime ClcLocalGameTime(float RealServerTime); // Метод для расчета локального игрового времени

    TSandboxGameTime ClcGameTimeOfDay(float RealServerTime, bool bAccordingTimeZone); // Метод для расчета времени суток

    TSandboxGameTime ClcGameTimeOfDay(); // Метод для расчета времени суток

    void SetTimeOffset(float time); // Метод для установки смещения времени

    double GetNewTimeOffset(); // Метод для получения нового смеще
