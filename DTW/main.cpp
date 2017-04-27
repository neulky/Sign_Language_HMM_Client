#include<iostream>
#include<string>
#include"DataDefine.h"
#include"AcquireKinectData.h"
#include"Speech.h"
#include"Client.h"
using namespace std;
int main()
{
	point kinectDataPointSeq[HAND_COUNT][MAXFRAME] = {};
	int kinectFrameCount[HAND_COUNT];

	AcquireKinectData(kinectDataPointSeq, kinectFrameCount);    //通过Kinect采集数据存入数组中
	
	Client* data_client = new Client();
	data_client->Connect();        //连接到服务端
	data_client->sendMessage(kinectDataPointSeq[RIGHT_HAND_FLAG], kinectFrameCount[RIGHT_HAND_FLAG]);   //将Kinect采集到的数据传入服务端
	string result = data_client->recvMessage();           //接收识别结果
	cout << result << endl;                //输出结果
	data_client->Close();         //关闭连接

	ResultSpeech(result);
	//string temp = "_9.txt";

    //StoreData(kinectDataPointSeq[LEFT_HAND_FLAG], kinectFrameCount[LEFT_HAND_FLAG], LEFT_HAND_FLAG, temp);     //第一次将采到数据存入模板文件当中
	//StoreData(kinectDataPointSeq[RIGHT_HAND_FLAG], kinectFrameCount[RIGHT_HAND_FLAG], RIGHT_HAND_FLAG, temp);     //第一次将采到数据存入模板文件当中

	return 0;
}