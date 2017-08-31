#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /**
     * 初始化图片列表
     */
    markedImageModel = new QFileSystemModel(this);
    //QDir::NoDotAndDotDot 将Windows中的"."和".."目录忽略
    markedImageModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);

    QStringList filter;
    filter<<"*.jpg"<<"*.png"<<"*.bmp"<<"*.jpeg"<<"*.JPG"<<"*.PNG"<<"*.BMP"<<"*.JPEG";

    markedImageModel->setNameFilters(filter);//过滤非指定类型
    markedImageModel->setNameFilterDisables(false);//隐藏被过滤的文件，没有此句的话不符合指定类型的文件以灰色不可选显示
    ui->LoadImageListView->setModel(markedImageModel);
    ui->LoadImageListView->setRootIndex(markedImageModel->setRootPath(curLoadImageDirPath));
    ui->LoadImagePathLineEdit->setText(curLoadImageDirPath);

    unMarkedImageModel = new QFileSystemModel(this);
    unMarkedImageModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);

    unMarkedImageModel->setNameFilters(filter);//过滤非指定类型
    unMarkedImageModel->setNameFilterDisables(false);//隐藏被过滤的文件
    ui->SaveImageListView->setModel(unMarkedImageModel);
    ui->SaveImageListView->setRootIndex(unMarkedImageModel->setRootPath(curSaveImageDirPath));
    ui->SaveImagePathLineEdit->setText(curSaveImageDirPath);

    /**
     * 初始化前景色和背景色
     */
    palette = ui->ForeColorLabel->palette();
    palette.setColor(QPalette::Background, QColor(0,0,0));//黑色
    ui->ForeColorLabel->setPalette(palette);
    palette = ui->BackColorLabel->palette();
    palette.setColor(QPalette::Background, QColor(255,255,255));//白色
    ui->BackColorLabel->setPalette(palette);

    ui->getResultAction->setEnabled(false);

    /**
     * 初始化粗细设置按钮
     */
    thicknessSlider = new ThicknessSlider(this);
    thicknessAction = ui->extraToolBar->addWidget(thicknessSlider);

    /**
     * 初始化OpenCV工具类
     */
    opencvtool = OpenCVTool();

    /**
     * 初始化水印区域的位置
     */
    X=0;Y=0;HEIGHT=0;WIDTH=0;
    ui->XLineEdit->setText(QString::number(X));
    ui->YLineEdit->setText(QString::number(Y));
    ui->WLineEdit->setText(QString::number(WIDTH));
    ui->HLineEdit->setText(QString::number(HEIGHT));

    // 初始化两个GraphicsView的Scene
    oriScene = new QGraphicsScene;
    currentScene = new QGraphicsScene;

    //当加载图片后，在状态栏显示鼠标所指向的图片的像素位置
    QObject::connect(ui->OriImageGraphicsView,SIGNAL(mouseMovetriggerSignal(QString)),this,SLOT(updatePixelLocationLabel(QString)));
    QObject::connect(ui->CurrentImageGraphicsView,SIGNAL(mouseMovetriggerSignal(QString)),this,SLOT(updatePixelLocationLabel(QString)));
    //当视图滚动条移动时，同步视图滚动条的位置
    QObject::connect(ui->OriImageGraphicsView->horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(OriHor2CurHorScrollBar(int)));
    QObject::connect(ui->OriImageGraphicsView->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(OriVer2CurVerScrollBar(int)));
    QObject::connect(ui->CurrentImageGraphicsView->horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(CurHor2OriHorScrollBar(int)));
    QObject::connect(ui->CurrentImageGraphicsView->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(CurVer2OriVerScrollBar(int)));
    //设置前景色和背景色
    QObject::connect(ui->ForeColorLabel,SIGNAL(changeColor()),this,SLOT(changeForeColor()));
    QObject::connect(ui->BackColorLabel,SIGNAL(changeColor()),this,SLOT(changeBackColor()));
    //放大镜工具时，点击图像按预定倍数放大放大
    QObject::connect(ui->CurrentImageGraphicsView,SIGNAL(zoomUpPressed()),ui->CurrentImageGraphicsView,SLOT(zoomUp()));
    QObject::connect(ui->CurrentImageGraphicsView,SIGNAL(zoomUpPressed()),ui->OriImageGraphicsView,SLOT(zoomUp()));
    QObject::connect(ui->OriImageGraphicsView,SIGNAL(zoomUpPressed()),ui->OriImageGraphicsView,SLOT(zoomUp()));
    QObject::connect(ui->OriImageGraphicsView,SIGNAL(zoomUpPressed()),ui->CurrentImageGraphicsView,SLOT(zoomUp()));
    QObject::connect(ui->CurrentImageGraphicsView,SIGNAL(zoomDownPressed()),ui->CurrentImageGraphicsView,SLOT(zoomDown()));
    QObject::connect(ui->CurrentImageGraphicsView,SIGNAL(zoomDownPressed()),ui->OriImageGraphicsView,SLOT(zoomDown()));
    QObject::connect(ui->OriImageGraphicsView,SIGNAL(zoomDownPressed()),ui->OriImageGraphicsView,SLOT(zoomDown()));
    QObject::connect(ui->OriImageGraphicsView,SIGNAL(zoomDownPressed()),ui->CurrentImageGraphicsView,SLOT(zoomDown()));
    //同步改变两侧当前的actionName
    QObject::connect(ui->OriImageGraphicsView,SIGNAL(actionNameChanged(ActionName)),ui->CurrentImageGraphicsView,SLOT(setActionName(ActionName)));
    QObject::connect(ui->CurrentImageGraphicsView,SIGNAL(actionNameChanged(ActionName)),ui->OriImageGraphicsView,SLOT(setActionName(ActionName)));
    //同步改变两侧放大镜的状态
    QObject::connect(ui->OriImageGraphicsView,SIGNAL(glassesChanged(bool)),ui->CurrentImageGraphicsView,SLOT(setGlasses(bool)));
    QObject::connect(ui->CurrentImageGraphicsView,SIGNAL(glassesChanged(bool)),ui->OriImageGraphicsView,SLOT(setGlasses(bool)));
    //当粗细发生改变时，同步改变铅笔和橡皮的粗细
    QObject::connect(thicknessSlider,SIGNAL(valueChanged(int)),ui->CurrentImageGraphicsView,SLOT(setWidth(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete markedImageModel;
    delete unMarkedImageModel;
    delete oriScene;
    delete oriPixmapItem;
    delete currentScene;
    delete currentPixmapItem;
    delete thicknessSlider;
    delete thicknessAction;
    delete maskItem;
    delete resultItem;
    delete maskScene;
    delete resultScene;
}

/**
 * @brief MainWindow::on_Open_triggered
 * 打开图片目录
 */
void MainWindow::on_Open_triggered()
{
    QString path = QFileDialog::getExistingDirectory(this,"请选择水印图像所在文件夹",QDir::currentPath());
    //当用户没有点击取消按钮时弹出
    if(!path.isEmpty()) {
        curLoadImageDirPath = path;
        ui->LoadImageListView->setRootIndex(markedImageModel->setRootPath(curLoadImageDirPath));
        ui->LoadImagePathLineEdit->setText(curLoadImageDirPath);
    }
}

/**
 * @brief MainWindow::on_LoadImageListView_doubleClicked
 * @param index
 * 原始图片列表双击打开目录 或 打开图片
 */
void MainWindow::on_LoadImageListView_doubleClicked(const QModelIndex &index)
{
    QString path = markedImageModel->fileInfo(index).absoluteFilePath();//获取到绝对路径
    if(markedImageModel->fileInfo(index).isDir()) {//是目录则进入目录
        curLoadImageDirPath = path;
        ui->LoadImageListView->setRootIndex(markedImageModel->setRootPath(path));
        ui->LoadImagePathLineEdit->setText(path);
    }else if(!oriPixmap.load(path) || !currentPixmap.load(path)){//是图片则读取图片显示
        QMessageBox::warning(this,"提示","打开图像失败！");
    }else {
        //当当前图片未保存时，提示保存
        if(ui->CurrentImageGraphicsView->scene() != NULL && ui->CurrentImageGraphicsView->isModified()) {//
            int reply = QMessageBox::warning(this,tr("提示"),tr("图片尚未保存，是否保存该图片？"),tr("保存"),tr("不保存"),tr("取消"),0,2);
            if(reply == 0) {//保存图片
                on_Save_triggered();
            }else if(reply == 2){//取消，直接返回，该次操作无效
                return;
            }//不保存，忽略图片，执行下一步
        }
        if(ui->CurrentImageGraphicsView->scene() != NULL && ui->CurrentImageGraphicsView->isSaved()){//当图片保存了，提示是否将其带入到计算
            int reply = QMessageBox::warning(this,tr("提示"),tr("是否将该图片带入到计算中？"),tr("是"),tr("否"),0,1);
            if(reply == 0) {//是，则将其存入到calculateImg中
                ImagePair img = ImagePair(markedImageDirPath, unMarkedImageDirPath, markedMat.clone(), unmarkedMat.clone());
                calculateImg.push_back(img);
                qDebug()<< "calculateImg Size:" <<calculateImg.size();
            }//否，执行下一步
        }
        //获取到图片的文件名，并去除文件名中的格式
        markedImageFileName = markedImageModel->fileInfo(index).fileName();
        int first = markedImageFileName.lastIndexOf ("."); //从后面查找"."位置
        markedImageFileName = markedImageFileName.left(first);//去除文件名中的格式
        //判断将要打开的图片在集合中是否存在，若存在则从集合中取出，同时将其从集合中删除
        string pa = (const char *)path.toLocal8Bit();//QString转string;
        int result = isContainMarkedImg(pa);//判断是否存在
        if(result != 0) {
            ImagePair img = calculateImg[result - 1];
            calculateImg.erase(calculateImg.begin()+result-1);
            markedImageDirPath = img.markedImgPath;
            unMarkedImageDirPath = img.unmarkedImgPath;
            markedMat = img.markedMat;
            unmarkedMat = img.unMarkedMat;
        }else{//不存在，则读取图片
            markedImageDirPath = pa;
            unMarkedImageDirPath.clear();//清空修改图片的路径
            markedMat = imread(markedImageDirPath, 1);
            unmarkedMat = markedMat.clone();
        }
        oriPixmap = opencvtool.MatToPixmap(markedMat);
        currentPixmap = opencvtool.MatToPixmap(unmarkedMat);
        ui->OriImageGraphicsView->setCurrentMat(markedMat);
        ui->CurrentImageGraphicsView->setCurrentMat(unmarkedMat);
        QString width,height;
        //更新状态栏图片大小信息
        ui->ImageSizeLabel->setText(width.setNum(oriPixmap.width()) + " × " + height.setNum(oriPixmap.height()) + " 像素");
        resetAction();//重置工具栏选中状态
        //视图栏显示图片
        oriPixmapItem = new QGraphicsPixmapItem(oriPixmap);
        oriPixmapItem->setPixmap(oriPixmap);
        oriScene->clear(); // 每次打开新图片前，为防止内存泄漏，先清空之前的scene | Removes and deletes all items from the scene, but otherwise leaves the state of the scene unchanged.
        oriScene->addItem(oriPixmapItem);
        ui->OriImageGraphicsView->reset();//重置状态
        ui->OriImageGraphicsView->setScene(oriScene);
        ui->OriImageGraphicsView->show();
        ui->OriImageGraphicsView->setPixmapItem(oriPixmapItem);
        currentPixmapItem = new QGraphicsPixmapItem(currentPixmap);
        currentPixmapItem->setPixmap(currentPixmap);
        currentScene->clear();
        currentScene->addItem(currentPixmapItem);
        ui->CurrentImageGraphicsView->reset();//重置状态
        ui->CurrentImageGraphicsView->setScene(currentScene);
        ui->CurrentImageGraphicsView->show();
        ui->CurrentImageGraphicsView->setPixmapItem(currentPixmapItem);
    }
}

/**
 * @brief MainWindow::on_SaveImageListView_doubleClicked
 * @param index
 * 保存图片列表双击打开目录
 */
void MainWindow::on_SaveImageListView_doubleClicked(const QModelIndex &index)
{
    QString path = unMarkedImageModel->fileInfo(index).absoluteFilePath();
    if(unMarkedImageModel->fileInfo(index).isDir()) {
        curSaveImageDirPath = path;
        ui->SaveImageListView->setRootIndex(unMarkedImageModel->setRootPath(path));
        ui->SaveImagePathLineEdit->setText(path);
    }
}

/**
 * @brief MainWindow::on_LoadImagePathLineEdit_returnPressed
 * 原始图片路径响应回车
 */
void MainWindow::on_LoadImagePathLineEdit_returnPressed()
{
    QString path = ui->LoadImagePathLineEdit->text().trimmed();
    QDir dir(path);
    if(dir.exists()) {
        curLoadImageDirPath = path;
        ui->LoadImageListView->setRootIndex(markedImageModel->setRootPath(path));
        ui->LoadImagePathLineEdit->setText(path);
    }else {
        QMessageBox::warning(this,"提示","目录路径错误！");
    }
}

/**
 * @brief MainWindow::on_SaveImagePathLineEdit_returnPressed
 * 保存图片路径响应回车
 */
void MainWindow::on_SaveImagePathLineEdit_returnPressed()
{
    QString path = ui->SaveImagePathLineEdit->text().trimmed();
    QDir dir(path);
    if(dir.exists()) {
        curSaveImageDirPath = path;
        ui->SaveImageListView->setRootIndex(unMarkedImageModel->setRootPath(path));
        ui->SaveImagePathLineEdit->setText(path);
    }else {
        QMessageBox::warning(this,"提示","目录路径错误！");
    }
}

/**
 * @brief MainWindow::on_LoadImagePathButton_clicked
 * 原始图片路径按钮返回上一级
 */
void MainWindow::on_LoadImagePathButton_clicked()
{
    int first = curLoadImageDirPath.lastIndexOf ("/"); //从后面查找"/"位置
    if(first < 0)
    {
        first = curLoadImageDirPath.lastIndexOf ("\\"); //从后面查找"\"位置
    }
    QString path = curLoadImageDirPath.left(first);//获得上一级路径
    if(first < 0) {path = "";}
    curLoadImageDirPath = path;
    ui->LoadImageListView->setRootIndex(markedImageModel->setRootPath(path));
    ui->LoadImagePathLineEdit->setText(path);
}

/**
 * @brief MainWindow::on_SaveImagePathButton_clicked
 * 保存图片路径按钮返回上一级
 */
void MainWindow::on_SaveImagePathButton_clicked()
{
    int first = curSaveImageDirPath.lastIndexOf ("/"); //从后面查找"/"位置
    if(first < 0)
    {
        first = curSaveImageDirPath.lastIndexOf ("\\"); //从后面查找"\"位置
    }
    QString path = curSaveImageDirPath.left(first);//获得上一级路径
    if(first < 0) {path = "";}
    curSaveImageDirPath = path;
    ui->SaveImageListView->setRootIndex(unMarkedImageModel->setRootPath(path));
    ui->SaveImagePathLineEdit->setText(path);
}

/**
 * @brief MainWindow::updatePixelLocationLabel
 * @param event
 * 当加载图片后，在状态栏显示鼠标所指向的图片的像素位置
 */
void MainWindow::updatePixelLocationLabel(QString location)
{
    ui->PixelLocationLabel->setText(location);
}

/**
 * @brief MainWindow::OriHor2CurHorScrollBar
 * 当点击原始视图水平滚动条移动时，同步操作视图水平滚动条的位置
 */
void MainWindow::OriHor2CurHorScrollBar(int value)
{
    ui->CurrentImageGraphicsView->horizontalScrollBar()->setValue(value);
}

/**
 * @brief MainWindow::OriVer2CurVerScrollBar
 * 当点击原始视图垂直滚动条移动时，同步操作视图垂直滚动条的位置
 */
void MainWindow::OriVer2CurVerScrollBar(int value)
{
    ui->CurrentImageGraphicsView->verticalScrollBar()->setValue(value);
}

/**
 * @brief MainWindow::CurHor2OriHorScrollBar
 * 当点击操作视图水平滚动条移动时，同步原始视图水平滚动条的位置
 */
void MainWindow::CurHor2OriHorScrollBar(int value)
{
    ui->OriImageGraphicsView->horizontalScrollBar()->setValue(value);
}

/**
 * @brief MainWindow::CurVer2OriVerScrollBar
 * 当点击操作视图垂直滚动条移动时，同步原始视图垂直滚动条的位置
 */
void MainWindow::CurVer2OriVerScrollBar(int value)
{
    ui->OriImageGraphicsView->verticalScrollBar()->setValue(value);
}

/**
 * @brief MainWindow::on_pencil_triggered
 * 铅笔工具
 */
void MainWindow::on_Pencil_triggered()
{
    resetAction();
    ui->Pencil->setChecked(true);
    ui->OriImageGraphicsView->setActionName(ImageGraphicsview::Forbidden);
    ui->CurrentImageGraphicsView->setActionName(ImageGraphicsview::Pencil);

}

/**
 * @brief MainWindow::on_Eraser_triggered
 * 橡皮工具
 */
void MainWindow::on_Eraser_triggered()
{
    resetAction();
    ui->Eraser->setChecked(true);
    ui->OriImageGraphicsView->setActionName(ImageGraphicsview::Forbidden);
    ui->CurrentImageGraphicsView->setActionName(ImageGraphicsview::Eraser);
}

/**
 * @brief MainWindow::on_Glasses_triggered
 * 放大镜工具
 */
void MainWindow::on_Glasses_triggered()
{
    resetAction();
    ui->Glasses->setChecked(true);
    ui->OriImageGraphicsView->setActionName(ImageGraphicsview::BigGlasses);
    ui->CurrentImageGraphicsView->setActionName(ImageGraphicsview::BigGlasses);
}

/**
 * @brief MainWindow::on_Hand_triggered
 * 抓手工具
 */
void MainWindow::on_Hand_triggered()
{
    resetAction();
    ui->Hand->setChecked(true);
    ui->OriImageGraphicsView->setActionName(ImageGraphicsview::OpenHand);
    ui->CurrentImageGraphicsView->setActionName(ImageGraphicsview::OpenHand);
}

/**
 * @brief MainWindow::on_RectSelect_triggered
 * 矩形选择工具
 */
void MainWindow::on_RectSelect_triggered()
{
    resetAction();
    ui->RectSelect->setChecked(true);
    ui->OriImageGraphicsView->setActionName(ImageGraphicsview::Forbidden);
    ui->CurrentImageGraphicsView->setActionName(ImageGraphicsview::RectSelect);
}

/**
 * @brief MainWindow::on_FreeSelect_triggered
 * 自由选择工具
 */
void MainWindow::on_FreeSelect_triggered()
{
    resetAction();
    ui->FreeSelect->setChecked(true);
    ui->OriImageGraphicsView->setActionName(ImageGraphicsview::Forbidden);
    ui->CurrentImageGraphicsView->setActionName(ImageGraphicsview::FreeSelect);
}

/**
 * @brief MainWindow::on_GetColor_triggered
 * 编辑颜色工具
 */
void MainWindow::on_GetColor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::white,this,"设置前景色");
    palette = ui->ForeColorLabel->palette();
    palette.setColor(QPalette::Background, color);
    ui->ForeColorLabel->setPalette(palette);
    ui->CurrentImageGraphicsView->setPencilColor(color);//设置编辑视图中铅笔工具颜色
}

