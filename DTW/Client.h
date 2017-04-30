#pragma once
#ifndef CLIENT_H
#define CLIENT_H


#include<stdio.h>
#pragma comment(lib,"ws2_32.lib")
#include"DataDefine.h"


class Client
{

public:
	void Connect();
	void sendMessage(point sequence[][MAXFRAME], int count[]);
	string recvMessage();
	void Close();
};


#endif // !CLIENT_H
