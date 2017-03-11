#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QString>

class MyGraphicsView : public QGraphicsView
{
     Q_OBJECT

public:
    enum ActionName{Pencil, Eraser, SelectColor, Glasses, Hand, RectSelect, FreeSelect};

    MyGraphicsView(QWidget *parent);

    void setActionName(ActionName);

signals:
    void mouseMovetriggerSignal(QString location);

private slots:

protected:
    void mouseMoveEvent(QMouseEvent *event);

private:
    ActionName currentActionName;//记录当前选中的工具
};

#endif // MYGRAPHICSVIEW_H
