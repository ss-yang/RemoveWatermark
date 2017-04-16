#ifndef OPENCVTOOL_H
#define OPENCVTOOL_H

#include <opencv2/opencv.hpp>

#include <QPixmap>
#include <QPoint>

using namespace cv;

class OpenCVTool
{
public:
    OpenCVTool();

    QPixmap MatToPixmap(Mat& src);//Mat转为QPixamp

    Mat PixmapToMat(QPixmap& src);//QPixamp转Mat

    void drawLine(Mat& img, QPoint pt1, QPoint pt2, const Scalar& color, int thickness=1);//画线函数

    void drawRect(Mat& img, QPoint pt1, QPoint pt2, const Scalar& color, int thickness=1);//画矩形函数

    Mat selectRoi(Mat& img, QPoint pt1, QPoint pt2);
};

#endif // OPENCVTOOL_H
