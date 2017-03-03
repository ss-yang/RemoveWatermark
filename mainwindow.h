#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QFileSystemModel>

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

private:
    Ui::MainWindow *ui;
    QFileSystemModel *markedImageModel;
    QFileSystemModel *unMarkedImageModel;
};

#endif // MAINWINDOW_H
