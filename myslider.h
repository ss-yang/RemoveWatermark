#ifndef MYSLIDER_H
#define MYSLIDER_H

#include <QWidget>

namespace Ui {
class MySlider;
}

class MySlider : public QWidget
{
    Q_OBJECT

public:
    explicit MySlider(QWidget *parent = 0);
    ~MySlider();

signals:
    void valueChanged(int value);

private:
    Ui::MySlider *ui;
};

#endif // MYSLIDER_H
