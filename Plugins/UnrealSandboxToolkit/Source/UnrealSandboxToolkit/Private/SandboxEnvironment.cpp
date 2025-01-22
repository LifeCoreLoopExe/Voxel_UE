// Этот файл отвечает за всю систему окружения в игре - смену дня и ночи, освещение, туман

#include "SandboxEnvironment.h"
#include <ctime>
#include "SunPos.h"
#include "Net/UnrealNetwork.h"

// Главный класс, который управляет всем окружением в игре
ASandboxEnvironment::ASandboxEnvironment() {
	bReplicates = true; // Включаем поддержку мультиплеера
	PrimaryActorTick.bCanEverTick = true; // Разрешаем обновление каждый кадр
	TimeSpeed = 10.f; // Время идет в 10 раз быстрее реального
	bEnableDayNightCycle = true; // Включаем смену дня и ночи
	Lng = 27.55; // Долгота на карте мира
	Lat = 53.91; // Широта на карте мира
	TimeZone = +3; // Часовой пояс (например, Москва = +3)
	PlayerPos = FVector::ZeroVector; // Начальная позиция игрока - в центре координат
	InitialSkyIntensity = 0.1; // Начальная яркость неба
	CaveSkyLightIntensity = 1; // Яркость освещения в пещерах
	CaveFogDensity = 0.5; // Густота тумана в пещерах
}

// Проверяет насколько ярко светит небо
float GetSkyLightIntensity(ASkyLight* SkyLight) {
	if (SkyLight) {
		USkyLightComponent* SkyLightComponent = SkyLight->GetLightComponent();
		if (SkyLightComponent) {
			return SkyLightComponent->Intensity;
		}
	}

	return -1;
}

// Проверяет насколько ярко светит солнце
float GetSunLightIntensity(ADirectionalLight* Light) {
	if (Light) {
		ULightComponent* LightComponent = Light->GetLightComponent();
		if (LightComponent) {
			return LightComponent->Intensity;
		}
	}

	return 10.f;
}

// Эта функция вызывается при старте игры
void ASandboxEnvironment::BeginPlay() {
	Super::BeginPlay();

	// Настраиваем начальное положение солнца
	if (DirectionalLightSource){
		DirectionalLightSource->SetActorRotation(FRotator(-90.0f, 0.0f, 0.0f));
		InitialSunIntensity = GetSunLightIntensity(DirectionalLightSource);
	}

	// Показываем или прячем пещеру
	if (CaveSphere) {
		CaveSphere->GetStaticMeshComponent()->SetVisibility(bCaveMode);
	}

	// Запоминаем начальные настройки тумана
	if (GlobalFog) {
		UExponentialHeightFogComponent* FogCmoponent = GlobalFog->GetComponent();
		InitialFogDensity = FogCmoponent->FogDensity;
	}

	// Запоминаем начальную яркость неба
	if (SkyLight) {
		InitialSkyIntensity = GetSkyLightIntensity(SkyLight);
	}
}

// Функция вызывается каждый кадр
void ASandboxEnvironment::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );

	// Если включена смена дня и ночи - обновляем время
	if(bEnableDayNightCycle) {
		PerformDayNightCycle();
	}
}

// Меняет яркость неба
void SetSkyLightIntensity(ASkyLight* SkyLight, float Intensity) {
	if (SkyLight) {
		USkyLightComponent* SkyLightComponent = SkyLight->GetLightComponent();
		if (SkyLightComponent) {
			SkyLightComponent->Intensity = Intensity;
			SkyLightComponent->MarkRenderStateDirty();
		}
	}
}

// Просто возвращает 1 (пока что не используется)
float ASandboxEnvironment::ClcHeightFactor() const {
	return 1.f;
}

