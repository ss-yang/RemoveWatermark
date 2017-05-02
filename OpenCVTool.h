#ifndef OPENCVTOOL_H
#define OPENCVTOOL_H

#include <opencv2/opencv.hpp>

#include <QPixmap>
#include <QPoint>
#include <QImage>
#include <QRGB>
#include <QDebug>

using namespace std;
using namespace cv;

class OpenCVTool
{
public:
    OpenCVTool();

    QPixmap MatToPixmap(Mat& src);//Mat转为QPixamp

    Mat PixmapToMat(QPixmap& src);//QPixamp转Mat

    void drawLine(Mat& img, QPoint pt1, QPoint pt2, const Scalar& color, int thickness=1);//画线函数

    Mat selectRectRoi(Mat &img, QPoint pt1, QPoint pt2);//获取矩形选择区域

    Mat selectFreeRoi(Mat &img, Mat &binaryMat, Rect tempRect);//获取自由选择的区域

    Mat mask2CurrentMat(Mat& mask, Mat& currentMat);//将图层与原图合并
};

#endif // OPENCVTOOL_H
