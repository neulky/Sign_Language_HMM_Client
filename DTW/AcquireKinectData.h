#ifndef ACQUIREKINECTDATA_H
#define ACQUIREKINECTDATA_H

#include"DataDefine.h"
#include<vector>
#include<opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int AcquireKinectData(point kinectDataPoint[][MAXFRAME], int* kinectFrameCount, vector<Mat> &rightMatSeq);

#endif // !ACQUIREKINECTDATA_H
