#ifndef DataDefine_H
#define DataDefine_H

#include<math.h>
#include<stdio.h>
#include<string>
using namespace std;

#define DTWMAXNUM 5000
#define DTWSEQ1 5
#define DTWSEQ2 6
#define MAXFRAME 1000      //采集数据的最大帧数
#define POINT_NUM_EACH_FRAME 3 //每帧点的个数
#define DTWVERYBIG 100000.0  //表示无穷大
#define MATCHRANGE 3  //匹配范围
#define LEFT_HAND_FLAG 0
#define RIGHT_HAND_FLAG 1
#define HAND_COUNT 2
#define MAX_MODULE_NUM 100  //最大模型个数
#define MATCHTHRESHOLD 10.0

#define MIN(a,b) ((a)<(b)?(a):(b))
#define ABS(a) ((a)>0?(a):(-(a)))

struct point{
	float x;
	float y;
	float z;
};

int StoreData(point * new_module, int newModuleFrameNum, int hand_flag, string fileName);  //更新DTW模板中数据

#endif // DataDefine_H
