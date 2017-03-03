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
#include <QFileSystemModel>

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
QString markedImageDirPath = "C:/";
QString unMarkedImageDirPath = "C:/";

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

    ui->LoadImageListView->setModel(markedImageModel);
    ui->LoadImageListView->setRootIndex(markedImageModel->setRootPath(markedImageDirPath));
    ui->LoadImagePathLineEdit->setText(markedImageDirPath);

    unMarkedImageModel = new QFileSystemModel(this);
    unMarkedImageModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);

    ui->SaveImageListView->setModel(unMarkedImageModel);
    ui->SaveImageListView->setRootIndex(unMarkedImageModel->setRootPath(unMarkedImageDirPath));
    ui->SaveImagePathLineEdit->setText(unMarkedImageDirPath);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::on_Open_triggered
 * 打开图片目录
 */
void MainWindow::on_Open_triggered()
{
    markedImageDirPath = QFileDialog::getExistingDirectory(this,"请选择水印图像所在文件夹",QDir::currentPath());
    //当用户没有点击取消按钮时弹出
    if(!markedImageDirPath.isEmpty()) {
        ui->LoadImageListView->setRootIndex(markedImageModel->setRootPath(markedImageDirPath));
        ui->LoadImagePathLineEdit->setText(markedImageDirPath);
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
    if(markedImageModel->fileInfo(index).isDir()) {
        ui->LoadImageListView->setRootIndex(markedImageModel->setRootPath(path));
        ui->LoadImagePathLineEdit->setText(path);
    }
}

/**
 * @brief MainWindow::on_SaveImageListView_doubleClicked
 * @param index
 * 原始图片列表双击打开目录
 */
void MainWindow::on_SaveImageListView_doubleClicked(const QModelIndex &index)
{
    QString path = unMarkedImageModel->fileInfo(index).absoluteFilePath();
    if(unMarkedImageModel->fileInfo(index).isDir()) {
        ui->SaveImageListView->setRootIndex(unMarkedImageModel->setRootPath(path));
        ui->SaveImagePathLineEdit->setText(path);
    }
}
