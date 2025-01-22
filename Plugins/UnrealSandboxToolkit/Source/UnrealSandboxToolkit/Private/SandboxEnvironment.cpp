// Заполните свое уведомление об авторских правах на странице "Описание" в настройках проекта.

#include "SandboxEnvironment.h"
#include <ctime>
#include "SunPos.h"
#include "Net/UnrealNetwork.h"

// Конструктор для ASandboxEnvironment
ASandboxEnvironment::ASandboxEnvironment() {
    bReplicates = true; // Включение репликации для этого актора
    PrimaryActorTick.bCanEverTick = true; // Разрешение тиков для этого актора
    TimeSpeed = 10.f; // Скорость времени
    bEnableDayNightCycle = true; // Включение цикла дня и ночи
    Lng = 27.55; // Долгота
    Lat = 53.91; // Широта
    TimeZone = +3; // Часовой пояс
    PlayerPos = FVector::ZeroVector; // Позиция игрока
    InitialSkyIntensity = 0.1; // Начальная интенсивность света неба
    CaveSkyLightIntensity = 1; // Интенсивность света неба в пещере
    CaveFogDensity = 0.5; // Плотность тумана в пещере
}

// Получение интенсивности света неба
float GetSkyLightIntensity(ASkyLight* SkyLight) {
    if (SkyLight) {
        USkyLightComponent* SkyLightComponent = SkyLight->GetLightComponent();
        if (SkyLightComponent) {
            return SkyLightComponent->Intensity;
        }
    }

    return -1;
}

// Получение интенсивности света солнца
float GetSunLightIntensity(ADirectionalLight* Light) {
    if (Light) {
        ULightComponent* LightComponent = Light->GetLightComponent();
        if (LightComponent) {
            return LightComponent->Intensity;
        }
    }

    return 10.f;
}

// Вызывается, когда игра начинается или когда объект создается
void ASandboxEnvironment::BeginPlay() {
    Super::BeginPlay(); // Вызов функции BeginPlay родительского класса

    if (DirectionalLightSource) {
        DirectionalLightSource->SetActorRotation(FRotator(-90.0f, 0.0f, 0.0f)); // Установка поворота источника света
        InitialSunIntensity = GetSunLightIntensity(DirectionalLightSource); // Получение начальной интенсивности света солнца
    }

    if (CaveSphere) {
        CaveSphere->GetStaticMeshComponent()->SetVisibility(bCaveMode); // Установка видимости сферы пещеры
    }

    if (GlobalFog) {
        UExponentialHeightFogComponent* FogCmoponent = GlobalFog->GetComponent(); // Получение компонента тумана
        InitialFogDensity = FogCmoponent->FogDensity; // Получение начальной плотности тумана
    }

    if (SkyLight) {
        InitialSkyIntensity = GetSkyLightIntensity(SkyLight); // Получение начальной интенсивности света неба
    }
}

// Вызывается каждый кадр
void ASandboxEnvironment::Tick(float DeltaTime) {
    Super::Tick(DeltaTime); // Вызов функции Tick родительского класса

    if (bEnableDayNightCycle) {
        PerformDayNightCycle(); // Выполнение цикла дня и ночи
    }
}

// Установка интенсивности света неба
void SetSkyLightIntensity(ASkyLight* SkyLight, float Intensity) {
    if (SkyLight) {
        USkyLightComponent* SkyLightComponent = SkyLight->GetLightComponent();
        if (SkyLightComponent) {
            SkyLightComponent->Intensity = Intensity;
            //SkyLightComponent->RecaptureSky(); // только для ue4
            SkyLightComponent->MarkRenderStateDirty(); // Отметка состояния рендеринга как грязного
        }
    }
}

// Вычисление фактора высоты
float ASandboxEnvironment::ClcHeightFactor() const {
    return 1.f;
}

