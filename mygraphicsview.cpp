#include "mygraphicsview.h"

#include <QList>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPointF>
#include <QPoint>
#include <QScrollBar>
#include <QDebug>
#include <QApplication>
#include <QCursor>
#include <QPixmap>
#include <QPainter>


MyGraphicsView::MyGraphicsView(QWidget *parent):QGraphicsView(parent)
{
    this->horizontalScrollBar()->setCursor(Qt::ArrowCursor);
    this->verticalScrollBar()->setCursor(Qt::ArrowCursor);
    this->isZoomUp = true;//默认放大镜工具为放大状态
    this->zoomUpRate=1.5;//默认的放大倍率，预计从设置中更改
    this->zoomDownRate=0.75;//默认的缩小倍率
    this->isPressed = false;//默认鼠标左键没有被按下

    QPixmap bigGlassesPixmap(":/myIcons/icon/bigGlasses.png");
    bigCursor =QCursor(bigGlassesPixmap);
    QPixmap smallGlassesPixmap(":/myIcons/icon/smallGlasses.png");
    smallCursor =QCursor(smallGlassesPixmap);
    QPixmap pencilPixmap(":/myIcons/icon/pencil_24px.png");
    pencilCursor =QCursor(pencilPixmap, 0, pencilPixmap.height());//让鼠标焦点位与图片左下角，默认为图片中心
    QPixmap eraserPixmap(":/myIcons/icon/eraser_24px.png");
    eraserCursor =QCursor(eraserPixmap);
    QPixmap forbiddenPixmap(":/myIcons/icon/forbidden.png");
    forbiddenCursor =QCursor(forbiddenPixmap);

    opencvTool = OpenCVTool();//初始化工具类
    thickness = 1;//初始化画笔粗细
    pencilColor = Scalar(0,0,0);//初始化铅笔颜色 黑色
    eraserColor = Scalar(255,255,255);//初始化橡皮颜色 白色

//    roiItem = new QGraphicsPixmapItem;//初始化roiItem
    hasSelected = false;//默认未选择roi

}

MyGraphicsView::~MyGraphicsView(){
    delete pixmapItem;
}

/**
 * @brief MyGraphicsView::clearUndoStack
 * 清空撤销区
 */
inline void MyGraphicsView::clearUndoStack()
{
    while(!undoStack.empty())
    {
        undoStack.pop();
    }
}

/**
 * @brief MyGraphicsView::clearRedoStack
 * 清空恢复区
 */
