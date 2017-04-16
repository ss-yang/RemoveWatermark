#include "opencvtool.h"

#include <QImage>
#include <QDebug>

OpenCVTool::OpenCVTool()
{

}

/**
 * @brief OpenCVTool::MatToPixmap
 * @param src
 * @return
 * Mat转为QPixamp
 */
QPixmap OpenCVTool::MatToPixmap(Mat& src)
{
    Mat img;
    cvtColor(src,img,CV_BGR2RGB);//三通道图片需bgr翻转成rgb
    QImage image =  QImage((const uchar *)img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
    return QPixmap::fromImage(image);
}

//QImage的构造函数进行转换，主要是使用cv::Mat的data来构造一个QImage类型，这样做确实可以达到转换目的
//但是，因此这样构造出来的QImage本身并不保存data，因此，在QImage的生存周期内，必须保证cv::Mat中的数据不会被释放。
//上面的这个问题也是比较容易解决的，主要是通过调用QImage::bits函数来强制QImage进行深层次复制，使得QImage自己保存一份data的副本，
//这样就可以保证在cv::Mat中的数据被释放的时候，QImage还能正常使用。

/**
 * @brief OpenCVTool::PixmapToMat
 * @param src
 * @return
 * QPixamp转Mat
 */
Mat OpenCVTool::PixmapToMat(QPixmap& src)
{
    QImage image = src.toImage();
    //uchar* bits();获取图像的首地址 bytesPerLine(); 图像每行字节数
    Mat mat = Mat(image.height(), image.width(), CV_8UC3, (uchar*)image.bits(), image.bytesPerLine());
    cvtColor(mat, mat, CV_RGB2BGR);
    return mat;
}

/**
 * @brief OpenCVTool::drawLine
 * @param img
 * @param pt1
 * @param pt2
 * @param color
 * @param thickness
 * @param lineType
 * @param shift
 * 画线函数
 */
void OpenCVTool::drawLine(Mat& img, QPoint pt1, QPoint pt2, const Scalar &color, int thickness)
{
    Point startPoint = Point(pt1.x(), pt1.y());
    Point ednPoint = Point(pt2.x(), pt2.y());
    line(img, startPoint, ednPoint, color, thickness);
}


/**
 * @brief OpenCVTool::selectRoi
 * @param img
 * @param pt1
 * @param pt2
 * 获取感兴趣区域
 */
Mat OpenCVTool::selectRoi(Mat &img, QPoint pt1, QPoint pt2)
{
    Point startPoint = Point(pt1.x(), pt1.y());
    Point endPoint = Point(pt2.x(), pt2.y());
    Mat roi = img(Rect(startPoint.x, startPoint.y, endPoint.x-startPoint.x, endPoint.y-startPoint.y));
    return roi;
}
