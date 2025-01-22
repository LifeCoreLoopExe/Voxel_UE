// Этот файл доступен в электронном виде по адресу http://www.psa.es/sdg/sunpos.htm

#ifndef __SUNPOS_H  // Если __SUNPOS_H не определен
#define __SUNPOS_H  // Определяем __SUNPOS_H, чтобы избежать повторного включения этого заголовочного файла

// Объявление некоторых констант
#define pi    3.14159265358979323846  // Число Пи
#define twopi (2*pi)  // Дважды Пи
#define rad   (pi/180)  // Константа для преобразования градусов в радианы
#define dEarthMeanRadius     6371.01  // Средний радиус Земли в километрах
#define dAstronomicalUnit    149597890  // Астрономическая единица в километрах

// Структура для хранения времени
struct cTime
{
    int iYear;  // Год
    int iMonth;  // Месяц
    int iDay;  // День
    double dHours;  // Часы
    double dMinutes;  // Минуты
    double dSeconds;  // Секунды
};

// Структура для хранения географического положения
struct cLocation
{
    double dLongitude;  // Долгота
    double dLatitude;  // Широта
};

// Структура для хранения координат Солнца
struct cSunCoordinates
{
    double dZenithAngle;  // Зенитный угол
    double dAzimuth;  // Азимут
};

// Функция для вычисления позиции Солнца
void sunpos(cTime udtTime, cLocation udtLocation, cSunCoordinates *udtSunCoordinates);

#endif  // Конец условной компиляции
