#include "mygraphicsview.h"

#include <QList>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPointF>
#include <QScrollBar>
#include <QDebug>

MyGraphicsView::MyGraphicsView(QWidget *parent):QGraphicsView(parent)
{
    this->horizontalScrollBar()->setCursor(Qt::ArrowCursor);
    this->verticalScrollBar()->setCursor(Qt::ArrowCursor);
}

/**
 * @brief MyGraphicsView::mouseMoveEvent
 * @param event
 * 实时显示鼠标下的图片像素位置
 */
void MyGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsScene *scence = this->scene();
    if(scence == NULL){
        return;
    }
    //获得鼠标移动时，当前所指图片中的像素位置
    QList<QGraphicsItem *>  items = scence->items();
    QGraphicsItem *pixmapItem = items.at(0);
    QPointF point = pixmapItem->mapFromScene(mapToScene(event->pos()));
    int width = scence->width();
    int height = scence->height();
    QString location;
    if(point.x() >= 0 && point.x() <= width && point.y() >= 0 && point.y() <= height) {
        location = QString::number(point.x()) + " , " + QString::number(point.y()) + " 像素";
        //检测当前所使用的工具类型，然后动态改变鼠标样式
        switch(this->currentActionName)
        {
            case MyGraphicsView::ActionName::Pencil:{setCursor(Qt::CrossCursor);break;}
            case MyGraphicsView::ActionName::Eraser:{setCursor(Qt::CrossCursor);break;}
            case MyGraphicsView::ActionName::SelectColor:{setCursor(Qt::CrossCursor);break;}
            case MyGraphicsView::ActionName::Glasses:{setCursor(Qt::CrossCursor);break;}
            case MyGraphicsView::ActionName::Hand:{setCursor(Qt::OpenHandCursor);break;}
            case MyGraphicsView::ActionName::RectSelect:{setCursor(Qt::CrossCursor);break;}
            case MyGraphicsView::ActionName::FreeSelect:{setCursor(Qt::CrossCursor);break;}
        default:{setCursor(Qt::CrossCursor);break;}//将鼠标样式设为十字形
        }
    }else {
        setCursor(Qt::ArrowCursor);//鼠标移出图片范围时恢复默认样式
        location = "";
    }
    emit mouseMovetriggerSignal(location);
}

/**
 * @brief MyGraphicsView::setActionName
 * @param actionName
 *
 */
void MyGraphicsView::setActionName(ActionName actionName)
{
    this->currentActionName = actionName;
}
