// Заполните уведомление об авторских правах на странице "Описание" в настройках проекта.

#include "SandboxEnvironment.h"
#include <ctime>
#include "SunPos.h"
#include "Net/UnrealNetwork.h"

// Конструктор класса ASandboxEnvironment
ASandboxEnvironment::ASandboxEnvironment() {
    bReplicates = true; // Включает репликацию для этого актора
    PrimaryActorTick.bCanEverTick = true; // Разрешает вызов функции Tick
    TimeSpeed = 10.f; // Скорость времени
    bEnableDayNightCycle = true; // Включает цикл дня и ночи
    Lng = 27.55; // Долгота
    Lat = 53.91; // Широта
    TimeZone = +3; // Часовой пояс
    PlayerPos = FVector::ZeroVector; // Позиция игрока
    InitialSkyIntensity = 0.1; // Начальная интенсивность света неба
    CaveSkyLightIntensity = 1; // Интенсивность света неба в пещере
    CaveFogDensity = 0.5; // Плотность тумана в пещере
}

// Функция для получения интенсивности света неба
float GetSkyLightIntensity(ASkyLight* SkyLight) {
    if (SkyLight) { // Если объект света неба существует
        USkyLightComponent* SkyLightComponent = SkyLight->GetLightComponent(); // Получаем компонент света неба
        if (SkyLightComponent) { // Если компонент существует
            return SkyLightComponent->Intensity; // Возвращаем интенсивность света
        }
    }
    return -1; // Возвращаем -1, если свет неба не найден
}

// Функция для получения интенсивности солнечного света
float GetSunLightIntensity(ADirectionalLight* Light) {
    if (Light) { // Если объект направленного света существует
        ULightComponent* LightComponent = Light->GetLightComponent(); // Получаем компонент света
        if (LightComponent) { // Если компонент существует
            return LightComponent->Intensity; // Возвращаем интенсивность света
        }
    }
    return 10.f; // Возвращаем 10.0, если направленный свет не найден
}

// Функция, вызываемая при начале игры
void ASandboxEnvironment::BeginPlay() {
    Super::BeginPlay(); // Вызываем родительскую функцию BeginPlay

    if (DirectionalLightSource){ // Если источник направленного света существует
        DirectionalLightSource->SetActorRotation(FRotator(-90.0f, 0.0f, 0.0f)); // Устанавливаем вращение источника света
        InitialSunIntensity = GetSunLightIntensity(DirectionalLightSource); // Получаем начальную интенсивность солнечного света
    }

    if (CaveSphere) { // Если сфера пещеры существует
        CaveSphere->GetStaticMeshComponent()->SetVisibility(bCaveMode); // Устанавливаем видимость сферы пещеры
    }

    if (GlobalFog) { // Если глобальный туман существует
        UExponentialHeightFogComponent* FogCmoponent = GlobalFog->GetComponent(); // Получаем компонент тумана
        InitialFogDensity = FogCmoponent->FogDensity; // Получаем начальную плотность тумана
    }

    if (SkyLight) { // Если свет неба существует
        InitialSkyIntensity = GetSkyLightIntensity(SkyLight); // Получаем начальную интенсивность света неба
    }
}

// Функция, вызываемая каждый тик
void ASandboxEnvironment::Tick( float DeltaTime ) {
    Super::Tick( DeltaTime ); // Вызываем родительскую функцию Tick

    if(bEnableDayNightCycle) { // Если цикл дня и ночи включен
        PerformDayNightCycle(); // Выполняем цикл дня и ночи
    }
}

// Функция для установки интенсивности света неба
void SetSkyLightIntensity(ASkyLight* SkyLight, float Intensity) {
    if (SkyLight) { // Если объект света неба существует
        USkyLightComponent* SkyLightComponent = SkyLight->GetLightComponent(); // Получаем компонент света неба
        if (SkyLightComponent) { // Если компонент существует
            SkyLightComponent->Intensity = Intensity; // Устанавливаем интенсивность света
            SkyLightComponent->MarkRenderStateDirty(); // Помечаем состояние рендеринга как измененное
        }
    }
}

// Функция для расчета фактора высоты
float ASandboxEnvironment::ClcHeightFactor() const {
    return 1.f; // Возвращаем 1.0
}

