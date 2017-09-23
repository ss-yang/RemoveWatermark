#include "Dialogsettings.h"
#include "ui_dialogsettings.h"

#include <QFileDialog>
#include <QDebug>

DialogSettings::DialogSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSettings)
{
    ui->setupUi(this);
    connect(ui->spinBox_pensize,SIGNAL(valueChanged(int)),this,SLOT(setPenSize(int)));//信号连接，改变penSize的值
    connect(this,SIGNAL(pathChanged()),this,SLOT(setPath()));

    initLayout();
    qDebug()<<"pensize: "<<penSize;
}

DialogSettings::~DialogSettings()
{
    delete ui;
    delete lineEditUnmarkedSavePath;
    delete lineEditOutputPath;
    delete btnUnmarkedSavePath;
    delete btnOutputPath;
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
 * slot: 设置penSize的值
 */
void DialogSettings::setPenSize(int value)
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

/**
 * @brief DialogSettings::initValue
 * @param penSize
 * @param unmarkedSavePath
 * @param outputPath
 * 创建DialogSettings窗口时向其传值
 */
void DialogSettings::initValue(int penSize, QString unmarkedSavePath, QString outputPath)
{
    this->penSize = penSize;
    this->unmarkedSavePath = unmarkedSavePath;
    this->outputPath = outputPath;
}

void DialogSettings::initLayout(){
    sbPenSize = new QSpinBox;
    lineEditUnmarkedSavePath = new QLineEdit;
    lineEditOutputPath = new QLineEdit;
    btnUnmarkedSavePath = new QPushButton;
    btnOutputPath = new QPushButton;
}
