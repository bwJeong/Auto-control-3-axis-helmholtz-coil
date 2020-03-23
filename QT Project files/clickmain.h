#ifndef CLICKMAIN_H
#define CLICKMAIN_H

#include <QLabel>
#include <QMouseEvent>
#include <QEvent>

class ClickMAIN : public QLabel
{
    Q_OBJECT
public:
    explicit ClickMAIN(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *ev);

signals:
    void Mouse_Pressed();
};

#endif // CLICKMAIN_H
