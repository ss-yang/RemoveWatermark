#include "Images.h"

Images::Images()
{

}

Images::Images(string &oriPath, string &currentPath, Mat &oriM, Mat &curM)
{
    markedImgPath = oriPath; unmarkedImgPath = currentPath;
    markedMat = oriM; unMarkedMat = curM;
}
