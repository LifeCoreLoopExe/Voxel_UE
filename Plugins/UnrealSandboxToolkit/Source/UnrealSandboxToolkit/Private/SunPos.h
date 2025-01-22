// This file is available in electronic form at http://www.psa.es/sdg/sunpos.htm

#ifndef __SUNPOS_H
#define __SUNPOS_H

// Declaration of some constants
#define pi 3.14159265358979323846 // Определяет значение pi
#define twopi (2*pi) // Определяет значение 2*pi
#define rad (pi/180) // Определяет значение радиана
#define dEarthMeanRadius 6371.01 // Средний радиус Земли в км
#define dAstronomicalUnit 149597890 // Астрономическая единица в км

struct cTime { // Структура для хранения времени
    int iYear; // Год
    int iMonth; // Месяц
    int iDay; // День
    double dHours; // Часы
    double dMinutes; // Минуты
    double dSeconds; // Секунды
};

struct cLocation { // Структура для хранения географических координат
    double dLongitude; // Долгота
    double dLatitude; // Широта
};

struct cSunCoordinates { // Структура для хранения координат Солнца
    double dZenithAngle; // Зенитный угол
    double dAzimuth; // Азимут
};

void sunpos(cTime udtTime, cLocation udtLocation, cSunCoordinates *udtSunCoordinates); // Функция для расчета позиции Солнца

#endif
