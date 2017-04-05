#include "myline.h"

MyLine::MyLine()
{

}

MyLine::MyLine(QPen p, QPointF poi)
{
    pen = p;
    startPoint = poi;
    endPoint = poi;
}

void MyLine::setPen(QPen p){
    pen = p;
}

void MyLine::setStartPoint(QPointF p){
    startPoint = p;
}

void MyLine::setEndPoint(QPointF p){
    endPoint = p;
}

QPen MyLine::getPen(){
    return pen;
}

QPointF MyLine::getStartPoint(){
    return startPoint;
}

QPointF MyLine::getEndPoint(){
    return endPoint;
}

