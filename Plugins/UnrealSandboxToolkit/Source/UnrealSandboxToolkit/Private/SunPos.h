#ifndef __SUNPOS_H
#define __SUNPOS_H

// Объявление некоторых констант
#define pi 3.14159265358979323846
#define twopi (2*pi)
#define rad (pi/180)
#define dEarthMeanRadius 6371.01 // В км
#define dAstronomicalUnit 149597890 // В км
struct cTime {
    int iYear;
    int iMonth;
    int iDay;
    double dHours;
    double dMinutes;
    double dSeconds;
};
struct cLocation {
    double dLongitude;
    double dLatitude;
};

struct cSunCoordinates {
    double dZenithAngle;
    double dAzimuth;
};
void sunpos(cTime udtTime, cLocation udtLocation, cSunCoordinates *udtSunCoordinates);
#endif
