#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <opencv2/opencv.hpp>
#include <ctime>

#include <QDebug>
#include <QString>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QPoint>
#include <QPointF>
#include <QFileSystemModel>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QScrollBar>
#include <QPalette>
#include <QColor>
#include <QColorDialog>

using namespace std;
using namespace cv;

//图像
vector<Mat> markedImg;
vector<Mat> unmarkedImg;
//图片路径
vector<string> markedImgPath;
vector<string> unmarkedImgPath;
//水印位置
int X, Y, WIDTH, HEIGHT;
//图片目录
QString markedImageDirPath;
QString unMarkedImageDirPath;
//当前图片列表目录
QString curLoadImageDirPath = "";
QString curSaveImageDirPath = "";

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
    filter<<"*.jpg"<<"*.png"<<"*.bmp"<<"*.jpeg"<<"*.gif"<<"*.JPG"<<"*.PNG"<<"*.BMP"<<"*.JPEG"<<"*.GIF";

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

    /**
     * 初始化粗细设置按钮
     */
    thicknessSlider = new MySlider(this);
    thicknessAction = ui->extraToolBar->addWidget(thicknessSlider);

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
    QObject::connect(thicknessSlider,SIGNAL(valueChanged(int)),ui->CurrentImageGraphicsView,SLOT(setPencilWidth(int)));
    QObject::connect(thicknessSlider,SIGNAL(valueChanged(int)),ui->CurrentImageGraphicsView,SLOT(setEraserWidth(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete markedImageModel;
    delete unMarkedImageModel;
    delete oriScence;
    delete oriPixmapItem;
    delete currentScence;
    delete currentPixmapItem;
    delete thicknessSlider;
    delete thicknessAction;
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
    QString path = markedImageModel->fileInfo(index).absoluteFilePath();
    if(markedImageModel->fileInfo(index).isDir()) {//是目录则进入目录
        curLoadImageDirPath = path;
        ui->LoadImageListView->setRootIndex(markedImageModel->setRootPath(path));
        ui->LoadImagePathLineEdit->setText(path);
    }else if(!oriPixmap.load(path) || !currentPixmap.load(path)){//是图片则读取图片显示
        QMessageBox::warning(this,"提示","打开图像失败！");
    }else {
        QString width,height;
        //更新状态栏图片大小信息
        ui->ImageSizeLabel->setText(width.setNum(oriPixmap.width()) + " × " + height.setNum(oriPixmap.height()) + " 像素");
        //视图栏显示图片
        oriScence = new QGraphicsScene;
        oriPixmapItem = new QGraphicsPixmapItem(oriPixmap);
        oriScence->addItem(oriPixmapItem);
        ui->OriImageGraphicsView->setScene(oriScence);
        ui->OriImageGraphicsView->show();
        currentScence = new QGraphicsScene;
        currentPixmapItem = new QGraphicsPixmapItem(currentPixmap);
        currentScence->addItem(currentPixmapItem);
        ui->CurrentImageGraphicsView->setScene(currentScence);
        ui->CurrentImageGraphicsView->show();
    }
}

/**
 * @brief MainWindow::on_SaveImageListView_doubleClicked
 * @param index
 * 保存图片列表双击打开目录=
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
 * @brief updatePixelLocationLabel
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
    ui->OriImageGraphicsView->setActionName(MyGraphicsView::Forbidden);
    ui->CurrentImageGraphicsView->setActionName(MyGraphicsView::Pencil);

}

/**
 * @brief MainWindow::on_Eraser_triggered
 * 橡皮工具
 */
void MainWindow::on_Eraser_triggered()
{
    resetAction();
    ui->Eraser->setChecked(true);
    ui->OriImageGraphicsView->setActionName(MyGraphicsView::Forbidden);
    ui->CurrentImageGraphicsView->setActionName(MyGraphicsView::Eraser);
}

/**
 * @brief MainWindow::on_Glasses_triggered
 * 放大镜工具
 */
void MainWindow::on_Glasses_triggered()
{
    resetAction();
    ui->Glasses->setChecked(true);
    ui->OriImageGraphicsView->setActionName(MyGraphicsView::BigGlasses);
    ui->CurrentImageGraphicsView->setActionName(MyGraphicsView::BigGlasses);
}

/**
 * @brief MainWindow::on_Hand_triggered
 * 抓手工具
 */
void MainWindow::on_Hand_triggered()
{
    resetAction();
    ui->Hand->setChecked(true);
    ui->OriImageGraphicsView->setActionName(MyGraphicsView::OpenHand);
    ui->CurrentImageGraphicsView->setActionName(MyGraphicsView::OpenHand);
}

/**
 * @brief MainWindow::on_RectSelect_triggered
 * 矩形选择工具
 */
void MainWindow::on_RectSelect_triggered()
{
    resetAction();
    ui->RectSelect->setChecked(true);
    ui->OriImageGraphicsView->setActionName(MyGraphicsView::RectSelect);
    ui->CurrentImageGraphicsView->setActionName(MyGraphicsView::RectSelect);
}

/**
 * @brief MainWindow::on_FreeSelect_triggered
 * 自由选择工具
 */
void MainWindow::on_FreeSelect_triggered()
{
    resetAction();
    ui->FreeSelect->setChecked(true);
    ui->OriImageGraphicsView->setActionName(MyGraphicsView::FreeSelect);
    ui->CurrentImageGraphicsView->setActionName(MyGraphicsView::FreeSelect);
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

