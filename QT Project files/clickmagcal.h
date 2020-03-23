#ifndef CLICKMAGCAL_H
#define CLICKMAGCAL_H

#include <QLabel>
#include <QMouseEvent>
#include <QEvent>

class ClickMAGCAL : public QLabel
{
    Q_OBJECT
public:
    explicit ClickMAGCAL(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *ev);

signals:
    void Mouse_Pressed();
};

#endif // CLICKMAGCAL_H
