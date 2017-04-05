#ifndef MYPIXMAPITEM_H
#define MYPIXMAPITEM_H

#include "myline.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QStack>
#include <QPixmap>

class MyPixmapItem : public QGraphicsPixmapItem
{
public:
    MyPixmapItem();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);//用来绘制该item的东西 必须

    QStack<MyLine *> undoStack;//撤销
    QStack<MyLine *> redoStack;//恢复

    void clearUndoStack();//清空撤销区
    void clearRedoStack();//清空恢复区

    void saveCurrentPixmap(QString path);

private:
    QPixmap currentPixmap;//当前画好的图形
};

#endif // MYPIXMAPITEM_H
