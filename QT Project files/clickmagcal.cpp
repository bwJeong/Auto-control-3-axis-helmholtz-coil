#include "clickmagcal.h"

ClickMAGCAL::ClickMAGCAL(QWidget *parent) :
    QLabel(parent)
{

}

void ClickMAGCAL::mousePressEvent(QMouseEvent *ev)
{
    emit Mouse_Pressed();
}
