#pragma once
#ifndef CLIENT_H
#define CLIENT_H


#include<stdio.h>
#pragma comment(lib,"ws2_32.lib")
#include"DataDefine.h"
#include<vector>


class Client
{

public:
	void Connect();
	void sendMessage(point sequence[][MAXFRAME], int count[]);
	void sendMessage(vector<vector<float>> rightHandDescriptorSeq);
	string recvMessage();
	void Close();
};


#endif // !CLIENT_H
