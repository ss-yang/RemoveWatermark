#include "ImageGraphicsview.h"

ImageGraphicsview::ImageGraphicsview(QWidget *parent):QGraphicsView(parent)
{
    this->horizontalScrollBar()->setCursor(Qt::ArrowCursor);
    this->verticalScrollBar()->setCursor(Qt::ArrowCursor);
    this->isZoomUp = true;//默认放大镜工具为放大状态
    this->zoomUpRate=1.5;//默认的放大倍率，预计从设置中更改
    this->zoomDownRate=0.75;//默认的缩小倍率
    this->isPressed = false;//默认鼠标左键没有被按下

    QPixmap bigGlassesPixmap(":/Icons/icon/bigGlasses.png");
    bigCursor =QCursor(bigGlassesPixmap);
    QPixmap smallGlassesPixmap(":/Icons/icon/smallGlasses.png");
    smallCursor =QCursor(smallGlassesPixmap);
    QPixmap pencilPixmap(":/Icons/icon/pencil_24px.png");
    pencilCursor =QCursor(pencilPixmap, 0, pencilPixmap.height());//让鼠标焦点位与图片左下角，默认为图片中心
    QPixmap eraserPixmap(":/Icons/icon/eraser_24px.png");
    eraserCursor =QCursor(eraserPixmap);
    QPixmap forbiddenPixmap(":/Icons/icon/forbidden.png");
    forbiddenCursor =QCursor(forbiddenPixmap);

    opencvTool = OpenCVTool();//初始化工具类
    thickness = 1;//初始化画笔粗细
    pencilColor = Scalar(0,0,0,255);//初始化铅笔颜色 黑色
    eraserColor = Scalar(255,255,255,0);//初始化橡皮颜色 白色
}

ImageGraphicsview::~ImageGraphicsview(){
    delete pixmapItem;
    delete roiItem;
    delete maskItem;
}

/**
 * @brief ImageGraphicsview::clearUndoStack
 * 清空撤销区
 */
inline void ImageGraphicsview::clearUndoStack()
{
    while(!undoStack.empty())
    {
        undoStack.pop();
    }
}

/**
 * @brief ImageGraphicsview::clearRedoStack
 * 清空恢复区
 */
inline void ImageGraphicsview::clearRedoStack()
{
    while(!redoStack.empty())
    {
        redoStack.pop();
    }
}

/**
 * @brief ImageGraphicsview::mouseMoveEvent
 * @param event
 * 鼠标移动事件
 */
