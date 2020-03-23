#include "powerswitchingmodule.h"

PowerSwitchingModule::PowerSwitchingModule()
{
    port = new QSerialPort();
}

void PowerSwitchingModule::setPort(QString portName)
{
    port->setPortName(portName);
    port->setBaudRate(QSerialPort::Baud9600);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);
}

char PowerSwitchingModule::xAxisPowerSwitching()
{
    return 0x10;
}

char PowerSwitchingModule::yAxisPowerSwitching()
{
    return 0x20;
}

char PowerSwitchingModule::zAxisPowerSwitching()
{
    return 0x30;
}

char PowerSwitchingModule::allAxisStatusCheck()
{
    return 0x40;
}
