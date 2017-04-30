#include<iostream>
#include<string>
#include"DataDefine.h"
#include"AcquireKinectData.h"
#include"Speech.h"
#include"Client.h"
using namespace std;
int main()
{
	string UTF8_To_GBK(const string &source);

	point kinectDataPointSeq[HAND_COUNT][MAXFRAME] = {};
	int kinectFrameCount[HAND_COUNT];

	AcquireKinectData(kinectDataPointSeq, kinectFrameCount);    //ͨ��Kinect�ɼ����ݴ���������
	
	Client* data_client = new Client();
	data_client->Connect();        //���ӵ������
	data_client->sendMessage(kinectDataPointSeq, kinectFrameCount);   //��Kinect�ɼ��������ݴ�������
	
	string result = UTF8_To_GBK(data_client->recvMessage());           //����ʶ����
	cout << result << endl;                //������
	ResultSpeech(result);
	
	data_client->Close();         //�ر�����


	
	//string temp = "_9.txt";

    //StoreData(kinectDataPointSeq[LEFT_HAND_FLAG], kinectFrameCount[LEFT_HAND_FLAG], LEFT_HAND_FLAG, temp);     //��һ�ν��ɵ����ݴ���ģ���ļ�����
	//StoreData(kinectDataPointSeq[RIGHT_HAND_FLAG], kinectFrameCount[RIGHT_HAND_FLAG], RIGHT_HAND_FLAG, temp);     //��һ�ν��ɵ����ݴ���ģ���ļ�����

	return 0;
}

string UTF8_To_GBK(const string &source)
{
	enum { GB2312 = 936 };

	unsigned long len = ::MultiByteToWideChar(CP_UTF8, NULL, source.c_str(), -1, NULL, NULL);
	if (len == 0)
		return std::string();
	wchar_t *wide_char_buffer = new wchar_t[len];
	::MultiByteToWideChar(CP_UTF8, NULL, source.c_str(), -1, wide_char_buffer, len);

	len = ::WideCharToMultiByte(GB2312, NULL, wide_char_buffer, -1, NULL, NULL, NULL, NULL);
	if (len == 0)
	{
		delete[] wide_char_buffer;
		return std::string();
	}
	char *multi_byte_buffer = new char[len];
	::WideCharToMultiByte(GB2312, NULL, wide_char_buffer, -1, multi_byte_buffer, len, NULL, NULL);

	std::string dest(multi_byte_buffer);
	delete[] wide_char_buffer;
	delete[] multi_byte_buffer;
	return dest;
}