/**
 * @brief MainWindow::resetAction
 * 重置QAction的选中状态
 */
inline void MainWindow::resetAction() {
    ui->Pencil->setChecked(false);
    ui->Eraser->setChecked(false);
    ui->Glasses->setChecked(false);
    ui->Hand->setChecked(false);
    ui->RectSelect->setChecked(false);
    ui->FreeSelect->setChecked(false);
}

/**
 * @brief MainWindow::isContainMarkedImg
 * @param markedPath
 * @return
 * 集合中是否存在水印图
 */
int MainWindow::isContainMarkedImg(string markedPath)
{
    ImagePair img;
    for(unsigned int i =0; i< calculateImg.size();i++) {
        img = calculateImg[i];
        if(img.markedImgPath == markedPath) {
            return i+1;
        }
    }
    return 0;
}

/**
 * @brief MainWindow::isContainUnMarkedImg
 * @param unmarkedPath
 * @return
 * 集合中是否存在去水印图
 */
int MainWindow::isContainUnMarkedImg(string unmarkedPath)
{
    ImagePair img;
    for(unsigned int i =0; i< calculateImg.size();i++) {
        img = calculateImg[i];
        if(img.unmarkedImgPath == unmarkedPath) {
            return i+1;
        }
    }
    return 0;
}

