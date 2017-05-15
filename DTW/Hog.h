#ifndef HOG_H
#define HOG_H

#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

vector<vector<float>> Hog(vector<Mat> &handMatSeq);

#endif // !HOG_H
