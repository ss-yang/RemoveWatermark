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
 * @brief OpenCVTool::selectFreeRoi
 * @param img
 * @param binaryMat
 * @param movePoints
 * @return
 * 获取自由选择的区域
 */
Mat OpenCVTool::selectFreeRoi(Mat &img, Mat &binaryMat, Rect tempRect)
{
    Mat tempImg = img(tempRect);//获取到选择区域处的图片
    Mat tempBinaryMat = binaryMat(tempRect);//获取到二值图像上选择到的区域
    vector<vector<Point>> contours;
    findContours(tempBinaryMat, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);//获取二值图像上的轮廓信息
    Mat alphaMat = Mat ::zeros(tempBinaryMat.size(), CV_8UC1);
    for(int i = 0; i < (int)contours.size(); i++) {
        drawContours(alphaMat, contours, i, Scalar(255), CV_FILLED);//轮廓内部填充
    }
    Mat tempResult = Mat(tempImg.size(), CV_8UC3, Scalar::all(0));
    tempImg.copyTo(tempResult,alphaMat);
    Mat result = Mat(tempImg.size(), CV_8UC4, Scalar::all(0));
    Mat in[] = {tempResult, alphaMat};
    int fromTo1[] = {0,0, 1,1, 2,2, 3,3};
    mixChannels(in, 2, &result, 1, fromTo1, 4);
    return result;
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

/**
 * @brief OpenCVTool::drawDashLine
 * @param img
 * @param pt1
 * @param pt2
 * @return
 * 绘制虚线矩形
 */
void OpenCVTool::drawDashLineRect(Mat &img, QPoint pt1, QPoint pt2)
{
    int linelength = 4;//实线长度
    int dashlength = 4;//虚线长度
    int thickness = 1;//线粗
    Scalar lineColor = Scalar(0,0,0,255);//实线黑色
    Scalar dashColor = Scalar(255,255,255,255);//虚线白色
    bool isLine = true;//当前要画的线是否为实线
    //获得矩形的起点与终点
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
    Point startPoint1,endPoint1,startPoint2,endPoint2;
    int startX = startPoint.x; int endX = startX;
    //绘制横轴的两条虚线
    while(endX < endPoint.x) {
        //如果要画实线加实线长度否则加虚线长度
        if(isLine) {endX += linelength;}else{endX += dashlength;}
        //如果超出范围，则等于最大值
        if(endX > endPoint.x) {endX = endPoint.x;}
        startPoint1 = Point(startX, startPoint.y);
        endPoint1   = Point(endX, startPoint.y);
        startPoint2 = Point(startX, endPoint.y);
        endPoint2   = Point(endX, endPoint.y);
        startX = endX;
        if(isLine) {
            line(img, startPoint1, endPoint1, lineColor, thickness);//绘制上边的实线
            line(img, startPoint2, endPoint2, lineColor, thickness);//绘制下边的实线
            isLine = false;
        }else{
            line(img, startPoint1, endPoint1, dashColor, thickness);//绘制上边的虚线
            line(img, startPoint2, endPoint2, dashColor, thickness);//绘制下边的虚线
            isLine = true;
        }
    }
    isLine = true;
    int startY = startPoint.y; int endY = startY;
    //绘制横轴的两条虚线
    while(endY < endPoint.y) {
        //如果要画实线加实线长度否则加虚线长度
        if(isLine) {endY += linelength;}else{endY += dashlength;}
        //如果超出范围，则等于最大值
        if(endY > endPoint.y) {endY = endPoint.y;}
        startPoint1 = Point(startPoint.x, startY);
        endPoint1   = Point(startPoint.x, endY);
        startPoint2 = Point(endPoint.x, startY);
        endPoint2   = Point(endPoint.x, endY);
        startY = endY;
        if(isLine) {
            line(img, startPoint1, endPoint1, lineColor, thickness);//绘制左边的实线
            line(img, startPoint2, endPoint2, lineColor, thickness);//绘制右边的实线
            isLine = false;
        }else{
            line(img, startPoint1, endPoint1, dashColor, thickness);//绘制左边的虚线
            line(img, startPoint2, endPoint2, dashColor, thickness);//绘制右边的虚线
            isLine = true;
        }
    }
}

/**
 * @brief OpenCVTool::getMaskAndOpacity
 * @param calculateImg
 * @param maskMat
 * @param opacityMat
 * @param X
 * @param Y
 * @param WIDTH
 * @param HEIGHT
 * 计算水印以及混合比
 */
void OpenCVTool::getMaskAndOpacity(vector<ImagePair> calculateImg, Mat &maskMat, Mat &opacityMat, int X, int Y, int WIDTH, int HEIGHT)
{
    maskMat = Mat(HEIGHT, WIDTH, CV_32FC3);
    opacityMat = Mat(HEIGHT, WIDTH, CV_32FC3);
    Rect rect = Rect(X, Y, WIDTH, HEIGHT);
    Mat Ex = Mat(HEIGHT, WIDTH, CV_32FC3, Scalar::all(0));
    Mat Ey = Mat(HEIGHT, WIDTH, CV_32FC3, Scalar::all(0));
    Mat Exy = Mat(HEIGHT, WIDTH, CV_32FC3, Scalar::all(0));
    Mat Ex2 = Mat(HEIGHT, WIDTH, CV_32FC3, Scalar::all(0));
    Mat one = Mat(HEIGHT, WIDTH, CV_32FC3, Scalar::all(1));
    int n = calculateImg.size();
//    Ex = 0; Ey = 0; Exy = 0; Ex2 = 0;

    for(int i = 0; i < n; i++) {
        ImagePair img = calculateImg[i];
        Mat marked = img.markedMat.clone();
        Mat temp1(marked, rect);
        temp1.convertTo(temp1,CV_32FC3);
        Mat unmarked = img.unMarkedMat.clone();
        Mat temp2(unmarked, rect);
        temp2.convertTo(temp2,CV_32FC3);

        Ex = Ex + temp2;
        Ey = Ey + temp1;
        Exy = Exy + temp2.mul(temp1);
        Ex2 = Ex2 + temp2.mul(temp2);
    }
    opacityMat = (Exy - Ex.mul(Ey) / n) / (Ex2 - Ex.mul(Ex) / n);
    //单个整数无法与矩阵进行加减操作，需要将整数转为矩阵
    maskMat = (Ey - opacityMat.mul(Ex)) / (one - opacityMat) / n;
    opacityMat = opacityMat.mul(255);
    opacityMat.convertTo(opacityMat, CV_8UC3);
    maskMat.convertTo(maskMat,CV_8UC3);
}

/**
 * @brief OpenCVTool::getResultMat
 * @param markedMat
 * @param resultMat
 * @param maskMat
 * @param opacityMat
 * @param X
 * @param Y
 * @param WIDTH
 * @param HEIGHT
 * 模拟去除结果
 */
void OpenCVTool::getResultMat(Mat &markedMat, Mat &resultMat, Mat &maskMat, Mat &opacityMat, int X, int Y, int WIDTH, int HEIGHT)
{
    markedMat.convertTo(markedMat, CV_32FC3);
    Rect rect = Rect(X, Y, WIDTH, HEIGHT);
    Mat marked(markedMat, rect);
    resultMat = Mat(HEIGHT, WIDTH, CV_32FC3);
    maskMat.convertTo(maskMat, CV_32FC3);
    opacityMat.convertTo(opacityMat, CV_32FC3);
    Mat one = Mat(HEIGHT, WIDTH, CV_32FC3, Scalar::all(1));
    Mat max = Mat(HEIGHT, WIDTH, CV_32FC3, Scalar::all(255));
    opacityMat = opacityMat / max;//混合度归一化
    //计算原图
    resultMat = (marked - maskMat.mul(one - opacityMat)) / opacityMat;
    //图像格式复原
    resultMat.convertTo(resultMat,CV_8UC3);
    markedMat.convertTo(markedMat, CV_8UC3);
    maskMat.convertTo(maskMat, CV_8UC3);
    opacityMat.convertTo(opacityMat, CV_8UC3);
}

/**
 * @brief OpenCVTool::overlay
 * @param bottom
 * @param top
 * @param position
 * 将top覆盖到bottom的position区域上
 */
void OpenCVTool::overlay(Mat &bottom, Mat top, Rect position)
{
    Mat roi, maskAdd;
    cvtColor(top,maskAdd,CV_BGR2GRAY); //将要overlay的图像转为灰度图作为掩膜
    roi = bottom(position);
    top.copyTo(roi,maskAdd);
}