// Функция для выполнения цикла дня и ночи
void ASandboxEnvironment::PerformDayNightCycle() {
    UWorld* World = GetWorld(); // Получаем мир
    AGameStateBase* GameState = World->GetGameState(); // Получаем состояние игры

    if (!GameState) { // Если состояние игры не существует
        return; // Выходим из функции
    }

    float RealServerTime = GameState->GetServerWorldTimeSeconds(); // Получаем реальное время сервера
    TSandboxGameTime GameDayTime = ClcGameTimeOfDay(RealServerTime, false); // Рассчитываем игровое время дня (UTC)

    cTime Time; // Создаем структуру времени
    Time.iYear = GameDayTime.year; // Устанавливаем год
    Time.iMonth = GameDayTime.month; // Устанавливаем месяц
    Time.iDay = GameDayTime.days; // Устанавливаем день
    Time.dHours = GameDayTime.hours; // Устанавливаем часы
    Time.dMinutes = GameDayTime.minutes; // Устанавливаем минуты
    Time.dSeconds = GameDayTime.seconds; // Устанавливаем секунды

    cLocation GeoLoc; // Создаем структуру геолокации
    GeoLoc.dLongitude = Lng; // Устанавливаем долготу
    GeoLoc.dLatitude = Lat; // Устанавливаем широту

    cSunCoordinates SunPosition; // Создаем структуру координат солнца
    sunpos(Time, GeoLoc, &SunPosition); // Рассчитываем позицию солнца

    if (DirectionalLightSource) { // Если источник направленного света существует
        DirectionalLightSource->SetActorRotation(FRotator(-(90 - SunPosition.dZenithAngle), SunPosition.dAzimuth, 0.0f)); // Устанавливаем вращение источника света

        if (bCaveMode) { // Если режим пещеры включен
            // Устанавливаем интенсивность света в пещере
        } else { // Иначе
            // Устанавливаем интенсивность света вне пещеры
        }

        float H = 1 - SunPosition.dZenithAngle / 180; // Рассчитываем высоту солнца
        bIsNight = H < 0.5; // Определяем, ночь ли сейчас

        float HeightFactor = ClcHeightFactor(); // Рассчитываем фактор высоты

        if (CaveSunLightCurve) { // Если кривая интенсивности света в пещере существует
            float SunIntensity = CaveSunLightCurve->GetFloatValue(HeightFactor); // Получаем интенсивность света из кривой
            DirectionalLightSource->GetLightComponent()->SetIntensity(InitialSunIntensity * SunIntensity); // Устанавливаем интенсивность света
        }

        if (SkyLight) { // Если свет неба существует
            float DayNightIntensity = InitialSkyIntensity; // Устанавливаем интенсивность света дня и ночи
            const float Intensity = (DayNightIntensity * HeightFactor) + (CaveSkyLightIntensity * (1 - HeightFactor)); // Рассчитываем интенсивность света
            if (bCaveMode) { // Если режим пещеры включен
                // Устанавливаем интенсивность света в пещере
            }
        }

        if (GlobalFog) { // Если глобальный туман существует
            UExponentialHeightFogComponent* FogCmoponent = GlobalFog->GetComponent(); // Получаем компонент тумана
            if (GlobalFogDensityCurve) { // Если кривая плотности тумана существует
                const float DayNightFogDensity = InitialFogDensity * GlobalFogDensityCurve->GetFloatValue(H); // Рассчитываем плотность тумана дня и ночи
                const float FogDensity = (DayNightFogDensity * HeightFactor) + (CaveFogDensity * (1 - HeightFactor)); // Рассчитываем плотность тумана
                FogCmoponent->SetFogDensity(FogDensity); // Устанавливаем плотность тумана
            }
        }
    }
}

// Функция для расчета игрового времени
float ASandboxEnvironment::ClcGameTime(float RealServerTime) {
    return (RealServerTime + RealTimeOffset) * TimeSpeed; // Рассчитываем игровое время
}

// Функция для расчета локального игрового времени
TSandboxGameTime ASandboxEnvironment::ClcLocalGameTime(float RealServerTime) {
    long input_seconds = (long)(ClcGameTime(RealServerTime)); // Рассчитываем входные секунды

    const int cseconds_in_day = 86400; // Количество секунд в дне
    const int cseconds_in_hour = 3600; // Количество секунд в часе
    const int cseconds_in_minute = 60; // Количество секунд в минуте
    const int cseconds = 1; // Количество секунд

    TSandboxGameTime ret; // Создаем структуру игрового времени
    ret.days = input_seconds / cseconds_in_day; // Рассчитываем дни
    ret.hours = (input_seconds % cseconds_in_day) / cseconds_in_hour; // Рассчитываем часы
    ret.minutes = ((input_seconds % cseconds_in_day) % cseconds_in_hour) / cseconds_in_minute; // Рассчитываем минуты
    ret.seconds = (((input_seconds % cseconds_in_day) % cseconds_in_hour) % cseconds_in_minute) / cseconds; // Рассчитываем секунды

    return ret; // Возвращаем игровое время
}

