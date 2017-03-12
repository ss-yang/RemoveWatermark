#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QFileSystemModel>
#include <mygraphicsview.h>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_Open_triggered();

    void on_LoadImageListView_doubleClicked(const QModelIndex &index);

    void on_SaveImageListView_doubleClicked(const QModelIndex &index);

    void on_LoadImagePathLineEdit_returnPressed();

    void on_SaveImagePathLineEdit_returnPressed();

    void on_LoadImagePathButton_clicked();

    void on_SaveImagePathButton_clicked();

    void updatePixelLocationLabel(QString location);//当加载图片后，在状态栏显示鼠标所指向的图片的像素位置

    void on_pencil_triggered();

    void on_Hand_triggered();

    void on_Eraser_triggered();

    void on_Glasses_triggered();

    void on_SelectColor_triggered();

    void on_RectSelect_triggered();

    void on_FreeSelect_triggered();

    void OriHor2CurHorScrollBar(int);//当点击原始视图水平滚动条移动时，同步操作视图水平滚动条的位置
    void OriVer2CurVerScrollBar(int);//当点击原始视图垂直滚动条移动时，同步操作视图垂直滚动条的位置
    void CurHor2OriHorScrollBar(int);//当点击操作视图水平滚动条移动时，同步原始视图水平滚动条的位置
    void CurVer2OriVerScrollBar(int);//当点击操作视图垂直滚动条移动时，同步原始视图垂直滚动条的位置

private:
    Ui::MainWindow *ui;
    QFileSystemModel *markedImageModel;
    QFileSystemModel *unMarkedImageModel;
    QPixmap oriPixmap;
    QPixmap currentPixmap;
    QGraphicsScene *oriScence;
    QGraphicsPixmapItem *oriPixmapItem;
    QGraphicsScene *currentScence;
    QGraphicsPixmapItem *currentPixmapItem;
};

#endif // MAINWINDOW_H
