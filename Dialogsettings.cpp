#include "Dialogsettings.h"
#include "ui_dialogsettings.h"

#include <QFileDialog>

DialogSettings::DialogSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSettings)
{
    ui->setupUi(this);
    connect(ui->spinBox_pensize,SIGNAL(valueChanged(int)),this,SLOT(setThisPenSize(int)));//信号连接，改变penSize的值
    connect(this,SIGNAL(pathChanged()),this,SLOT(setPath()));

    initLayout();
}

DialogSettings::~DialogSettings()
{
    delete ui;
}


int DialogSettings::getPenSize()
{
    return this->penSize;
}

QString DialogSettings::getUnmarkedSavePath()
{
    return this->unmarkedSavePath;
}

QString DialogSettings::getOutputPath()
{
    return this->outputPath;
}

/**
 * @brief DialogSettings::getPenSize
 * @return
 * 设置penSize的值，同时设置控件的值
 */
void DialogSettings::setPenSize(int value)
{
    this->penSize = value;
    ui->spinBox_pensize->setValue(value);
}

void DialogSettings::setUnmarkedSavePath(QString path)
{
    this->unmarkedSavePath = path;
    ui->lineEdit_unmarkedSavePath->setText(path);
}

void DialogSettings::setOutputPath(QString path)
{
    this->outputPath = path;
    ui->lineEdit_outputPath->setText(path);
}

/**
 * @brief DialogSettings::setThisPenSize
 * @param value
 * slot：
 * 在对话框修改spinBox的值时，会发送信号到这个槽，修改this->penSize
 */
void DialogSettings::setThisPenSize(int value)
{
    this->penSize = value;
}

/**
 * @brief DialogSettings::setPath
 * slot: 设置两个默认路径lineEdit的值
 */
void DialogSettings::setPath()
{
    this->ui->lineEdit_unmarkedSavePath->setText(this->unmarkedSavePath);
    this->ui->lineEdit_outputPath->setText(this->outputPath);
}

void DialogSettings::on_btn_unmarkedSavePath_clicked()
{
    this->unmarkedSavePath = QFileDialog::getExistingDirectory(this, tr("选择默认文件夹"),
                                                               "d:\\",
                                                               QFileDialog::ShowDirsOnly
                                                               | QFileDialog::DontResolveSymlinks);
    emit pathChanged();//发送信号，设置LineEdit控件的string
}

void DialogSettings::on_btn_outputPath_clicked()
{
    this->outputPath = QFileDialog::getExistingDirectory(this, tr("选择默认文件夹"),
                                                    "d:\\",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    emit pathChanged();
}

void DialogSettings::initLayout(){
    //提示
    ui->lb_hint->setText("提示:当前设置重启程序后生效。");
}
