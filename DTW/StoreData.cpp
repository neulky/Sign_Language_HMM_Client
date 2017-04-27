#include<string>
#include"DataDefine.h"
#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<iomanip>

using namespace std;

/*
*将Kinect采集到的数据存入txt文档 
*主要用于建立数据训练集
*/
int StoreData(point * new_module, int newModuleFrameNum, int hand_flag, string fileName)
{
	string sourceFile;
	ofstream outfile;
	string moduleDataStr;
	if (hand_flag == LEFT_HAND_FLAG)
		sourceFile = "G:\\GitHubKinect\\HMM_Model_3\\Right_Train_Data\\2\\" + fileName;
	else{
		if (hand_flag == RIGHT_HAND_FLAG)
			sourceFile = "G:\\GitHubKinect\\HMM_Model_3\\Right_Train_Data\\2\\" + fileName;
		else
			return 0;
	}

	outfile.open(sourceFile);

	for (int i = 0; i < newModuleFrameNum; i++)
	{
		for (int j = 0; j < POINT_NUM_EACH_FRAME; j++)
		{
			switch (j)
			{
			case 0: moduleDataStr += to_string(new_module[i].x); moduleDataStr += ','; break;
			case 1: moduleDataStr += to_string(new_module[i].y); moduleDataStr += ','; break;
			case 2: moduleDataStr += to_string(new_module[i].z); break;

			default: 
				break;
			}
		}
		moduleDataStr += '@';
	}

	//cout << moduleDataStr << endl;

	outfile << moduleDataStr;

	outfile.close();

	return 1;
}