// Выполнение цикла дня и ночи
void ASandboxEnvironment::PerformDayNightCycle() {
    UWorld* World = GetWorld();
    AGameStateBase* GameState = World->GetGameState();

    if (!GameState) {
        return;
    }

    float RealServerTime = GameState->GetServerWorldTimeSeconds(); // Получение времени сервера
    TSandboxGameTime GameDayTime = ClcGameTimeOfDay(RealServerTime, false); // Вычисление игрового времени дня (использовать время UTC)

    //UE_LOG(LogTemp, Log, TEXT("%f"), RealServerTime);
    //UE_LOG(LogTemp, Log, TEXT("%d : %d"), GameTimeOfDay.hours, GameTimeOfDay.minutes);

    //FString ttt = GetCurrentTimeAsString();
    //UE_LOG(LogTemp, Log, TEXT("%s"), *ttt);

    cTime Time;
    Time.iYear = GameDayTime.year;
    Time.iMonth = GameDayTime.month;
    Time.iDay = GameDayTime.days;

    Time.dHours = GameDayTime.hours;
    Time.dMinutes = GameDayTime.minutes;
    Time.dSeconds = GameDayTime.seconds;

    cLocation GeoLoc;
    GeoLoc.dLongitude = Lng;
    GeoLoc.dLatitude = Lat;

    cSunCoordinates SunPosition;

    sunpos(Time, GeoLoc, &SunPosition); // Вычисление позиции солнца

    if (DirectionalLightSource) {
        DirectionalLightSource->SetActorRotation(FRotator(-(90 - SunPosition.dZenithAngle), SunPosition.dAzimuth, 0.0f)); // Установка поворота источника света

        if (bCaveMode) {
            //DirectionalLightSource->GetLightComponent()->SetIntensity(0.1);
        } else {
            //DirectionalLightSource->GetLightComponent()->SetIntensity(4);
            //DirectionalLightSource->SetEnabled(true);
        }

        float H = 1 - SunPosition.dZenithAngle / 180;
        bIsNight = H < 0.5; // Определение, является ли ночь

        float HeightFactor = ClcHeightFactor(); // Вычисление фактора высоты

        if (CaveSunLightCurve) {
            float SunIntensity = CaveSunLightCurve->GetFloatValue(HeightFactor);
            DirectionalLightSource->GetLightComponent()->SetIntensity(InitialSunIntensity * SunIntensity); // Установка интенсивности света солнца
        }

        if (SkyLight) {
            float DayNightIntensity = InitialSkyIntensity;

            //const float CaveSkyLightIntensity = InitialSkyIntensity * CaveSkyLightRatio;
            const float Intensity = (DayNightIntensity * HeightFactor) + (CaveSkyLightIntensity * (1 - HeightFactor));
            //UE_LOG(LogTemp, Log, TEXT("H = %f, DayNightIntensity = %f, HeightFactor = %f ---> %f"), H, DayNightIntensity, HeightFactor, Intensity);
            //UE_LOG(LogTemp, Log, TEXT("Intensity -> %f"), Intensity);

            //SetSkyLightIntensity(SkyLight, Intensity);

            if (bCaveMode) {
                //SetSkyLightIntensity(SkyLight, 6);
            }
        }

        if (GlobalFog) {
            UExponentialHeightFogComponent* FogCmoponent = GlobalFog->GetComponent();
            //FogCmoponent->SetFogInscatteringColor(FogColor);

            if (GlobalFogDensityCurve) {
                const float DayNightFogDensity = InitialFogDensity * GlobalFogDensityCurve->GetFloatValue(H);
                const float FogDensity = (DayNightFogDensity * HeightFactor) + (CaveFogDensity * (1 - HeightFactor));
                //UE_LOG(LogTemp, Log, TEXT("H = %f, GlobalFogDensityCurve = %f, HeightFactor = %f ---> %f"), H, GlobalFogDensityCurve->GetFloatValue(H), HeightFactor, FogDensity);
                FogCmoponent->SetFogDensity(FogDensity); // Установка плотности тумана
            }
        }
    }
}

// Вычисление игрового времени
float ASandboxEnvironment::ClcGameTime(float RealServerTime) {
    return (RealServerTime + RealTimeOffset) * TimeSpeed;
}

// Вычисление локального игрового времени
TSandboxGameTime ASandboxEnvironment::ClcLocalGameTime(float RealServerTime) {
    long input_seconds = (long)(ClcGameTime(RealServerTime));

    const int cseconds_in_day = 86400;
    const int cseconds_in_hour = 3600;
    const int cseconds_in_minute = 60;
    const int cseconds = 1;

    TSandboxGameTime ret;
    ret.days = input_seconds / cseconds_in_day;
    ret.hours = (input_seconds % cseconds_in_day) / cseconds_in_hour;
    ret.minutes = ((input_seconds % cseconds_in_day) % cseconds_in_hour) / cseconds_in_minute;
    ret.seconds = (((input_seconds % cseconds_in_day) % cseconds_in_hour) % cseconds_in_minute) / cseconds;

    return ret;
}

