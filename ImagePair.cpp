#include "ImagePair.h"

ImagePair::ImagePair()
{

}

ImagePair::ImagePair(string oriPath, string currentPath, Mat oriM, Mat curM)
{
    markedImgPath = oriPath; unmarkedImgPath = currentPath;
    markedMat = oriM; unMarkedMat = curM;
}
