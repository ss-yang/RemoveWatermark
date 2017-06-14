#ifndef IMAGEPAIR_H
#define IMAGEPAIR_H

#include <opencv2/opencv.hpp>
#include <map>
using namespace cv;
using namespace std;

class ImagePair
{
public:
    ImagePair();
    ImagePair(string oriPath, string currentPath, Mat oriM, Mat curM);

    string markedImgPath;//原始图片路径
    string unmarkedImgPath;//修改图片保存路径
    Mat markedMat;//原始图片
    Mat unMarkedMat;//修改后图片
};

#endif // IMAGEPAIR_H
