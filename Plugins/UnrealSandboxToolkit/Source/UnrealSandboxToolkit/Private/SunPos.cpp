// Этот файл доступен в электронном виде по адресу http://www.psa.es/sdg/sunpos.htm
// Код используется для вычисления положения солнца на небе

#include "SunPos.h"
#include <math.h>

// Основная функция для расчета положения солнца
// udtTime - текущее время
// udtLocation - географические координаты места наблюдения
// udtSunCoordinates - сюда будут записаны результаты расчетов (азимут и угол к зениту)
void sunpos(cTime udtTime, cLocation udtLocation, cSunCoordinates *udtSunCoordinates)
{
	// Основные переменные для расчетов
	double dElapsedJulianDays;    // Прошедшие юлианские дни
	double dDecimalHours;         // Десятичные часы
	double dEclipticLongitude;    // Эклиптическая долгота
	double dEclipticObliquity;    // Наклон эклиптики
	double dRightAscension;       // Прямое восхождение
	double dDeclination;          // Склонение

	// Вспомогательные переменные для промежуточных вычислений
	double dY;
	double dX;

	// Вычисляем разницу в днях между текущим юлианским днем
	// и 2451545.0 (полдень 1 января 2000 года по всемирному времени)
	{
		double dJulianDate;           // Юлианская дата
		long int liAux1;              // Вспомогательная переменная 1
		long int liAux2;              // Вспомогательная переменная 2
		
		// Переводим время в десятичные часы
		dDecimalHours = udtTime.dHours + (udtTime.dMinutes + udtTime.dSeconds / 60.0) / 60.0;
		
		// Вычисляем текущий юлианский день по григорианскому календарю
		liAux1 = (udtTime.iMonth - 14) / 12;
		liAux2 = (1461 * (udtTime.iYear + 4800 + liAux1)) / 4 + (367 * (udtTime.iMonth - 2 - 12 * liAux1)) / 12 
				 - (3 * ((udtTime.iYear + 4900 + liAux1) / 100)) / 4 + udtTime.iDay - 32075;
		
		dJulianDate = (double)(liAux2)-0.5 + dDecimalHours / 24.0;
		
		// Вычисляем разницу с эпохой J2000.0
		dElapsedJulianDays = dJulianDate - 2451545.0;
	}

	// Вычисляем эклиптические координаты
	// (эклиптическую долготу и наклон эклиптики в радианах)
	{
		double dMeanLongitude;        // Средняя долгота
		double dMeanAnomaly;          // Средняя аномалия
		double dOmega;                // Долгота восходящего узла лунной орбиты
		
		dOmega = 2.1429 - 0.0010394594 * dElapsedJulianDays;
		dMeanLongitude = 4.8950630 + 0.017202791698 * dElapsedJulianDays;    // В радианах
		dMeanAnomaly = 6.2400600 + 0.0172019699 * dElapsedJulianDays;
		
		// Вычисляем эклиптическую долготу с учетом поправок
		dEclipticLongitude = dMeanLongitude + 0.03341607 * sin(dMeanAnomaly)
			+ 0.00034894 * sin(2 * dMeanAnomaly) - 0.0001134 - 0.0000203 * sin(dOmega);
			
		// Вычисляем наклон эклиптики
		dEclipticObliquity = 0.4090928 - 6.2140e-9 * dElapsedJulianDays + 0.0000396 * cos(dOmega);
	}

	// Вычисляем небесные координаты (прямое восхождение и склонение) в радианах
	{
		double dSin_EclipticLongitude;
		dSin_EclipticLongitude = sin(dEclipticLongitude);
		dY = cos(dEclipticObliquity) * dSin_EclipticLongitude;
		dX = cos(dEclipticLongitude);
		
		// Вычисляем прямое восхождение
		dRightAscension = atan2(dY, dX);
		if (dRightAscension < 0.0) dRightAscension = dRightAscension + twopi;
		
		// Вычисляем склонение
		dDeclination = asin(sin(dEclipticObliquity) * dSin_EclipticLongitude);
	}

	// Вычисляем локальные координаты (азимут и угол к зениту) в градусах
	{
		double dGreenwichMeanSiderealTime;     // Среднее звездное время по Гринвичу
		double dLocalMeanSiderealTime;         // Местное среднее звездное время
		double dLatitudeInRadians;             // Широта в радианах
		double dHourAngle;                     // Часовой угол
		double dCos_Latitude;                  // Косинус широты
		double dSin_Latitude;                  // Синус широты
		double dCos_HourAngle;                 // Косинус часового угла
		double dParallax;                      // Параллакс
		
		// Вычисляем звездное время по Гринвичу
		dGreenwichMeanSiderealTime = 6.6974243242 + 0.0657098283 * dElapsedJulianDays + dDecimalHours;
		
		// Вычисляем местное звездное время
		dLocalMeanSiderealTime = (dGreenwichMeanSiderealTime * 15 + udtLocation.dLongitude) * rad;
		
		// Вычисляем часовой угол
		dHourAngle = dLocalMeanSiderealTime - dRightAscension;
		
		// Переводим широту в радианы и вычисляем тригонометрические функции
		dLatitudeInRadians = udtLocation.dLatitude * rad;
		dCos_Latitude = cos(dLatitudeInRadians);
		dSin_Latitude = sin(dLatitudeInRadians);
		dCos_HourAngle = cos(dHourAngle);
		
		// Вычисляем угол к зениту
		udtSunCoordinates->dZenithAngle = (acos(dCos_Latitude * dCos_HourAngle * cos(dDeclination) 
											  + sin(dDeclination) * dSin_Latitude));
		
		// Вычисляем азимут
		dY = -sin(dHourAngle);
		dX = tan(dDeclination) * dCos_Latitude - dSin_Latitude * dCos_HourAngle;
		udtSunCoordinates->dAzimuth = atan2(dY, dX);
		
		// Корректируем азимут, чтобы он был в диапазоне [0, 2π]
		if (udtSunCoordinates->dAzimuth < 0.0)
			udtSunCoordinates->dAzimuth = udtSunCoordinates->dAzimuth + twopi;
			
		// Переводим азимут из радиан в градусы
		udtSunCoordinates->dAzimuth = udtSunCoordinates->dAzimuth / rad;
		
		// Корректируем угол к зениту с учетом параллакса
		dParallax = (dEarthMeanRadius / dAstronomicalUnit) * sin(udtSunCoordinates->dZenithAngle);
		udtSunCoordinates->dZenithAngle = (udtSunCoordinates->dZenithAngle + dParallax) / rad;
	}
}