void ImageGraphicsview::mouseMoveEvent(QMouseEvent *event){
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
                    opencvTool.drawLine(maskMat, startPoint.toPoint(), point.toPoint(), pencilColor, thickness);
                    updateMaskItem();
                    startPoint = point;
                }
                break;
            }
            case Eraser:{
                setCursor(eraserCursor);
                if(isPressed) {
                    opencvTool.drawLine(maskMat, startPoint.toPoint(), point.toPoint(), eraserColor, thickness);
                    updateMaskItem();
                    startPoint = point;
                }
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
                if(isInsideOfRoi(point)) {
                    setCursor(Qt::SizeAllCursor);
                    selectMoving(event, this->mapToScene(event->pos()));
                }else {
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
 * @brief ImageGraphicsview::mousePressEvent
 * @param event
 * 鼠标点击事件（只检测左键点击）
 */
void ImageGraphicsview::mousePressEvent(QMouseEvent *event){
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
        emit actionNameChanged(currentActionName);
        setCursor(Qt::ClosedHandCursor);
    }
    //获得鼠标移动时，当前所指图片中的像素位置
    QPointF point = this->pixmapItem->mapFromScene(this->mapToScene(event->pos()));
    //检测鼠标是否在图片范围内
    if(point.x() >= 0 && point.x() <= this->pixmapItem->pixmap().width() && point.y() >= 0 && point.y() <= this->pixmapItem->pixmap().height()) {
        this->startPoint = point;
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
    //当为铅笔或者橡皮工具时
    if(this->currentActionName == Pencil || this->currentActionName == Eraser) {
        clearRedoStack();//当铅笔工具或橡皮工具开心新的操作时，则清空恢复区
        if(this->currentActionName == Pencil) {//当为铅笔工具时，前景色画点
            opencvTool.drawLine(maskMat, point.toPoint(), point.toPoint(), pencilColor, thickness);
            updateMaskItem();
        }
        if(this->currentActionName == Eraser) {//当为橡皮工具时，背景色画点
            opencvTool.drawLine(maskMat, point.toPoint(), point.toPoint(), eraserColor, thickness);
            updateMaskItem();
        }
    }
    //当为工具为selectMove时,判断鼠标点击的位置
    if(this->currentActionName == SelectMove) {
        if(!isInsideOfRoi(point)){//判断鼠标是否在区域外，若在则将区域合成到maskMat中，然后删除roiItem
            roiToMaskMat();//将选择的区域合成到图片中
            updateMaskItem();
            this->currentActionName = RectSelect;
        }
    }
}

/**
 * @brief ImageGraphicsview::mouseReleaseEvent
 * @param event
 * 鼠标点击释放事件（只检测左键点击）
 */
void ImageGraphicsview::mouseReleaseEvent(QMouseEvent *event){
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
    if(this->currentActionName == Pencil || this->currentActionName == Eraser) {

    }
    //如果是矩形选择工具，则画矩形
    if(this->currentActionName == RectSelect && startPoint != endPoint) {
        Mat tempMat = opencvTool.mask2CurrentMat(maskMat, currentMat);
        roiMat = opencvTool.selectRectRoi(tempMat, startPoint.toPoint(), endPoint.toPoint());
        roiPixmap = opencvTool.MatToPixmap(roiMat);
        roiItem = new QGraphicsPixmapItem(roiPixmap);

        QPointF temp = QPointF();
        if(startPoint.x() > endPoint.x()){
            temp.setX(endPoint.x());
        }else{
            temp.setX(startPoint.x());
        }
        if(startPoint.y() > endPoint.y()){
            temp.setY(endPoint.y());
        }else{
            temp.setY(startPoint.y());
        }
        roiItem->setPos(temp);//设置位置
        roiItem->setFlag(QGraphicsItem::ItemIsSelectable);//设置可选
        roiItem->setFlag(QGraphicsItem::ItemIsMovable);//设置可移动
        roiItem->setSelected(true);//设置被选中
        //加入到scene中
        this->scene()->addItem(roiItem);
        this->scene()->setFocusItem(roiItem);
        this->scene()->update();
        this->currentActionName = SelectMove;//设置当前的工具
    }
}

/**
 * @brief ImageGraphicsview::keyPressEvent
 * @param event
 * 监听键盘按键按下
 */
void ImageGraphicsview::keyPressEvent(QKeyEvent *event){
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
 * @brief ImageGraphicsview::keyReleaseEvent
 * @param event
 * 监听键盘按键松开
 */
void ImageGraphicsview::keyReleaseEvent(QKeyEvent *event){
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
 * @brief ImageGraphicsview::setActionName
 * @param actionName
 * 设置当前对象中所使用的工具类型
 */
void ImageGraphicsview::setActionName(ActionName actionName)
{
    if(this->currentActionName == SelectMove) {//当从selectmove切换到其他工具时，若roi区域还未合成到图片中，则将其合成到图片中
        if(roiItem->isSelected()){
            roiToMaskMat();
            updateMaskItem();
        }
    }
    this->currentActionName = actionName;
}

/**
 * @brief ImageGraphicsview::zoomUp
 * @param event
 * 放大scence
 */
void ImageGraphicsview::zoomUp(){
    scale(zoomUpRate,zoomUpRate);
}

/**
 * @brief ImageGraphicsview::zoomDown
 * @param event
 * 缩小scence
 */
void ImageGraphicsview::zoomDown(){
    scale(zoomDownRate,zoomDownRate);
}

/**
 * @brief ImageGraphicsview::setGlasses
 * @param isZoomUp
 * 改变放大镜工具状态
 */
void ImageGraphicsview::setGlasses(bool flag)
{
    if(flag) {
        this->currentActionName = BigGlasses;
    }else{
        this->currentActionName = SmallGlasses;
    }
    this->isZoomUp = flag;
}

/**
 * @brief ImageGraphicsview::setPencilColor
 * @param color
 * 设置铅笔工具颜色
 */
void ImageGraphicsview::setPencilColor(QColor color){
    pencilColor = Scalar(color.blue(), color.green(), color.red(), 255);
}

/**
 * @brief ImageGraphicsview::serEraserColor
 * @param color
 * 设置橡皮工具颜色
 */
void ImageGraphicsview::setEraserColor(QColor color){
    eraserColor = Scalar(color.blue(), color.green(), color.red(), 0);
}

/**
 * @brief ImageGraphicsview::setPencilWidth
 * @param width
 * 设置线宽
 */
void ImageGraphicsview::setWidth(int width){
    thickness = width;
}


/**
 * @brief ImageGraphicsview::actionHandDrag
 * 抓手移动实现
 */
void ImageGraphicsview::actionHandDrag(QMouseEvent *event,QPointF point){
    if (!(event->buttons() & Qt::LeftButton)){return;}//当左键没有按住时拖动则跳过
    int Ex = point.x() - this->startPoint.x();//鼠标移动到鼠标点击处的横轴距离
    int Ey = point.y() - this->startPoint.y();//鼠标移动到鼠标点击处的纵轴距离
    this->horizontalScrollBar()->setValue(horizontalScrollBar()->value()-Ex);
    this->verticalScrollBar()->setValue(verticalScrollBar()->value()-Ey);
}

/**
 * @brief ImageGraphicsview::setPixmapItem
 * @param item
 * 设置当前scene中的图片项
 */
void ImageGraphicsview::setPixmapItem(QGraphicsPixmapItem *item){
    this->pixmapItem = item;
    initMaskItem();//初始化图层
}

/**
 * @brief ImageGraphicsview::setCurrentMat
 * @param m
 * 设置当前的Mat
 */
void ImageGraphicsview::setCurrentMat(Mat &m)
{
    currentMat = m;
}

/**
 * @brief ImageGraphicsview::selectMoving
 * @param event
 * @param point
 * 拖动选中区域
 */
void ImageGraphicsview::selectMoving(QMouseEvent *event, QPointF point)
{
    if (!(event->buttons() & Qt::LeftButton)){return;}//当左键没有按住时拖动则跳过
    QPointF sceneStartPoint = pixmapItem->mapToScene(startPoint);
    qreal Ex = point.x() - sceneStartPoint.x();//鼠标移动到鼠标点击处的横轴距离
    qreal Ey = point.y() - sceneStartPoint.y();//鼠标移动到鼠标点击处的纵轴距离
    qreal currentX = roiItem->scenePos().x() + Ex;
    qreal currentY = roiItem->scenePos().y() + Ey;
    //if(currentX < 0.0) {currentX = 0.0;}
    //if(currentX > (pixmap.width() - roiPixmap.width())) {currentX = pixmap.width() - roiPixmap.width();}
    //if(currentY < 0.0) {currentY = 0.0;}
    //if(currentY > (pixmap.height() - roiPixmap.height())) {currentY = pixmap.height() - roiPixmap.height();}
    roiItem->setPos(currentX, currentY);
    startPoint = pixmapItem->mapFromScene(point);
}

/**
 * @brief ImageGraphicsview::isInsideOfRoi
 * @param point
 * @return
 * 判断鼠标是否在所选区域内
 */
bool ImageGraphicsview::isInsideOfRoi(QPointF point)
{
    if(point.x()>roiItem->scenePos().x() && point.y()>roiItem->scenePos().y() && point.x()<(roiItem->scenePos().x()+roiPixmap.width()) && point.y()<(roiItem->scenePos().y()+roiPixmap.height()))
    {
        return true;
    }
    return false;
}

/**
 * @brief ImageGraphicsview::updatePixmapItem
 * 更新currentMat到PixmapItem
 */
void ImageGraphicsview::updatePixmapItem()
{
    pixmap = opencvTool.MatToPixmap(currentMat);
    pixmapItem->setPixmap(pixmap);
    pixmapItem->update();
}

/**
 * @brief ImageGraphicsview::updateMaskItem
 * 更新maskMat到maskItem
 */
void ImageGraphicsview::updateMaskItem()
{
    maskPixmap = opencvTool.MatToPixmap(maskMat);
    maskItem->setPixmap(maskPixmap);
    maskItem->update();
}

/**
 * @brief ImageGraphicsview::initMaskItem
 * 初始化maskItem
 */
inline void ImageGraphicsview::initMaskItem()
{
    this->maskMat = Mat(pixmapItem->pixmap().height(), pixmapItem->pixmap().width(), CV_8UC4, Scalar::all(0));//生成一个全透明的图层
    maskPixmap = opencvTool.MatToPixmap(maskMat);
    maskItem = new QGraphicsPixmapItem(maskPixmap);
    this->scene()->addItem(maskItem);
    this->scene()->update();
}

/**
 * @brief ImageGraphicsview::roiToMaskMat
 * 将选择的区域合成到图片中
 */
void ImageGraphicsview::roiToMaskMat()
{
    QPointF a,b;
    a = QPointF(roiItem->x(), roiItem->y());
    b = QPointF(roiItem->x() + roiItem->pixmap().width(), roiItem->y() + roiItem->pixmap().height());
    Mat tempMat = maskMat.clone();//在新图上覆盖的原因：copyto函数在执行时，若移动的区域和原来的区域有重合的话，覆盖会不合理
    Mat tempRoi = opencvTool.selectRectRoi(tempMat, a.toPoint(), b.toPoint());
    //将roiMat覆盖到maskMat上
    Mat alpha = Mat(roiMat.rows, roiMat.cols, CV_8UC1, Scalar(255));
    Mat in[] = {roiMat, alpha};
    int fromTo1[] = {0,0, 1,1, 2,2, 3,3};
    mixChannels(in, 2, &tempRoi, 1, fromTo1, 4);
    maskMat = tempMat;
    roiItem->setSelected(false);//设置为未选中
    this->scene()->removeItem(roiItem);
}

/**
 * @brief ImageGraphicsview::undo
 * 撤销
 */
void ImageGraphicsview::undo()
{

}

/**
 * @brief ImageGraphicsview::redo
 * 恢复
 */
void ImageGraphicsview::redo()
{

}

/**
 * @brief ImageGraphicsview::reset
 * 重置变量状态
 */
void ImageGraphicsview::reset()
{
    isZoomUp = true;
    isPressed = false;
    clearRedoStack();
    clearUndoStack();
    currentActionName = Default;
    startPoint = QPointF(0.0,0.0);
    endPoint = QPointF(0.0,0.0);
}
