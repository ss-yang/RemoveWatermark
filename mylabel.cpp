#include "mylabel.h"

#include <QLabel>

MyLabel::MyLabel(QWidget *parent):QLabel(parent)
{

}

/**
 * @brief MyLabel::mouseMoveEvent
 * @param event
 * 鼠标按下时，打开调色板
 */
void MyLabel::mousePressEvent(QMouseEvent *)
{
    emit changeColor();
}
