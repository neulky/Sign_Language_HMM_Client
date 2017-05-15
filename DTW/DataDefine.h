#ifndef DataDefine_H
#define DataDefine_H

#include<math.h>
#include<stdio.h>
#include<string>
#include<vector>
using namespace std;

#define DTWMAXNUM 5000
#define DTWSEQ1 5
#define DTWSEQ2 6
#define MAXFRAME 1000      //�ɼ����ݵ����֡��
#define POINT_NUM_EACH_FRAME 3 //ÿ֡��ĸ���
#define DTWVERYBIG 100000.0  //��ʾ�����
#define MATCHRANGE 3  //ƥ�䷶Χ
#define LEFT_HAND_FLAG 0
#define RIGHT_HAND_FLAG 1
#define HAND_COUNT 2
#define MAX_MODULE_NUM 100  //���ģ�͸���
#define MATCHTHRESHOLD 10.0

#define MIN(a,b) ((a)<(b)?(a):(b))
#define ABS(a) ((a)>0?(a):(-(a)))

struct point{
	float x;
	float y;
	float z;
};

int StoreData(point * new_module, int newModuleFrameNum, int hand_flag, string fileName);  //����DTWģ��������
int StoreData(vector<vector<float>> descriptorSeq, string fileName);

#endif // DataDefine_H
