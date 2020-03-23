#include "clickpscontrol.h"

ClickPSCONTROL::ClickPSCONTROL(QWidget *parent) :
    QLabel(parent)
{

}

void ClickPSCONTROL::mousePressEvent(QMouseEvent *ev)
{
    emit Mouse_Pressed();
}
