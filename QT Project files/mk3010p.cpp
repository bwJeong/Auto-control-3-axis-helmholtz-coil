#include "mk3010p.h"

MK3010P::MK3010P()
{
    port = new QSerialPort();
}

void MK3010P::setPort(QString portName)
{
    port->setPortName(portName);
    port->setBaudRate(QSerialPort::Baud9600);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);
}

QString MK3010P::ISET(QString value) // Unit: A
{
    QString str = "ISET1:" + value;
    return str;
}

QString MK3010P::VSET(QString value) // Unit: V
{
    QString str = "VSET1:" + value;
    return str;
}

QString MK3010P::IOUT()
{
    QString str = "IOUT1?";
    return str;
}

QString MK3010P::VOUT()
{
    QString str = "VOUT1?";
    return str;
}

QString MK3010P::OUT1()
{
    QString str = "OUT1";
    return str;
}

QString MK3010P::OUT0()
{
    QString str = "OUT0";
    return str;
}
