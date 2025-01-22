// Заполните информацию об авторских правах на странице "Описание" в настройках проекта.

#pragma once

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "SandboxEnvironment.generated.h"

// Структура для хранения игрового времени
struct TSandboxGameTime {
    int days;        // Дни
    int hours;       // Часы
    int minutes;     // Минуты
    int seconds;     // Секунды
    int month;       // Месяц
    int year;        // Год
};

// Класс окружения песочницы, наследуемый от AActor
UCLASS()
class UNREALSANDBOXTOOLKIT_API ASandboxEnvironment : public AActor
{
    GENERATED_BODY()

public:
    ASandboxEnvironment(); // Конструктор

    virtual void BeginPlay() override; // Метод, вызываемый при начале игры

    virtual void Tick(float DeltaSeconds) override; // Метод, вызываемый каждый кадр

    // Источник направленного света
    UPROPERTY(EditAnywhere, Category = "Sandbox")
    ADirectionalLight* DirectionalLightSource;

    // Источник света неба
    UPROPERTY(EditAnywhere, Category = "Sandbox")
    ASkyLight* SkyLight;

    // Глобальный туман
    UPROPERTY(EditAnywhere, Category = "Sandbox")
    AExponentialHeightFog* GlobalFog;

    // Сфера пещеры
    UPROPERTY(EditAnywhere, Category = "Sandbox")
    AStaticMeshActor* CaveSphere;

    // Фоновый звук
    UPROPERTY(EditAnywhere, Category = "Sandbox")
    AAmbientSound* AmbientSound;

    // Кривая изменения высоты
    UPROPERTY(EditAnywhere, Category = "Sandbox")
    UCurveFloat* HeightCurve;

    // Интенсивность света неба в пещере
    UPROPERTY(EditAnywhere, Category = "Sandbox Cave")
    float CaveSkyLightIntensity;

    // Кривая изменения солнечного света в пещере
    UPROPERTY(EditAnywhere, Category = "Sandbox")
    UCurveFloat* CaveSunLightCurve;

    // Кривая изменения плотности глобального тумана
    UPROPERTY(EditAnywhere, Category = "Sandbox")
    UCurveFloat* GlobalFogDensityCurve;

    // Скорость времени
    UPROPERTY(EditAnywhere, Replicated, Category = "Sandbox")
    float TimeSpeed;

    // Плотность тумана в пещере
    UPROPERTY(EditAnywhere, Category = "Sandbox Cave")
    float CaveFogDensity;

    // Включить цикл дня и ночи
    UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
    bool bEnableDayNightCycle;

    // Начальный год
    UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
    int InitialYear = 2016;

    // Начальный месяц
    UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
    int InitialMonth = 6;

    // Начальный день
    UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
    int InitialDay = 10;

    // Часовой пояс
    UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
    int TimeZone;

    // Широта
    UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
    float Lat;

    // Долгота
    UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
    float Lng;

    // Смещение реального времени
    UPROPERTY(Replicated)
    double RealTimeOffset = 0;

    // Рассчитать игровое время
    float ClcGameTime(float RealServerTime);

    // Рассчитать локальное игровое время
    TSandboxGameTime ClcLocalGameTime(float RealServerTime);

    // Рассчитать игровое время дня
    TSandboxGameTime ClcGameTimeOfDay(float RealServerTime, bool bAccordingTimeZone);

    // Рассчитать игровое время дня
    TSandboxGameTime ClcGameTimeOfDay();

    // Установить временное смещение
    void SetTimeOffset(float time);

    // Получить новое временное смещение
    double GetNewTimeOffset();

    // Обновить позицию игрока
    virtual void UpdatePlayerPosition(FVector Pos, APlayerController* Controller);

    // Установить режим пещеры
    void SetCaveMode(bool bCaveModeEnabled);

    // Проверить, включен ли режим пещеры
    bool IsCaveMode();

    // Проверить, ночь ли сейчас
    bool IsNight() const;

    // Получить текущее время в виде строки
    UFUNCTION(BlueprintCallable, Category = "Sandbox")
    FString GetCurrentTimeAsString();

protected:
    // Рассчитать фактор высоты
    virtual float ClcHeightFactor() const;

private:
    // Флаг, указывающий, ночь ли сейчас
    bool bIsNight = false;

    // Флаг, указывающий, включен ли режим пещеры
    bool bCaveMode = false;

    // Последнее время
    float LastTime;

    // Начальная непрозрачность тумана
    float InitialFogOpacity;

    // Начальная плотность тумана
    float InitialFogDensity;

    // Выполнить цикл дня и ночи
    void PerformDayNightCycle();

    // Позиция игрока
    FVector PlayerPos;

    // Начальная интенсивность света неба
    float InitialSkyIntensity;

    // Начальная интенсивность солнечного света
    float InitialSunIntensity;
};
