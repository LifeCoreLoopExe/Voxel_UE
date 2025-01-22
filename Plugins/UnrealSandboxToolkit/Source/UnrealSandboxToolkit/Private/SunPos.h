// Этот файл доступен в электронном виде по адресу http://www.psa.es/sdg/sunpos.htm

#ifndef __SUNPOS_H  // Защита от повторного включения заголовочного файла
#define __SUNPOS_H

// Объявление некоторых констант
#define pi    3.14159265358979323846  // Значение числа π
#define twopi (2*pi)                   // Значение 2π
#define rad   (pi/180)                 // Конвертация градусов в радианы
#define dEarthMeanRadius     6371.01   // Средний радиус Земли в км
#define dAstronomicalUnit    149597890  // Астрономическая единица в км

// Структура для хранения информации о времени
struct cTime
{
	int iYear;        // Год
	int iMonth;       // Месяц
	int iDay;         // День
	double dHours;    // Часы
	double dMinutes;  // Минуты
	double dSeconds;  // Секунды
};

// Структура для хранения информации о местоположении
struct cLocation
{
	double dLongitude;  // Долгота
	double dLatitude;   // Широта
};

// Структура для хранения координат Солнца
struct cSunCoordinates
{
	double dZenithAngle;  // Угол зенита
	double dAzimuth;      // Азимут
};

// Функция для вычисления положения Солнца
void sunpos(cTime udtTime, cLocation udtLocation, cSunCoordinates *udtSunCoordinates);

#endif  // Конец блока защиты от повторного включения
