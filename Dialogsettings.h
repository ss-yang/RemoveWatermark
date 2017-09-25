#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QDialog>
#include <QSpinBox>

namespace Ui {
class DialogSettings;
}

class DialogSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSettings(QWidget *parent = 0);
    ~DialogSettings();

    int getPenSize();
    QString getUnmarkedSavePath();
    QString getOutputPath();

    void setPenSize(int value);
    void setUnmarkedSavePath(QString path);
    void setOutputPath(QString path);

signals:
    void penSizeChanged(int value);
    void pathChanged();

private slots:
    void setThisPenSize(int value);
    void setPath();

    void on_btn_unmarkedSavePath_clicked();

    void on_btn_outputPath_clicked();

private:
    /**ui元素**/
    Ui::DialogSettings *ui;
    QSpinBox *sbPenSize;
    QLineEdit *lineEditUnmarkedSavePath;
    QLineEdit *lineEditOutputPath;
    QPushButton *btnUnmarkedSavePath;
    QPushButton *btnOutputPath;

    /**需要保存的设置项的值**/
    int penSize;//默认画笔大小
    QString unmarkedSavePath;//通过文件选择器取到的默认路径
    QString outputPath;//通过文件选择器取到的默认路径

    void initLayout();
};

#endif // DIALOGSETTINGS_H
