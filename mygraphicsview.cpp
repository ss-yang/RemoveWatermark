#include "mygraphicsview.h"

#include <QList>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPointF>
#include <QScrollBar>
#include <QDebug>
#include <QTransform>
#include <QApplication>

MyGraphicsView::MyGraphicsView(QWidget *parent):QGraphicsView(parent)
{
    this->horizontalScrollBar()->setCursor(Qt::ArrowCursor);
    this->verticalScrollBar()->setCursor(Qt::ArrowCursor);
}

/**
 * @brief MyGraphicsView::mouseMoveEvent
 * @param event
 * 鼠标移动事件
 */
void MyGraphicsView::mouseMoveEvent(QMouseEvent *event){
    if(this->scene() == NULL){
        this->currentActionName = Default;
        return;
    }
    //获得鼠标移动时，当前所指图片中的像素位置
    QTransform transform;
    transform.rotate(+0.0);
    QGraphicsItem *pixmapItem = this->scene()->itemAt(0, 0,transform);
    QPointF point = pixmapItem->mapFromScene(this->mapToScene(event->pos()));
    QString location;
    //检测鼠标是否在图片范围内
    if(point.x() >= 0 && point.x() <= this->scene()->width() && point.y() >= 0 && point.y() <= this->scene()->height()) {
        location = QString::number(point.x()) + " , " + QString::number(point.y()) + " 像素";
        //检测当前所使用的工具类型，然后动态改变鼠标样式
        switch(this->currentActionName)
        {
            case Pencil:{
                setCursor(Qt::CrossCursor);
                break;
            }
            case Eraser:{
                setCursor(Qt::CrossCursor);
                break;
            }
            case SelectColor:{
                setCursor(Qt::CrossCursor);
                break;
            }
            case Glasses:{
                setCursor(Qt::CrossCursor);
                break;
            }
            case OpenHand:{
                setCursor(Qt::OpenHandCursor);
                break;
            }
            case ClosedHand:{//抓手工具移动时改变滑轮位置，
                setCursor(Qt::ClosedHandCursor);
                if (!(event->buttons() & Qt::LeftButton)){break;}//当左键没有按住时拖动则跳过
               //if ((point - startPoint).manhattanLength() < QApplication::startDragDistance()){break;}//移开一段距离后再进行拖动，但会有延迟感所以注释
                int Ex = point.x() - this->startPoint.x();//鼠标移动到鼠标点击处的横轴距离
                int Ey = point.y() - this->startPoint.y();//鼠标移动到鼠标点击处的纵轴距离
                int length;
                length = this->startPointHorValue - Ex;
                if(Ex >= 0) {
                    if(length > this->horizontalScrollBar()->minimum()){
                        this->horizontalScrollBar()->setValue(length);
                    }else{
                        this->horizontalScrollBar()->setValue(this->horizontalScrollBar()->minimum());
                    }
                }else{
                    if(length < this->horizontalScrollBar()->maximum()) {
                        this->horizontalScrollBar()->setValue(length);
                    }else {
                        this->horizontalScrollBar()->setValue(this->horizontalScrollBar()->maximum());
                    }
                }
                length = this->startPointVerValue - Ey;
                if(Ey >= 0) {
                    if(length > this->verticalScrollBar()->minimum()){
                        this->verticalScrollBar()->setValue(length);
                    }else{
                        this->verticalScrollBar()->setValue(this->verticalScrollBar()->minimum());
                    }
                }else{
                    if(length < this->verticalScrollBar()->maximum()) {
                        this->verticalScrollBar()->setValue(length);
                    }else {
                        this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
                    }
                }
                break;
            }
            case RectSelect:{
                setCursor(Qt::CrossCursor);
                break;
            }
            case FreeSelect:{
                setCursor(Qt::CrossCursor);
                break;
            }
            case Default:{
                setCursor(Qt::CrossCursor);break;
            }
            default:{setCursor(Qt::CrossCursor);break;}//默认将鼠标样式设为十字形
        }
    }else {
        setCursor(Qt::ArrowCursor);//鼠标移出图片范围时恢复默认样式
        location = "";
    }
    //实时显示鼠标下的图片像素位置
    emit mouseMovetriggerSignal(location);
}

/**
 * @brief MyGraphicsView::mousePressEvent
 * @param event
 * 鼠标点击事件（只检测左键点击）
 */
void MyGraphicsView::mousePressEvent(QMouseEvent *event){
    if (!(event->button() == Qt::LeftButton)){
        return;
    }
    if(this->scene() == NULL){
        this->currentActionName = Default;
        return;
    }
    //如果是抓手工具，则在点击时鼠标变成抓紧的样式
    if(this->currentActionName == OpenHand) {
        this->currentActionName = ClosedHand;
        setCursor(Qt::ClosedHandCursor);
    }
    //获得鼠标移动时，当前所指图片中的像素位置
    QTransform transform;
    transform.rotate(+0.0);
    QGraphicsItem *pixmapItem = this->scene()->itemAt(0, 0,transform);
    QPointF point = pixmapItem->mapFromScene(this->mapToScene(event->pos()));
    //检测鼠标是否在图片范围内
    if(point.x() >= 0 && point.x() <= this->scene()->width() && point.y() >= 0 && point.y() <= this->scene()->height()) {
        this->startPoint = point;
        this->startPointHorValue = this->horizontalScrollBar()->value();
        this->startPointVerValue = this->verticalScrollBar()->value();
    }else {
        return;
    }
}

/**
 * @brief MyGraphicsView::mouseReleaseEvent
 * @param event
 * 鼠标点击释放事件（只检测左键点击）
 */
void MyGraphicsView::mouseReleaseEvent(QMouseEvent *event){
    if (!(event->button() == Qt::LeftButton)){
        return;
    }
    if(this->scene() == NULL){
        this->currentActionName = Default;
        return;
    }
    //如果是抓手工具，则在点击释放时鼠标变成松开的样式
    if(this->currentActionName == ClosedHand) {
        this->currentActionName = OpenHand;
        setCursor(Qt::OpenHandCursor);
    }
    //获得鼠标移动时，当前所指图片中的像素位置
    QTransform transform;
    transform.rotate(+0.0);
    QGraphicsItem *pixmapItem = this->scene()->itemAt(0, 0,transform);
    QPointF point = pixmapItem->mapFromScene(this->mapToScene(event->pos()));
    //检测鼠标是否在图片范围内
    if(point.x() >= 0 && point.x() <= this->scene()->width() && point.y() >= 0 && point.y() <= this->scene()->height()) {
        this->endPoint = point;
    }else {
        return;
    }
}

/**
 * @brief MyGraphicsView::setActionName
 * @param actionName
 * 设置当前对象中所使用的工具类型
 */
void MyGraphicsView::setActionName(ActionName actionName)
{
    this->currentActionName = actionName;
}
