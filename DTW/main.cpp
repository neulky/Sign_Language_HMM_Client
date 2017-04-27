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

	AcquireKinectData(kinectDataPointSeq, kinectFrameCount);    //ͨ��Kinect�ɼ����ݴ���������
	
	Client* data_client = new Client();
	data_client->Connect();        //���ӵ������
	data_client->sendMessage(kinectDataPointSeq[RIGHT_HAND_FLAG], kinectFrameCount[RIGHT_HAND_FLAG]);   //��Kinect�ɼ��������ݴ�������
	string result = data_client->recvMessage();           //����ʶ����
	cout << result << endl;                //������
	data_client->Close();         //�ر�����

	ResultSpeech(result);
	//string temp = "_9.txt";

    //StoreData(kinectDataPointSeq[LEFT_HAND_FLAG], kinectFrameCount[LEFT_HAND_FLAG], LEFT_HAND_FLAG, temp);     //��һ�ν��ɵ����ݴ���ģ���ļ�����
	//StoreData(kinectDataPointSeq[RIGHT_HAND_FLAG], kinectFrameCount[RIGHT_HAND_FLAG], RIGHT_HAND_FLAG, temp);     //��һ�ν��ɵ����ݴ���ģ���ļ�����

	return 0;
}