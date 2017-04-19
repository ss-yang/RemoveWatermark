#ifndef COLORLABEL_H
#define COLORLABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QColor>

class ColorLabel : public QLabel
{
    Q_OBJECT

public:
    ColorLabel(QWidget *parent);

signals:
    void changeColor();

protected:
    void mousePressEvent(QMouseEvent *);
};

#endif // COLORLABEL_H
