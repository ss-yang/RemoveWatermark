#ifndef THICKNESSSLIDER_H
#define THICKNESSSLIDER_H

#include <QWidget>

namespace Ui {
class ThicknessSlider;
}

class ThicknessSlider : public QWidget
{
    Q_OBJECT

public:
    explicit ThicknessSlider(QWidget *parent = 0);
    ~ThicknessSlider();

signals:
    void valueChanged(int value);

private:
    Ui::ThicknessSlider *ui;
};

#endif // THICKNESSSLIDER_H