/**
 * @brief MainWindow::changeForeColor
 * 改变前景色
 */
void MainWindow::changeForeColor()
{
    QColor color = QColorDialog::getColor(Qt::white,this,"设置前景色");
    palette = ui->ForeColorLabel->palette();
    palette.setColor(QPalette::Background, color);
    ui->ForeColorLabel->setPalette(palette);
    ui->CurrentImageGraphicsView->setPencilColor(color);//设置编辑视图中铅笔工具颜色
}

/**
 * @brief MainWindow::changeBackColor
 * 改变背景色
 */
void MainWindow::changeBackColor()
{
    QColor color = QColorDialog::getColor(Qt::white,this,"设置背景色");
    palette = ui->BackColorLabel->palette();
    palette.setColor(QPalette::Background, color);
    ui->BackColorLabel->setPalette(palette);
    ui->CurrentImageGraphicsView->setEraserColor(color);//设置编辑视图中橡皮工具颜色
}

/**
 * @brief MainWindow::on_Save_triggered
 * 保存当前的图片
 */
void MainWindow::on_Save_triggered()
{
    if(ui->CurrentImageGraphicsView->scene() ==NULL) {//没有加载图片时，不执行
        return;
    }
    if(!ui->CurrentImageGraphicsView->isModified()) {//已被保存未修改过，不执行
        return;
    }
    if(unMarkedImageDirPath.length() == 0) {//当保存修改的图片路径为空时
        QString targetPath = curSaveImageDirPath + "/" + markedImageFileName;
        QString path = QFileDialog::getSaveFileName(this,"保存修改后文件",targetPath,"*.bmp");
        if(!path.isEmpty()) {
            unMarkedImageDirPath = (const char *)path.toLocal8Bit();//QString转string
            unmarkedMat = ui->CurrentImageGraphicsView->saveCurrentMat(unMarkedImageDirPath);//获取到保存的Mat
            // 保存后，修改图片状态
            ui->CurrentImageGraphicsView->setSaved(true);
            ui->CurrentImageGraphicsView->setModified(false);
            ui->CurrentImageGraphicsView->makeMaskUnion(); // 保存图片后，将图片的修改区域叠加到maskUnion中
            //更新保存目录控件
            int first = path.lastIndexOf ("/"); //从后面查找"/"位置
            markedImageFileName = path.right(path.length()-first-1);//获取到新的文件名
            path = path.left(first);//获得上一级路径
            if(first < 0) {path = "";}
            curSaveImageDirPath = path;
            ui->SaveImageListView->setRootIndex(unMarkedImageModel->setRootPath(path));
            ui->SaveImagePathLineEdit->setText(path);
        }
    }else{//文件被保存过时，直接按路径保存
        unmarkedMat = ui->CurrentImageGraphicsView->saveCurrentMat(unMarkedImageDirPath);//获取到保存的Mat
    }
}

