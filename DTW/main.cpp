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

	AcquireKinectData(kinectDataPointSeq, kinectFrameCount);    //通过Kinect采集数据存入数组中
	
	Client* data_client = new Client();
	data_client->Connect();        //连接到服务端
	data_client->sendMessage(kinectDataPointSeq, kinectFrameCount);   //将Kinect采集到的数据传入服务端
	
	string result = UTF8_To_GBK(data_client->recvMessage());           //接收识别结果
	cout << result << endl;                //输出结果
	ResultSpeech(result);
	
	data_client->Close();         //关闭连接


	
	//string temp = "_9.txt";

    //StoreData(kinectDataPointSeq[LEFT_HAND_FLAG], kinectFrameCount[LEFT_HAND_FLAG], LEFT_HAND_FLAG, temp);     //第一次将采到数据存入模板文件当中
	//StoreData(kinectDataPointSeq[RIGHT_HAND_FLAG], kinectFrameCount[RIGHT_HAND_FLAG], RIGHT_HAND_FLAG, temp);     //第一次将采到数据存入模板文件当中

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