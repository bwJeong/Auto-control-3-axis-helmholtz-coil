#ifndef POWERSWITCHINGMODULE_H
#define POWERSWITCHINGMODULE_H

#include <QtSerialPort/QSerialPort>

class PowerSwitchingModule
{
public:
    PowerSwitchingModule();
    void setPort(QString portName);
    char xAxisPowerSwitching();
    char yAxisPowerSwitching();
    char zAxisPowerSwitching();
    char allAxisStatusCheck();

    QSerialPort *port;
};

#endif // POWERSWITCHINGMODULE_H
