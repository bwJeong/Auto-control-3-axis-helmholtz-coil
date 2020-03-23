#ifndef CLICKCONFIG_H
#define CLICKCONFIG_H

#include <QLabel>
#include <QMouseEvent>
#include <QEvent>

class ClickCONFIG : public QLabel
{
    Q_OBJECT
public:
    explicit ClickCONFIG(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *ev);

signals:
    void Mouse_Pressed();
};

#endif // CLICKCONFIG_H
