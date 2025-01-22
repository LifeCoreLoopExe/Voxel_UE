// This file is available in electronic form at http://www.psa.es/sdg/sunpos.htm
#include "SunPos.h"
#include <math.h>

// Функция для вычисления позиции Солнца
void sunpos(cTime udtTime, cLocation udtLocation, cSunCoordinates *udtSunCoordinates)
{
	// Основные переменные
	double dElapsedJulianDays; // Прошедшие юлианские дни
	double dDecimalHours; // Десятичные часы
	double dEclipticLongitude; // Эклиптическая долгота
	double dEclipticObliquity; // Наклон эклиптики
	double dRightAscension; // Прямое восхождение
	double dDeclination; // Склонение

	// Вспомогательные переменные
	double dY;
	double dX;

	// Вычисление разницы в днях между текущим юлианским днем
	// и JD 2451545.0, который является полднем 1 января 2000 года по универсальному времени
	{
		double dJulianDate; // Юлианская дата
		long int liAux1; // Вспомогательная переменная 1
		long int liAux2; // Вспомогательная переменная 2
		// Вычисление времени суток в десятичных часах UT
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

	// Вычисление эклиптических координат (эклиптическая долгота и наклон эклиптики в радианах,
	// но без ограничения угла меньше 2*Pi (т.е. результат может быть больше 2*Pi))
	{
		double dMeanLongitude; // Средняя долгота
		double dMeanAnomaly; // Средняя аномалия
		double dOmega; // Долгота восходящего узла
		dOmega = 2.1429 - 0.0010394594*dElapsedJulianDays;
		dMeanLongitude = 4.8950630 + 0.017202791698*dElapsedJulianDays; // Радианы
		dMeanAnomaly = 6.2400600 + 0.0172019699*dElapsedJulianDays;
		dEclipticLongitude = dMeanLongitude + 0.03341607*sin(dMeanAnomaly)
			+ 0.00034894*sin(2 * dMeanAnomaly) - 0.0001134
			- 0.0000203*sin(dOmega);
		dEclipticObliquity = 0.4090928 - 6.2140e-9*dElapsedJulianDays
			+ 0.0000396*cos(dOmega);
	}

	// Вычисление небесных координат (прямое восхождение и склонение) в радианах,
	// но без ограничения угла меньше 2*Pi (т.е. результат может быть больше 2*Pi)
	{
		double dSin_EclipticLongitude; // Синус эклиптической долготы
		dSin_EclipticLongitude = sin(dEclipticLongitude);
		dY = cos(dEclipticObliquity) * dSin_EclipticLongitude;
		dX = cos(dEclipticLongitude);
		dRightAscension = atan2(dY, dX);
		if (dRightAscension < 0.0) dRightAscension = dRightAscension + twopi;
		dDeclination = asin(sin(dEclipticObliquity)*dSin_EclipticLongitude);
	}

	// Вычисление местных координат (азимут и зенитный угол) в градусах
	{
		double dGreenwichMeanSiderealTime; // Среднее звездное время Гринвича
		double dLocalMeanSiderealTime; // Местное среднее звездное время
		double dLatitudeInRadians; // Широта в радианах
		double dHourAngle; // Часовой угол
		double dCos_Latitude; // Косинус широты
		double dSin_Latitude; // Синус широты
		double dCos_HourAngle; // Косинус часового угла
		double dParallax; // Параллакс
		dGreenwichMeanSiderealTime = 6.6974243242 +
			0.0657098283*dElapsedJulianDays
			+ dDecimalHours;
		dLocalMeanSiderealTime = (dGreenwichMeanSiderealTime * 15
			+ udtLocation.dLongitude)*rad;
		dHourAngle = dLocalMeanSiderealTime - dRightAscension;
		dLatitudeInRadians = udtLocation.dLatitude*rad;
		dCos_Latitude = cos(dLatitudeInRadians);
		dSin_Latitude = sin(dLatitudeInRadians);
		dCos_HourAngle = cos(dHourAngle);
		udtSunCoordinates->dZenithAngle = (acos(dCos_Latitude*dCos_HourAngle
			*cos(dDeclination) + sin(dDeclination)*dSin_Latitude));
		dY = -sin(dHourAngle);
		dX = tan(dDeclination)*dCos_Latitude - dSin_Latitude*dCos_HourAngle;
		udtSunCoordinates->dAzimuth = atan2(dY, dX);
		if (udtSunCoordinates->dAzimuth < 0.0)
			udtSunCoordinates->dAzimuth = udtSunCoordinates->dAzimuth + twopi;
		udtSunCoordinates->dAzimuth = udtSunCoordinates->dAzimuth / rad;
		// Коррекция параллакса
		dParallax = (dEarthMeanRadius / dAstronomicalUnit)
			*sin(udtSunCoordinates->dZenithAngle);
		udtSunCoordinates->dZenithAngle = (udtSunCoordinates->dZenithAngle
			+ dParallax) / rad;
	}
}