// Самая важная функция - тут происходит вся магия смены дня и ночи
void ASandboxEnvironment::PerformDayNightCycle() {
	UWorld* World = GetWorld();
	AGameStateBase* GameState = World->GetGameState();

	if (!GameState) {
		return;
	}

	// Получаем текущее время на сервере
	float RealServerTime = GameState->GetServerWorldTimeSeconds();
	// Переводим реальное время в игровое
	TSandboxGameTime GameDayTime = ClcGameTimeOfDay(RealServerTime, false); // используем UTC время

	// Заполняем структуру времени для расчета положения солнца
	cTime Time;
	Time.iYear = GameDayTime.year;      // Год
	Time.iMonth = GameDayTime.month;    // Месяц
	Time.iDay = GameDayTime.days;       // День
	Time.dHours = GameDayTime.hours;    // Часы
	Time.dMinutes = GameDayTime.minutes; // Минуты
	Time.dSeconds = GameDayTime.seconds; // Секунды

	// Задаем географическое положение
	cLocation GeoLoc;
	GeoLoc.dLongitude = Lng; // Долгота
	GeoLoc.dLatitude = Lat;  // Широта

	cSunCoordinates SunPosition;
	// Вычисляем положение солнца
	sunpos(Time, GeoLoc, &SunPosition);

	// Если есть источник солнечного света
	if (DirectionalLightSource) {
		// Поворачиваем солнце как надо
		DirectionalLightSource->SetActorRotation(FRotator(-(90 - SunPosition.dZenithAngle), SunPosition.dAzimuth, 0.0f));

		// Определяем день сейчас или ночь
		float H = 1 - SunPosition.dZenithAngle / 180;
		bIsNight = H < 0.5; // Если солнце низко - значит ночь

		float HeightFactor = ClcHeightFactor();

		// Настраиваем яркость солнца в пещерах
		if (CaveSunLightCurve) {
			float SunIntensity = CaveSunLightCurve->GetFloatValue(HeightFactor);
			DirectionalLightSource->GetLightComponent()->SetIntensity(InitialSunIntensity * SunIntensity);
		}

		// Настраиваем освещение неба
		if (SkyLight) {
			float DayNightIntensity = InitialSkyIntensity;
			const float Intensity = (DayNightIntensity * HeightFactor) + (CaveSkyLightIntensity * (1 - HeightFactor));
		}

		// Настраиваем туман
		if (GlobalFog) {
			UExponentialHeightFogComponent* FogCmoponent = GlobalFog->GetComponent();

			// Меняем плотность тумана в зависимости от времени суток
			if (GlobalFogDensityCurve) {
				const float DayNightFogDensity = InitialFogDensity * GlobalFogDensityCurve->GetFloatValue(H);
				const float FogDensity = (DayNightFogDensity * HeightFactor) + (CaveFogDensity * (1 - HeightFactor));
				FogCmoponent->SetFogDensity(FogDensity);
			}
		}
	}
}

// Переводит реальное время в игровое
float ASandboxEnvironment::ClcGameTime(float RealServerTime) {
	return (RealServerTime + RealTimeOffset) * TimeSpeed;
}

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

TSandboxGameTime ASandboxEnvironment::ClcGameTimeOfDay(float RealServerTime, bool bAccordingTimeZone) {
	std::tm initial_ptm {};
	initial_ptm.tm_hour = 12;
	initial_ptm.tm_min = 0;
	initial_ptm.tm_sec = 0;
	initial_ptm.tm_mon = InitialMonth + 1;
	initial_ptm.tm_mday = InitialDay;
	initial_ptm.tm_year = InitialYear - 1900;

	time_t initial_time = std::mktime(&initial_ptm);

	//static const uint64 InitialOffset = 60 * 60 * 12; // always start game at 12:00
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

TSandboxGameTime ASandboxEnvironment::ClcGameTimeOfDay() {
	UWorld* World = GetWorld();
	AGameStateBase* GameState = World->GetGameState();

	if (!GameState) {
		return TSandboxGameTime();
	}

	return  ClcGameTimeOfDay(GameState->GetServerWorldTimeSeconds(), true);
}

void ASandboxEnvironment::SetTimeOffset(float Offset) {
	RealTimeOffset = Offset;
}

double ASandboxEnvironment::GetNewTimeOffset() {
	AGameStateBase* GameState = GetWorld()->GetGameState();

	if (!GameState) {
		return RealTimeOffset;
	}

	float RealServerTime = GameState->GetServerWorldTimeSeconds();
	return RealTimeOffset + RealServerTime;
}

void ASandboxEnvironment::UpdatePlayerPosition(FVector Pos, APlayerController* Controller) {
	PlayerPos = Pos;

	if (CaveSphere) {
		CaveSphere->SetActorLocation(Pos);
	}

	if (GlobalFog) {
		GlobalFog->SetActorLocation(Pos);
	}
}

bool ASandboxEnvironment::IsCaveMode() {
	return bCaveMode;
}

void ASandboxEnvironment::SetCaveMode(bool bCaveModeEnabled) {
	if (bCaveMode == bCaveModeEnabled) {
		return;
	}

	if (CaveSphere) {
		CaveSphere->GetStaticMeshComponent()->SetVisibility(bCaveModeEnabled);
	}

	bCaveMode = bCaveModeEnabled;
}

bool ASandboxEnvironment::IsNight() const {
	return bIsNight;
}

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

void ASandboxEnvironment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASandboxEnvironment, RealTimeOffset);
	DOREPLIFETIME(ASandboxEnvironment, TimeSpeed);
}