// Вычисление игрового времени дня
TSandboxGameTime ASandboxEnvironment::ClcGameTimeOfDay(float RealServerTime, bool bAccordingTimeZone) {
    std::tm initial_ptm{};
    initial_ptm.tm_hour = 12;
    initial_ptm.tm_min = 0;
    initial_ptm.tm_sec = 0;
    initial_ptm.tm_mon = InitialMonth + 1;
    initial_ptm.tm_mday = InitialDay;
    initial_ptm.tm_year = InitialYear - 1900;

    time_t initial_time = std::mktime(&initial_ptm);

    //static const uint64 InitialOffset = 60 * 60 * 12; // всегда начинать игру в 12:00
    const uint64 InitialOffset = initial_time;
    const uint64 TimezoneOffset = bAccordingTimeZone ? 60 * 60 * TimeZone : 0;
    const uint64 input_seconds = (int)ClcGameTime(RealServerTime) + InitialOffset + TimezoneOffset;

    time_t rawtime = (time_t)input_seconds;
    tm ptm;

#ifdef _MSC_VER
    gmtime_s(&ptm, &rawtime);
#else
    ptm = *gmtime_r(&rawtime, &ptm);
#endif

    TSandboxGameTime Time;
    Time.hours = ptm.tm_hour;
    Time.minutes = ptm.tm_min;
    Time.seconds = ptm.tm_sec;
    Time.days = ptm.tm_mday;
    Time.month = ptm.tm_mon + 1;
    Time.year = ptm.tm_year + 1900;

    return Time;
}

// Вычисление игрового времени дня
TSandboxGameTime ASandboxEnvironment::ClcGameTimeOfDay() {
    UWorld* World = GetWorld();
    AGameStateBase* GameState = World->GetGameState();

    if (!GameState) {
        return TSandboxGameTime();
    }

    return ClcGameTimeOfDay(GameState->GetServerWorldTimeSeconds(), true);
}

// Установка смещения времени
void ASandboxEnvironment::SetTimeOffset(float Offset) {
    RealTimeOffset = Offset;
}

// Получение нового смещения времени
double ASandboxEnvironment::GetNewTimeOffset() {
    AGameStateBase* GameState = GetWorld()->GetGameState();

    if (!GameState) {
        return RealTimeOffset;
    }

    float RealServerTime = GameState->GetServerWorldTimeSeconds();
    return RealTimeOffset + RealServerTime;
}

// Обновление позиции игрока
void ASandboxEnvironment::UpdatePlayerPosition(FVector Pos, APlayerController* Controller) {
    PlayerPos = Pos;

    if (CaveSphere) {
        CaveSphere->SetActorLocation(Pos); // Установка местоположения сферы пещеры
    }

    if (GlobalFog) {
        GlobalFog->SetActorLocation(Pos); // Установка местоположения тумана
    }
}

// Проверка, включен ли режим пещеры
bool ASandboxEnvironment::IsCaveMode() {
    return bCaveMode;
}

// Установка режима пещеры
void ASandboxEnvironment::SetCaveMode(bool bCaveModeEnabled) {
    if (bCaveMode == bCaveModeEnabled) {
        return;
    }

    if (CaveSphere) {
        CaveSphere->GetStaticMeshComponent()->SetVisibility(bCaveModeEnabled); // Установка видимости сферы пещеры
    }

    bCaveMode = bCaveModeEnabled;
}

// Проверка, является ли ночь
bool ASandboxEnvironment::IsNight() const {
    return bIsNight;
}

// Получение текущего времени в виде строки
FString ASandboxEnvironment::GetCurrentTimeAsString() {
    UWorld* World = GetWorld();
    AGameStateBase* GameState = World->GetGameState();

    if (!GameState) {
        return TEXT("");
    }

    float RealServerTime = GameState->GetServerWorldTimeSeconds();
    TSandboxGameTime CurrentTime = ClcGameTimeOfDay(RealServerTime, true);

    FString Str = FString::Printf(TEXT("%02d:%02d"), CurrentTime.hours, CurrentTime.minutes);
    return Str;
}

// Репликация свойств для сетевой синхронизации
void ASandboxEnvironment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps); // Вызов функции родительского класса
    DOREPLIFETIME(ASandboxEnvironment, RealTimeOffset); // Репликация смещения времени
    DOREPLIFETIME(ASandboxEnvironment, TimeSpeed); // Репликация скорости времени
}
