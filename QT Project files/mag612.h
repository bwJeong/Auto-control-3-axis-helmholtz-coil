#ifndef MAG612_H
#define MAG612_H

#include "LabjackUD.h"

class MAG612
{
private:
    double AIN0, AIN1, AIN2;
    int xAxisMagFieldStrength, yAxisMagFieldStrength, zAxisMagFieldStrength;
public:
    MAG612();
    int connectUSB();
    void disconnectUSB();
    void initialize();
    void collectData();
    void calibrateData();
    int getXAxisMagFieldStrength();
    int getYAxisMagFieldStrength();
    int getZAxisMagFieldStrength();

    LJ_HANDLE lngHandle = 0;
};

#endif // MAG612_H
