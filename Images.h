#ifndef IMAGES_H
#define IMAGES_H

#include <opencv2/opencv.hpp>
#include <map>
using namespace cv;
using namespace std;

class Images
{
public:
    Images();
    Images(string& oriPath, string& currentPath, Mat& oriM, Mat& curM);

    string markedImgPath;//原始图片路径
    string unmarkedImgPath;//修改图片保存路径
    Mat markedMat;//原始图片
    Mat unMarkedMat;//修改后图片

    static map<string,int> markedPathMap;
    static map<string,int> unMarkedPathMap;
};

#endif // IMAGES_H
