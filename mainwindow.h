#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QFileSystemModel>
#include <mygraphicsview.h>
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

    void updatePixelLocationLabel(QString location);

    void on_pencil_triggered();

    void on_Hand_triggered();

    void on_Eraser_triggered();

    void on_Glasses_triggered();

    void on_SelectColor_triggered();

    void on_RectSelect_triggered();

    void on_FreeSelect_triggered();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *markedImageModel;
    QFileSystemModel *unMarkedImageModel;
    QPixmap oriPixmap;
    QPixmap currentPixmap;
};

#endif // MAINWINDOW_H
