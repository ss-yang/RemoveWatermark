#include "mypixmapitem.h"

#include <QPixmap>
#include <QPoint>
#include <QLine>
#include <QPen>

MyPixmapItem::MyPixmapItem(){}

void MyPixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)//这个宏是用来把不用到的参数注掉的功能
    currentPixmap = this->pixmap();
    QPainter p(&currentPixmap);
    foreach (MyLine *line, undoStack) {
        p.setPen(line->getPen());
        p.drawLine(line->getStartPoint(),line->getEndPoint());
    }
    painter->drawPixmap(currentPixmap.rect(),currentPixmap);
}

/**
 * @brief MyPixmapItem::clearUndoStack
 * 清空撤销区
 */
void MyPixmapItem::clearUndoStack()
{
    while(!undoStack.empty())
    {
        MyLine *tmp = undoStack.top();
        undoStack.pop();
        delete tmp;
    }
}

/**
 * @brief MyPixmapItem::clearRedoStack
 * 清空恢复区
 */
void MyPixmapItem::clearRedoStack()
{
    while(!redoStack.empty())
    {
        MyLine *tmp = redoStack.top();
        redoStack.pop();
        delete tmp;
    }
}

/**
 * @brief MyPixmapItem::saveCurrentPixmap
 * @param path
 * 保存当前画好的图片
 */
void MyPixmapItem::saveCurrentPixmap(QString path)
{
    if(!currentPixmap.isNull())
    {
        path = "C:/Users/huiyi/Desktop/test.jpg";
        currentPixmap.save(path);
    }
}
