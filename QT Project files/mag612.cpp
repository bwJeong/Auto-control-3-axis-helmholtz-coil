#include "mag612.h"

#define CANT_CONNECT 0
#define CONNECTION 1

MAG612::MAG612()
{
    xAxisMagFieldStrength = 0;
    yAxisMagFieldStrength = 0;
    zAxisMagFieldStrength = 0;

    initialize();
}

int MAG612::connectUSB()
{
    if (OpenLabJack (LJ_dtU6, LJ_ctUSB, "1", 1, &lngHandle) == 0) {
        return CONNECTION;
    }
    else {
        return CANT_CONNECT;
    }
}

void MAG612::disconnectUSB()
{
    Close();
}

void MAG612::initialize()
{
    ePut (lngHandle, LJ_ioPUT_CONFIG, LJ_chAIN_RESOLUTION, 0, 0);
    ePut (lngHandle,  LJ_ioPUT_AIN_RANGE, 0, LJ_rgBIP10V, 0);
    ePut (lngHandle,  LJ_ioPUT_AIN_RANGE, 1, LJ_rgBIP10V, 0);
    ePut (lngHandle,  LJ_ioPUT_AIN_RANGE, 2, LJ_rgBIP10V, 0);
}

void MAG612::collectData()
{
    long lngIOType = 0;
    long lngChannel_0 = 0, lngChannel_1 = 1, lngChannel_2 = 2;

    AddRequest (lngHandle, LJ_ioGET_AIN, 0, 0, 0, 0);
    AddRequest (lngHandle, LJ_ioGET_AIN, 1, 0, 0, 0);
    AddRequest (lngHandle, LJ_ioGET_AIN, 2, 0, 0, 0);

    GoOne(lngHandle);

    GetFirstResult(lngHandle, &lngIOType, &lngChannel_0, &AIN0, 0, 0);
    GetNextResult(lngHandle, &lngIOType, &lngChannel_1, &AIN1, 0, 0);
    GetNextResult(lngHandle, &lngIOType, &lngChannel_2, &AIN2, 0, 0);
}

void MAG612::calibrateData()
{
    // 100,000 nT / 10 V
    xAxisMagFieldStrength = AIN0 * 10000;
    yAxisMagFieldStrength = AIN1 * 10000;
    zAxisMagFieldStrength = AIN2 * 10000;
}

int MAG612::getXAxisMagFieldStrength()
{
    return xAxisMagFieldStrength;
}

int MAG612::getYAxisMagFieldStrength()
{
    return yAxisMagFieldStrength;
}

int MAG612::getZAxisMagFieldStrength()
{
    return zAxisMagFieldStrength;
}
