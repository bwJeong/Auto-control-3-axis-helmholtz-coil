#include "clickmain.h"

ClickMAIN::ClickMAIN(QWidget *parent) :
    QLabel(parent)
{

}

void ClickMAIN::mousePressEvent(QMouseEvent *ev)
{
    emit Mouse_Pressed();
}
