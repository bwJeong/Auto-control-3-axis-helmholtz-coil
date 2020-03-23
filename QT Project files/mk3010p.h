#ifndef MK3010P_H
#define MK3010P_H

#include <QtSerialPort/QSerialPort>

class MK3010P
{
public:
    MK3010P();
    void setPort(QString portName);
    QString ISET(QString value);
    QString VSET(QString value);
    QString IOUT();
    QString VOUT();
    QString OUT1();
    QString OUT0();

    QSerialPort *port;
};

#endif // MK3010P_H
