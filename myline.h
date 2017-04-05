#ifndef POINT_H
#define POINT_H

#include <QPen>
#include <QPointF>


class MyLine
{
public:
    MyLine();
    MyLine(QPen p, QPointF poi);
    void setPen(QPen p);
    void setStartPoint(QPointF p);
    void setEndPoint(QPointF p);
    QPen getPen();
    QPointF getStartPoint();
    QPointF getEndPoint();

private:
    QPen pen;
    QPointF startPoint;
    QPointF endPoint;
};

#endif // POINT_H
