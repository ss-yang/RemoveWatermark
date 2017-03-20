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
    enum ActionName{Pencil, Eraser, SelectColor, Glasses, OpenHand, ClosedHand, RectSelect, FreeSelect, Default};

    MyGraphicsView(QWidget *parent);

    void setActionName(ActionName);

signals:
    void mouseMovetriggerSignal(QString location);

protected:
    void mouseMoveEvent(QMouseEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

private:
    ActionName currentActionName = Default;//记录当前选中的工具
    QPointF startPoint;//鼠标点击起始点
    QPointF endPoint;//鼠标释放点
    int startPointHorValue;//鼠标点击时横轴滑轮的位置
    int startPointVerValue;//鼠标点击时纵轴滑轮的位置
};

#endif // MYGRAPHICSVIEW_H
