#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include "myline.h"
#include "mypixmapitem.h"

#include <QGraphicsView>
#include <QMouseEvent>
#include <QString>
#include <QPointF>
#include <QCursor>
#include <QPen>
#include <QColor>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QStack>

class MyGraphicsView : public QGraphicsView
{
     Q_OBJECT

public:
    enum ActionName{Pencil, Eraser, BigGlasses, SmallGlasses, OpenHand, ClosedHand, RectSelect, FreeSelect, Default, Forbidden};

    MyGraphicsView(QWidget *parent);
    ~MyGraphicsView();

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
    void setPencilColor(QColor color);//设置铅笔工具颜色
    void setEraserColor(QColor color);//设置橡皮工具颜色
    void setPencilWidth(int width);//设置铅笔工具线宽
    void setEraserWidth(int width);//设置橡皮工具线宽
    void setPixmap(QPixmap &map);//设置当前的图片
    void setPixmapItem(MyPixmapItem* item);//设置当前scene中的图片项

protected:
    void mouseMoveEvent(QMouseEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);

    void keyReleaseEvent(QKeyEvent *event);

    void actionHandDrag(QMouseEvent *event,QPointF point);

private:
    ActionName currentActionName = Default;//记录当前选中的工具
    QPointF startPoint;//鼠标点击起始点
    QPointF endPoint;//鼠标释放点
    int startPointHorValue;//鼠标点击时横轴滑轮的位置
    int startPointVerValue;//鼠标点击时纵轴滑轮的位置
    bool isZoomUp;//标记放大缩小的状态，使用的原因是QT的键盘响应事件存在问题，具体见：http://z632922970z.blog.163.com/blog/static/16316610320112245372844/
    double zoomUpRate;//放大倍率
    double zoomDownRate;//缩小倍率
    bool isPressed;//标记鼠标左键是否被按下

    QCursor bigCursor;//放大镜鼠标样式
    QCursor smallCursor;//缩小镜鼠标样式
    QCursor pencilCursor;//铅笔鼠标样式
    QCursor eraserCursor;//橡皮鼠标样式
    QCursor forbiddenCursor;//禁止鼠标样式

    QPen pencilPen;//铅笔工具
    QPen eraserPen;//橡皮工具

    QPixmap pixmap;//当前图片
    MyPixmapItem *pixmapItem;//当前scene中的图片项

    MyLine *currentLine;//铅笔/橡皮工具当前所指向的点
};

#endif // MYGRAPHICSVIEW_H
