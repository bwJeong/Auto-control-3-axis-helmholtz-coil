#include "clickconfig.h"

ClickCONFIG::ClickCONFIG(QWidget *parent) :
    QLabel(parent)
{

}

void ClickCONFIG::mousePressEvent(QMouseEvent *ev)
{
    emit Mouse_Pressed();
}
