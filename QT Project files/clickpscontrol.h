#ifndef CLICKPSCONTROL_H
#define CLICKPSCONTROL_H

#include <QLabel>
#include <QMouseEvent>
#include <QEvent>

class ClickPSCONTROL : public QLabel
{
    Q_OBJECT
public:
    ClickPSCONTROL(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *ev);

signals:
    void Mouse_Pressed();
};

#endif // CLICKPSCONTROL_H
