#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QString>
#include <QPointF>

class MyGraphicsView : public QGraphicsView
{
     Q_OBJECT

public:
    enum ActionName{Pencil, Eraser, bigGlasses, smallGlasses, OpenHand, ClosedHand, RectSelect, FreeSelect, Default};

    MyGraphicsView(QWidget *parent);

    void setActionName(ActionName);

signals:
    void mouseMovetriggerSignal(QString location);
    void zoomUpPressed(QMouseEvent *event);
    void zoomDownPressed(QMouseEvent *event);

public slots:
    void zoomUp(QMouseEvent *event);
    void zoomDown(QMouseEvent *event);

protected:
    void mouseMoveEvent(QMouseEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);

    void keyReleaseEvent(QKeyEvent *event);

private:
    ActionName currentActionName = Default;//记录当前选中的工具
    QPointF startPoint;//鼠标点击起始点
    QPointF endPoint;//鼠标释放点
    int startPointHorValue;//鼠标点击时横轴滑轮的位置
    int startPointVerValue;//鼠标点击时纵轴滑轮的位置
    bool isZoomUp;//放大、缩小标记(判断缩放时用）
    double zoomUpRate;//放大倍率
    double zoomDownRate;//缩小倍率
};

#endif // MYGRAPHICSVIEW_H
