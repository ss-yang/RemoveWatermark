#ifndef MYLABEL_H
#define MYLABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QColor>

class MyLabel : public QLabel
{
    Q_OBJECT

public:
    MyLabel(QWidget *parent);

signals:
    void changeColor();

protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // MYLABEL_H
