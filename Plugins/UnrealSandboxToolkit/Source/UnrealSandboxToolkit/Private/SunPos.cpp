#include "SunPos.h"
#include <math.h>

void sunpos(cTime udtTime, cLocation udtLocation, cSunCoordinates *udtSunCoordinates)
{
	// Главные переменные
	double dElapsedJulianDays; // Прошедшие юлианские дни
	double dDecimalHours; // Десятичные часы
	double dEclipticLongitude; // Эклиптическая долгота
	double dEclipticObliquity; // Эклиптическая обликва
	double dRightAscension; // Прямое восхождение
	double dDeclination; // Склонение

	// Вспомогательные переменные
	double dY; // Координата Y
	double dX; // Координата X

	// Вычисление разницы в днях между текущим юлианским днем
	// и JD 2451545.0, который соответствует полудню 1 января 2000 года по всемирному времени
	{
		double dJulianDate; // Юлианская дата
		long int liAux1; // Вспомогательная переменная 1
		long int liAux2; // Вспомогательная переменная 2
		// Вычисление времени суток в десятичных часах по всемирному времени
		dDecimalHours = udtTime.dHours + (udtTime.dMinutes
			+ udtTime.dSeconds / 60.0) / 60.0;
		// Вычисление текущего юлианского дня
		liAux1 = (udtTime.iMonth - 14) / 12;
		liAux2 = (1461 * (udtTime.iYear + 4800 + liAux1)) / 4 + (367 * (udtTime.iMonth
			- 2 - 12 * liAux1)) / 12 - (3 * ((udtTime.iYear + 4900
				+ liAux1) / 100)) / 4 + udtTime.iDay - 32075;
		dJulianDate = (double)(liAux2)-0.5 + dDecimalHours / 24.0;
		// Вычисление разницы между текущим юлианским днем и JD 2451545.0
		dElapsedJulianDays = dJulianDate - 2451545.0;
	}

	// Вычисление эклиптических координат (эклиптическая долгота и обликва
	// эклиптики в радианах, без ограничения угла меньше 2*Pi
	// (т.е. результат может быть больше 2*Pi)
	{
		double dMeanLongitude; // Средняя долгота
		double dMeanAnomaly; // Средний аномалия
		double dOmega; // Омега
		dOmega = 2.1429 - 0.0010394594*dElapsedJulianDays;
		dMeanLongitude = 4.8950630 + 0.017202791698*dElapsedJulianDays; // Радианы
		dMeanAnomaly = 6.2400600 + 0.0172019699*dElapsedJulianDays;
		dEclipticLongitude = dMeanLongitude + 0.03341607*sin(dMeanAnomaly)
			+ 0.00034894*sin(2 * dMeanAnomaly) - 0.0001134
			- 0.0000203*sin(dOmega);
		dEclipticObliquity = 0.4090928 - 6.2140e-9*dElapsedJulianDays
			+ 0.0000396*cos(dOmega);
	}

	// Вычисление небесных координат (прямое восхождение и склонение) в радианах
	// без ограничения угла меньше 2*Pi (т.е. результат может быть
	// больше 2*Pi)
	{
		double dSin_EclipticLongitude; // Синус эклиптической долготы
		dSin_EclipticLongitude = sin(dEclipticLongitude);
		dY = cos(dEclipticObliquity) * dSin_EclipticLongitude; // Вычисление Y
		dX = cos(dEclipticLongitude); // Вычисление X
		dRightAscension = atan2(dY, dX); // Прямое восхождение
		if (dRightAscension < 0.0) dRightAscension = dRightAscension + twopi; // Коррекция
		dDeclination = asin(sin(dEclipticObliquity)*dSin_EclipticLongitude); // Склонение
	}

	// Вычисление местных координат (азимут и угол зенита) в градусах
	{
		double dGreenwichMeanSiderealTime; // Среднее звездное время на Гринуиче
		double dLocalMeanSiderealTime; // Местное среднее звездное время
		double dLatitudeInRadians; // Широта в радианах
		double dHourAngle; // Угол часа
		double dCos_Latitude; // Косинус широты
		double dSin_Latitude; // Синус широты
		double dCos_HourAngle; // Косинус угла часа
		double dParallax; // Параллакс
		dGreenwichMeanSiderealTime = 6.6974243242 +
			0.0657098283*dElapsedJulianDays
			+ dDecimalHours; // Вычисление среднего звездного времени
		dLocalMeanSiderealTime = (dGreenwichMeanSiderealTime * 15
			+ udtLocation.dLongitude)*rad; // Местное звездное время
		dHourAngle = dLocalMeanSiderealTime - dRightAscension; // Угол часа
		dLatitudeInRadians = udtLocation.dLatitude*rad; // Широта в радианах
		dCos_Latitude = cos(dLatitudeInRadians); // Косинус широты
		dSin_Latitude = sin(dLatitudeInRadians); // Синус широты
		dCos_HourAngle = cos(dHourAngle); // Косинус угла часа
		udtSunCoordinates->dZenithAngle = (acos(dCos_Latitude*dCos_HourAngle
			*cos(dDeclination) + sin(dDeclination)*dSin_Latitude)); // Угол зенита
		dY = -sin(dHourAngle); // Y координата для азимута
		dX = tan(dDeclination)*dCos_Latitude - dSin_Latitude*dCos_HourAngle; // X координата для азимута
		udtSunCoordinates->dAzimuth = atan2(dY, dX); // Азимут
		if (udtSunCoordinates->dAzimuth < 0.0)
			udtSunCoordinates->dAzimuth = udtSunCoordinates->dAzimuth + twopi; // Коррекция азимута
		udtSunCoordinates->dAzimuth = udtSunCoordinates->dAzimuth / rad; // Перевод в градусы
		// Коррекция параллакса
		dParallax = (dEarthMeanRadius / dAstronomicalUnit)
			*sin(udtSunCoordinates->dZenithAngle); 
		udtSunCoordinates->dZenithAngle = (udtSunCoordinates->dZenithAngle
			+ dParallax) / rad; // Угол зенита с учетом параллакса
	}
}
