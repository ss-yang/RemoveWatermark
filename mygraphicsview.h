#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include <opencvtool.h>

#include <QGraphicsView>
#include <QMouseEvent>
#include <QString>
#include <QPointF>
#include <QCursor>
#include <QPen>
#include <QColor>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QStack>
#include <QPointF>

using namespace cv;

class MyGraphicsView : public QGraphicsView
{
     Q_OBJECT

public:
    enum ActionName{Pencil, Eraser, BigGlasses, SmallGlasses, OpenHand, ClosedHand, RectSelect, FreeSelect, SelectMove, Default, Forbidden};

    MyGraphicsView(QWidget *parent);
    ~MyGraphicsView();

    inline void clearUndoStack();//清空撤销区
    inline void clearRedoStack();//清空恢复区

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
    void setWidth(int width);//设置线宽
    void setPixmapItem(QGraphicsPixmapItem* item);//设置当前scene中的图片项
    void setCurrentMat(Mat& m);//设置当前的Mat

protected:
    void mouseMoveEvent(QMouseEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);

    void keyReleaseEvent(QKeyEvent *event);

    void actionHandDrag(QMouseEvent *event,QPointF point);//抓手移动实现

    void selectMoving(QMouseEvent *event,QPointF point);//拖动选中区域

    bool isInsideOfRoi(QPointF point);//判断鼠标是否在所选区域内

    void updatePixmapItem();//更新currentMat到PixmapItem

    void roiToCurrentMat();//将选择的区域合成到图片中

private:
    ActionName currentActionName = Default;//记录当前选中的工具
    QPointF startPoint;//鼠标点击起始点
    QPointF endPoint;//鼠标释放点
    bool isZoomUp;//标记放大缩小的状态，使用的原因是QT的键盘响应事件存在问题，具体见：http://z632922970z.blog.163.com/blog/static/16316610320112245372844/
    double zoomUpRate;//放大倍率
    double zoomDownRate;//缩小倍率
    bool isPressed;//标记鼠标左键是否被按下

    QCursor bigCursor;//放大镜鼠标样式
    QCursor smallCursor;//缩小镜鼠标样式
    QCursor pencilCursor;//铅笔鼠标样式
    QCursor eraserCursor;//橡皮鼠标样式
    QCursor forbiddenCursor;//禁止鼠标样式
    QPixmap pixmap;//当前的图片Pixmap
    QGraphicsPixmapItem *pixmapItem;//当前scene中的图片项

    Mat currentMat;//当前的图片Mat
    int thickness;//画笔粗细

    QPixmap roiPixmap;//选择工具所选的区域Pixmap
    Mat roiMat;//选择工具所选的区域Mat
    QGraphicsPixmapItem *roiItem;//选择工具所选的区域Item

    Scalar pencilColor;//铅笔颜色
    Scalar eraserColor;//橡皮颜色

    QStack<QPixmap> undoStack;//撤销
    QStack<QPixmap> redoStack;//恢复

    OpenCVTool opencvTool;//Opencv工具类
};

#endif // MYGRAPHICSVIEW_H
