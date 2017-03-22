#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QString>
#include <QPointF>
#include <QCursor>

class MyGraphicsView : public QGraphicsView
{
     Q_OBJECT

public:
    enum ActionName{Pencil, Eraser, bigGlasses, smallGlasses, OpenHand, ClosedHand, RectSelect, FreeSelect, Default};

    MyGraphicsView(QWidget *parent);

signals:
    void mouseMovetriggerSignal(QString location);
    void zoomUpPressed();
    void zoomDownPressed();
    void actionNameChanged(ActionName);
    void glassesChanged(bool isZoomUp);//放大镜工具改变信号

public slots:
    void zoomUp();
    void zoomDown();
    void setActionName(ActionName actionName);
    void setGlasses(bool flag);//改变放大镜工具状态

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
    bool isZoomUp;//标记放大缩小的状态，使用的原因是QT的键盘响应事件存在问题，具体见：http://z632922970z.blog.163.com/blog/static/16316610320112245372844/
    double zoomUpRate;//放大倍率
    double zoomDownRate;//缩小倍率
    QCursor bigCursor;//放大镜鼠标样式
    QCursor smallCursor;//缩小镜鼠标样式
};

#endif // MYGRAPHICSVIEW_H
