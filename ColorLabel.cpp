#include "ColorLabel.h"

ColorLabel::ColorLabel(QWidget *parent):QLabel(parent)
{

}

/**
 * @brief ColorLabel::mouseMoveEvent
 * @param event
 * 鼠标按下时，打开调色板
 */
void ColorLabel::mousePressEvent(QMouseEvent *)
{
    emit changeColor();
}
