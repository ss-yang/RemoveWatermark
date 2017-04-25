#include "OpenCVTool.h"

OpenCVTool::OpenCVTool()
{

}

/**
 * @brief OpenCVTool::MatToPixmap
 * @param src
 * @return
 * Mat转为QPixamp
 */
QPixmap OpenCVTool::MatToPixmap(Mat& src) {
    if(src.type() == CV_8UC1) {
        QImage image(src.cols, src.rows, QImage::Format_Indexed8);
        image.setColorCount(256);// Set the color table (used to translate colour indexes to qRgb values)
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = src.data;
        for(int row = 0; row < src.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, src.cols);
            pSrc += src.step;
        }
        return QPixmap::fromImage(image);
    }
    else if(src.type() == CV_8UC3) {
        Mat img;
        cvtColor(src,img,CV_BGR2RGB);//三通道图片需bgr翻转成rgb
        QImage image =  QImage((const uchar *)img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
        return QPixmap::fromImage(image);
    }
    else if(src.type() == CV_8UC4) {
        Mat img = src.clone();
        QImage image =  QImage((const uchar *)img.data, img.cols, img.rows, img.step, QImage::Format_ARGB32);
        return QPixmap::fromImage(image);
    }
    return QPixmap();
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
    Mat mat;
    if(image.format() == QImage::Format_ARGB32) {
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.bits(), image.bytesPerLine());
    }
    else if(image.format() == QImage::Format_RGB888) {
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.bits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
    }
    else if(image.format() == QImage::Format_Indexed8) {
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.bits(), image.bytesPerLine());
    }
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
    Point endPoint = Point(pt2.x(), pt2.y());
    line(img, startPoint, endPoint, color, thickness);
}

/**
 * @brief OpenCVTool::selectRoi
 * @param img
 * @param pt1
 * @param pt2
 * @return
 * 获取矩形选择区域
 */
Mat OpenCVTool::selectRectRoi(Mat &img, QPoint pt1, QPoint pt2)
{
    Point startPoint = Point(pt1.x(), pt1.y());
    Point endPoint = Point(pt2.x(), pt2.y());
    int temp;
    if(startPoint.x > endPoint.x) {
        temp = startPoint.x;
        startPoint.x = endPoint.x;
        endPoint.x =temp;
    }
    if(startPoint.y > endPoint.y) {
        temp = startPoint.y;
        startPoint.y = endPoint.y;
        endPoint.y =temp;
    }
    //rectRoi指向图片中的该区域，共享同一个内存
    Mat rectRoi = img(Rect(startPoint.x, startPoint.y, endPoint.x - startPoint.x, endPoint.y - startPoint.y));
    return rectRoi;
}

/**
 * @brief OpenCVTool::mask2CurrentMat
 * @param mask
 * @param currentMat
 * @return
 * 将图层与原图合并
 */
Mat OpenCVTool::mask2CurrentMat(Mat &mask, Mat &currentMat)
{
    Mat result = Mat(currentMat.rows, currentMat.cols, CV_8UC3);
    for(int i = 0; i < currentMat.rows; i++) {
        Vec4b *p1 = mask.ptr<Vec4b>(i);
        Vec3b *p2 = currentMat.ptr<Vec3b>(i);
        Vec3b *p3 = result.ptr<Vec3b>(i);
        for(int j = 0; j < currentMat.cols; j++) {
            Vec4b &pix1 = *p1++;
            Vec3b &pix2 = *p2++;
            Vec3b &pix3 = *p3++;
            if(pix1[3] == 0) {
                pix3 = pix2;
            }else{
                pix3[0] = pix1[0];
                pix3[1] = pix1[1];
                pix3[2] = pix1[2];
            }
        }
    }
    return result;
}