inline void MyGraphicsView::clearRedoStack()
{
    while(!redoStack.empty())
    {
        redoStack.pop();
    }
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
    QPointF point = this->pixmapItem->mapFromScene(this->mapToScene(event->pos()));
    QString location;
    //检测鼠标是否在图片范围内
    if(point.x() >= 0 && point.x() <= this->pixmapItem->pixmap().width() && point.y() >= 0 && point.y() <= this->pixmapItem->pixmap().height()) {
        //坐标强制转为整型，从而使得缩小放大时不会显示亚像素位置
        location = QString::number((int)point.x()) + " , " + QString::number((int)point.y()) + " 像素";
        //检测当前所使用的工具类型，然后动态改变鼠标样式
        switch(this->currentActionName)
        {
            case Pencil:{
                setCursor(pencilCursor);
                if(isPressed) {
                    opencvTool.drawLine(currentMat, startPoint.toPoint(), point.toPoint(), pencilColor, thickness);
                    updatePixmapItem();
                    startPoint = point;
                }
                break;
            }
            case Eraser:{
                setCursor(eraserCursor);
                break;
            }
            case BigGlasses:{
                setCursor(bigCursor);
                break;
            }
            case SmallGlasses:{
                setCursor(smallCursor);
                break;
            }
            case OpenHand:{
                setCursor(Qt::OpenHandCursor);
                break;
            }
            case ClosedHand:{//抓手工具移动时改变滑轮位置，
                setCursor(Qt::ClosedHandCursor);
                actionHandDrag(event,point);//拖动图片
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
            case SelectMove:{
                if(isInsideofRoi(point)){
                    setCursor(Qt::SizeAllCursor);
                    selectMoving(event,point);
                }else{
                    setCursor(Qt::CrossCursor);
                }
                break;
            }
            case Forbidden:{
                setCursor(forbiddenCursor);break;
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
    //获得鼠标移动时，当前所指图片中的像素位置

    if(this->scene() == NULL){
        this->currentActionName = Default;
        return;
    }
    //如果是抓手工具，则在点击时鼠标变成抓紧的样式
    if(this->currentActionName == OpenHand) {
        //改变光标样式
        this->currentActionName = ClosedHand;
        emit actionNameChanged(currentActionName);
        setCursor(Qt::ClosedHandCursor);
    }
    QPointF point = this->pixmapItem->mapFromScene(this->mapToScene(event->pos()));
    if(this->currentActionName == SelectMove ){
        //判断光标是否在选择区域，否则将选择区域roi合成到mat中，更新到pixmap,删除roiItem
        if(hasSelected){
            if(!isInsideofRoi(point)){
                QPoint *a,*b;
                a = new QPoint(this->roiItem->x(),this->roiItem->y());
                b = new QPoint(this->roiItem->x() + this->roiItem->pixmap().width(),
                           this->roiItem->y() + this->roiItem->pixmap().height());
                Mat tempRoi = opencvTool.selectRoi(currentMat,*a,*b);//移动后的roi
                this->mroi.copyTo(tempRoi);
                roiItem->clearFocus();
                this->hasSelected = false;
                this->scene()->removeItem(roiItem);
                updatePixmapItem();
                setActionName(RectSelect);
            }
        }
    }
    //检测鼠标是否在图片范围内
    if(point.x() >= 0 && point.x() <= this->pixmapItem->pixmap().width() && point.y() >= 0 && point.y() <= this->pixmapItem->pixmap().height()) {
        this->startPoint = point;
        this->startPointHorValue = this->horizontalScrollBar()->value();
        this->startPointVerValue = this->verticalScrollBar()->value();
    }else {
        return;
    }
    //当为放大镜工具时，点击放大
    if(this->currentActionName == BigGlasses) {
        if(this->isZoomUp){
            this->centerOn(this->mapToScene(event->pos()));
            emit zoomUpPressed();//放大
        }
    }
    //当为缩小工具时，点击缩小
    if(this->currentActionName == SmallGlasses) {
        if(!this->isZoomUp) {
            this->centerOn(this->mapToScene(event->pos()));
            emit zoomDownPressed();//缩小
        }
    }
    //设置鼠标左键被按下
    this->isPressed = true;
    if(this->currentActionName == Pencil || this->currentActionName == Eraser) {
        //当铅笔工具或橡皮工具开心新的操作时，则清空恢复区
        clearRedoStack();
        //当为铅笔工具时，前景色画点
        if(this->currentActionName == Pencil) {
            opencvTool.drawLine(currentMat, point.toPoint(), point.toPoint(), pencilColor, thickness);
            updatePixmapItem();
        }
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
        emit actionNameChanged(currentActionName);
        setCursor(Qt::OpenHandCursor);
    }
    //获得鼠标移动时，当前所指图片中的像素位置
    QPointF point = this->pixmapItem->mapFromScene(this->mapToScene(event->pos()));
    //检测鼠标是否在图片范围内
    if(point.x() >= 0 && point.x() <= this->pixmapItem->pixmap().width() && point.y() >= 0 && point.y() <= this->pixmapItem->pixmap().height()) {
        this->endPoint = point;
    }else {
        return;
    }
    this->isPressed = false;//设置鼠标左键被按下
    //画矩形
    if(this->currentActionName == RectSelect){
        mroi = opencvTool.selectRoi(currentMat,startPoint.toPoint(),point.toPoint());
        roi = opencvTool.MatToPixmap(mroi);
        roiItem = new QGraphicsPixmapItem;
        roiItem->setPixmap(roi);

        int gapX = point.x() - startPoint.x();
        int gapY = point.y() - startPoint.y();
        // 2
        if(gapY<0 && gapX>0){
            roiItem->moveBy(startPoint.x(),startPoint.y() - abs(gapY));
        }
        // 3
        else if(gapX<0 && gapY<0){
            roiItem->moveBy(point.x(),point.y());
        }

        // 4
        else if(gapX<0 && gapY>0){
            roiItem->moveBy(point.x(),point.y() - abs(gapY));
        }
        // 1
        else{
            roiItem->moveBy(startPoint.x(),startPoint.y());
        }

        this->scene()->addItem(roiItem);
        this->scene()->setFocusItem(roiItem);
        this->scene()->update();

        roiItem->setFlag(QGraphicsItem::ItemIsSelectable);//设置可选
        roiItem->setFlag(QGraphicsItem::ItemIsMovable);//设置可移动
        roiItem->setSelected(true);
        this->hasSelected = true;

        setActionName(SelectMove);
    }
    if(this->currentActionName == Pencil || this->currentActionName == Eraser) {

    }
}

/**
 * @brief MyGraphicsView::keyPressEvent
 * @param event
 * 监听键盘按键按下
 */
void MyGraphicsView::keyPressEvent(QKeyEvent *event){
    if(this->scene() == NULL){
        this->currentActionName = Default;
        return;
    }
    if(event->isAutoRepeat()) {//按键重复响应时不执行
        return;
    }
    //当为放大镜工具时，按下control则变为缩小工具
    if(this->currentActionName == BigGlasses) {
        if(event->key()== Qt::Key_Control){
            if(isZoomUp) {//这里是真正按下触发的事件
                this->currentActionName = SmallGlasses;
                emit glassesChanged(false);
                setCursor(smallCursor);
            }
            isZoomUp = false;
        }
    }
}

/**
 * @brief MyGraphicsView::keyReleaseEvent
 * @param event
 * 监听键盘按键松开
 */
void MyGraphicsView::keyReleaseEvent(QKeyEvent *event){
    if(this->scene() == NULL){
        this->currentActionName = Default;
        return;
    }
    if(event->isAutoRepeat()) {//按键重复响应时不执行
        return;
    }
    //当为缩小工具时，松开control则变为放大镜工具
    if(this->currentActionName == SmallGlasses) {
        if(event->key()== Qt::Key_Control){
            if(!isZoomUp) {//这里是真正松开触发的事件
                this->currentActionName = BigGlasses;
                emit glassesChanged(true);
                setCursor(bigCursor);
            }
            isZoomUp = true;
        }
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

/**
 * @brief MyGraphicsView::zoomUp
 * @param event
 * 放大scence
 */
void MyGraphicsView::zoomUp(){
    scale(zoomUpRate,zoomUpRate);
}

/**
 * @brief MyGraphicsView::zoomDown
 * @param event
 * 缩小scence
 */
void MyGraphicsView::zoomDown(){
    scale(zoomDownRate,zoomDownRate);
}

/**
 * @brief MyGraphicsView::setGlasses
 * @param isZoomUp
 * 改变放大镜工具状态
 */
void MyGraphicsView::setGlasses(bool flag)
{
    if(flag) {
        this->currentActionName = BigGlasses;
    }else{
        this->currentActionName = SmallGlasses;
    }
    this->isZoomUp = flag;
}

/**
 * @brief MyGraphicsView::setPencilColor
 * @param color
 * 设置铅笔工具颜色
 */
void MyGraphicsView::setPencilColor(QColor color){
    pencilColor = Scalar(color.blue(), color.green(), color.red());
}

/**
 * @brief MyGraphicsView::serEraserColor
 * @param color
 * 设置橡皮工具颜色
 */
void MyGraphicsView::setEraserColor(QColor color){
    eraserColor = Scalar(color.blue(), color.green(), color.red());
}

/**
 * @brief MyGraphicsView::setPencilWidth
 * @param width
 * 设置线宽
 */
void MyGraphicsView::setWidth(int width){
    thickness = width;
}


/**
 * @brief MyGraphicsView::actionHandDrag
 * 拖动图片
 */
void MyGraphicsView::actionHandDrag(QMouseEvent *event,QPointF point){
    if (!(event->buttons() & Qt::LeftButton)){return;}//当左键没有按住时拖动则跳过
    int Ex = point.x() - this->startPoint.x();//鼠标移动到鼠标点击处的横轴距离
    int Ey = point.y() - this->startPoint.y();//鼠标移动到鼠标点击处的纵轴距离
    //推动Graphicview
    this->horizontalScrollBar()->setValue(horizontalScrollBar()->value()-Ex);
    this->verticalScrollBar()->setValue(verticalScrollBar()->value()-Ey);
}

void MyGraphicsView::selectMoving(QMouseEvent *event, QPointF point)
{
    if (!(event->buttons() & Qt::LeftButton)){return;}//当左键没有按住时拖动则跳过
    int Ex = point.x() - this->startPoint.x();//鼠标移动到鼠标点击处的横轴距离
    int Ey = point.y() - this->startPoint.y();//鼠标移动到鼠标点击处的纵轴距离
    roiItem->setX(roiItem->x()+Ex);
    roiItem->setY(roiItem->y()+Ey);
}

/**
 * @brief MyGraphicsView::isInsideofRoi
 * @param point
 * @return true if point inside of roi
 * 判断当前点是否在Roi选择区域内
 */
bool MyGraphicsView::isInsideofRoi(QPointF point)
{
    if(point.x()>roiItem->x() && point.x()<roiItem->x() + roiItem->pixmap().width() &&
            point.y()>roiItem->y() && point.y()<roiItem->y() + roiItem->pixmap().height()){
        return true;
    }
    return false;
}

/**
 * @brief MyGraphicsView::updatePixmapItem
 * 更新currentMat到PixmapItem
 */
void MyGraphicsView::updatePixmapItem()
{
    pixmap = opencvTool.MatToPixmap(currentMat);
    pixmapItem->setPixmap(pixmap);
    pixmapItem->update();
}

/**
 * @brief setPixmapItem
 * @param item
 * 设置当前scene中的图片项
 */
void MyGraphicsView::setPixmapItem(QGraphicsPixmapItem *item){
    this->pixmapItem = item;
}

/**
 * @brief MyGraphicsView::setCurrentMat
 * @param m
 * 设置当前的Mat
 */
void MyGraphicsView::setCurrentMat(Mat &m)
{
    currentMat = m;
}
