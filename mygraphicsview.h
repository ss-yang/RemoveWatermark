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
    int startPointHorValue;
    int startPointVerValue;
};

#endif // MYGRAPHICSVIEW_H