/**
 * @brief MainWindow::on_Undo_triggered
 * 撤销
 */
void MainWindow::on_Undo_triggered()
{
    ui->CurrentImageGraphicsView->undo();
}

/**
 * @brief MainWindow::on_Redo_triggered
 * 恢复
 */
void MainWindow::on_Redo_triggered()
{
    ui->CurrentImageGraphicsView->redo();
}

/**
 * @brief MainWindow::watermarkRegion
 * @return
 * 计算水印区域矩形的坐标和高宽
 */
Rect MainWindow::watermarkRegion()
{
    Mat maskUnion = ui->CurrentImageGraphicsView->maskUnion;
    int x1 = 999999, y1 = 999999, x2 = 0, y2 = 0;
    int rows = maskUnion.rows;
    int cols = maskUnion.cols;
    for (int i = 0; i < rows; i++)
    {
        Vec4b *p = maskUnion.ptr<Vec4b>(i);
        for (int j = 0; j < cols; j++)
        {
            Vec4b &pix = *p++;
            uchar a = pix[3];
            if(a == 255){
                if(x1 > j) {x1 = j;}
                if(y1 > i) {y1 = i;}
                if(x2 < j) {x2 = j;}
                if(y2 < i) {y2 = i;}
            }
        }
    }
    return Rect(x1,y1,x2-x1,y2-y1);
}