// Функция для расчета игрового времени дня
TSandboxGameTime ASandboxEnvironment::ClcGameTimeOfDay(float RealServerTime, bool bAccordingTimeZone) {
    std::tm initial_ptm {}; // Создаем структуру времени
    initial_ptm.tm_hour = 12; // Устанавливаем час
    initial_ptm.tm_min = 0; // Устанавливаем минуты
    initial_ptm.tm_sec = 0; // Устанавливаем секунды
    initial_ptm.tm_mon = InitialMonth + 1; // Устанавливаем месяц
    initial_ptm.tm_mday = InitialDay; // Устанавливаем день
    initial_ptm.tm_year = InitialYear - 1900; // Устанавливаем год

    time_t initial_time = std::mktime(&initial_ptm); // Рассчитываем начальное время

    const uint64 InitialOffset = initial_time; // Устанавливаем начальный смещение
    const uint64 TimezoneOffset = bAccordingTimeZone ? 60 * 60 * TimeZone : 0; // Рассчитываем смещение часового пояса
    const uint64 input_seconds = (int)ClcGameTime(RealServerTime) + InitialOffset + TimezoneOffset; // Рассчитываем входные секунды

    time_t rawtime = (time_t)input_seconds; // Устанавливаем сырое время
    tm ptm; // Создаем структуру времени

#ifdef _MSC_VER
    gmtime_s(&ptm, &rawtime); // Рассчитываем время в формате UTC
#else
    ptm = *gmtime_r(&rawtime, &ptm); // Рассчитываем время в формате UTC
#endif

    TSandboxGameTime Time; // Создаем структуру игрового времени
    Time.hours = ptm.tm_hour; // Устанавливаем часы
    Time.minutes = ptm.tm_min; // Устанавливаем минуты
    Time.seconds = ptm.tm_sec; // Устанавливаем секунды
    Time.days = ptm.tm_mday; // Устанавливаем день
    Time.month = ptm.tm_mon + 1; // Устанавливаем месяц
    Time.year = ptm.tm_year + 1900; // Устанавливаем год

    return Time; // Возвращаем игровое время
}

// Функция для расчета игрового времени дня
TSandboxGameTime ASandboxEnvironment::ClcGameTimeOfDay() {
    UWorld* World = GetWorld(); // Получаем мир
    AGameStateBase* GameState = World->GetGameState(); // Получаем состояние игры

    if (!GameState) { // Если состояние игры не существует
        return TSandboxGameTime(); // Возвращаем пустое игровое время
    }

    return  ClcGameTimeOfDay(GameState->GetServerWorldTimeSeconds(), true); // Рассчитываем игровое время дня
}

// Функция для установки смещения времени
void ASandboxEnvironment::SetTimeOffset(float Offset) {
    RealTimeOffset = Offset; // Устанавливаем смещение времени
}

// Функция для получения нового смещения времени
double ASandboxEnvironment::GetNewTimeOffset() {
    AGameStateBase* GameState = GetWorld()->GetGameState(); // Получаем состояние игры

    if (!GameState) { // Если состояние игры не существует
        return RealTimeOffset; // Возвращаем текущее смещение времени
    }

    float RealServerTime = GameState->GetServerWorldTimeSeconds(); // Получаем реальное время сервера
    return RealTimeOffset + RealServerTime; // Возвращаем новое смещение времени
}

// Функция для обновления позиции игрока
void ASandboxEnvironment::UpdatePlayerPosition(FVector Pos, APlayerController* Controller) {
    PlayerPos = Pos; // Устанавливаем позицию игрока

    if (CaveSphere) { // Если сфера пещеры существует
        CaveSphere->SetActorLocation(Pos); // Устанавливаем позицию сферы пещеры
    }

    if (GlobalFog) { // Если глобальный туман существует
        GlobalFog->SetActorLocation(Pos); // Устанавливаем позицию тумана
    }
}

// Функция для проверки, включен ли режим пещеры
bool ASandboxEnvironment::IsCaveMode() {
    return bCaveMode; // Возвращаем состояние режима пещеры
}

// Функция для установки режима пещеры
void ASandboxEnvironment::SetCaveMode(bool bCaveModeEnabled) {
    if (bCaveMode == bCaveModeEnabled) { // Если режим пещеры не изменился
        return; // Выходим из функции
    }

    if (CaveSphere) { // Если сфера пещеры существует
        CaveSphere->GetStaticMeshComponent()->SetVisibility(bCaveModeEnabled); // Устанавливаем видимость сферы пещеры
    }

    bCaveMode = bCaveModeEnabled; // Устанавливаем режим пещеры
}

// Функция для проверки, ночь ли сейчас
bool ASandboxEnvironment::IsNight() const {
    return bIsNight; // Возвращаем состояние ночи
}

// Функция для получения текущего времени в виде строки
FString ASandboxEnvironment::GetCurrentTimeAsString() {
    UWorld* World = GetWorld(); // Получаем мир
    AGameStateBase* GameState = World->GetGameState(); // Получаем состояние игры

    if (!GameState) { // Если состояние игры не существует
        return TEXT(""); // Возвращаем пустую строку
    }

    float RealServerTime = GameState->GetServerWorldTimeSeconds(); // Получаем реальное время сервера
    TSandboxGameTime CurrentTime = ClcGameTimeOfDay(RealServerTime, true); // Рассчитываем текущее игровое время

    FString Str = FString::Printf(TEXT("%02d:%02d"), CurrentTime.hours, CurrentTime.minutes); // Форматируем время в строку
    return Str; // Возвращаем строку времени
}

// Функция для получения реплицируемых свойств
void ASandboxEnvironment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps); // Вызываем родительскую функцию
    DOREPLIFETIME(ASandboxEnvironment, RealTimeOffset); // Реплицируем смещение времени
    DOREPLIFETIME(ASandboxEnvironment, TimeSpeed); // Реплицируем скорость времени
}