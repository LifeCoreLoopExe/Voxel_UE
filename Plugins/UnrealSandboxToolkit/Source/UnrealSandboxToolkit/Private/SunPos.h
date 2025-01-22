// Этот файл доступен в электронном виде по адресу http://www.psa.es/sdg/sunpos.htm

#ifndef __SUNPOS_H
#define __SUNPOS_H

// Объявление важных математических констант 
#define pi    3.14159265358979323846    // Число Пи - математическая константа
#define twopi (2*pi)                    // Два Пи (полный оборот в радианах)
#define rad   (pi/180)                  // Коэффициент для перевода градусов в радианы
#define dEarthMeanRadius     6371.01	   // Средний радиус Земли в километрах
#define dAstronomicalUnit    149597890  // Астрономическая единица (среднее расстояние от Земли до Солнца) в километрах

// Структура для хранения времени
struct cTime
{
	int iYear;      // Год
	int iMonth;     // Месяц
	int iDay;       // День
	double dHours;  // Часы
	double dMinutes;// Минуты
	double dSeconds;// Секунды
};

// Структура для хранения географических координат
struct cLocation
{
	double dLongitude; // Долгота местности
	double dLatitude;  // Широта местности
};

// Структура для хранения координат Солнца
struct cSunCoordinates
{
	double dZenithAngle; // Зенитный угол (угол между направлением на Солнце и вертикалью)
	double dAzimuth;     // Азимут (угол между проекцией направления на Солнце на горизонтальную плоскость и направлением на север)
};

// Функция для вычисления положения Солнца
// Принимает время (udtTime), местоположение (udtLocation)
// Записывает результат в структуру udtSunCoordinates
void sunpos(cTime udtTime, cLocation udtLocation, cSunCoordinates *udtSunCoordinates);

#endif