/**
 * @brief MainWindow::updateWatermarkRegionLabel
 * @param x
 * @param y
 * @param w
 * @param h
 * 更新水印区域的坐标和高宽
 */
void MainWindow::updateWatermarkRegionLabel(Rect region)
{
    ui->XLineEdit->setText(QString::number(region.x));
    ui->YLineEdit->setText(QString::number(region.y));
    ui->WLineEdit->setText(QString::number(region.width));
    ui->HLineEdit->setText(QString::number(region.height));
}

/**
 * @brief MainWindow::on_getMaskAction_triggered
 * 计算提取水印
 */
void MainWindow::on_getMaskAction_triggered()
{
    if(calculateImg.size() == 0) {
        QMessageBox::warning(this,"提示","图片数量不足！");
        return;
    }
    Rect region = this->watermarkRegion(); // 获取水印区域
    ui->CurrentImageGraphicsView->setWatermark(region); // 保存水印区域
    X = region.x;
    Y = region.y;
    WIDTH = region.width;
    HEIGHT = region.height;
    updateWatermarkRegionLabel(region); // 更新UI

    if(WIDTH ==0 && HEIGHT ==0) {
        QMessageBox::warning(this,"提示","未指定水印区域！");
        return;
    }
    opencvtool.getMaskAndOpacity(calculateImg,maskMat,opacityMat,X,Y,WIDTH,HEIGHT);
    maskPixmap = opencvtool.MatToPixmap(maskMat);
    maskScene = new QGraphicsScene;
    maskItem = new QGraphicsPixmapItem(maskPixmap);
    maskScene->addItem(maskItem);
    ui->MaskImageGraphicsView->setScene(maskScene);
    ui->MaskImageGraphicsView->show();
    ui->getResultAction->setEnabled(true);//提取水印后，设置模拟去除按钮为可用
}

/**
 * @brief MainWindow::on_getResultAction_triggered
 * 模拟去除结果
 */
void MainWindow::on_getResultAction_triggered()
{
    //模拟去除结果的时候，应该另开一块内存来操作，否则去除结果会影响原来的Mat,造成结果异常（全黑、或全白）
    Mat marked = markedMat.clone();
    Mat mask = maskMat.clone();
    Mat opacity = opacityMat.clone();

    opencvtool.getResultMat(marked,resultMat, mask,opacity,X,Y,WIDTH,HEIGHT);
    opencvtool.overlay(unmarkedMat,resultMat,watermarkRegion()); // 将去水印结果叠加到原图上
    currentPixmap = opencvtool.MatToPixmap(unmarkedMat);
    currentPixmapItem->setPixmap(currentPixmap);
    currentPixmapItem->update(); // 将叠加后的图更新到currentPixmapItem
    resultPixmap = opencvtool.MatToPixmap(resultMat);
    resultScene = new QGraphicsScene;
    resultItem = new QGraphicsPixmapItem(resultPixmap);
    resultScene->addItem(resultItem);
    ui->ResultImagegraphicsView->setScene(resultScene);
    ui->ResultImagegraphicsView->show();
}
