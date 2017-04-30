#include "Client.h"
#include <winsock2.h>
#include <cstring>

SOCKET sockClient;

void Client::Connect()
{
	WSADATA wsaData;
	SOCKADDR_IN addrServer;//服务端地址
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//新建客户端socket
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	//定义要连接的服务端地址
	addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.2"); //目标IP(127.0.0.2是回送地址)
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(6000);//连接端口6000
	//连接到服务端
	connect(sockClient, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
}

//发送数据
void Client::sendMessage(point sequence[][MAXFRAME], int count[])
{
	string sendStr;
	for (int i = 0; i < HAND_COUNT; i++)           //先左手0，后右手1
	{
		for (int j = 0; j < count[i]; j++)
		{
			for (int k = 0; k < POINT_NUM_EACH_FRAME; k++)
				switch (k)
			{
				case 0: sendStr += to_string(sequence[i][j].x); sendStr += ','; break;
				case 1: sendStr += to_string(sequence[i][j].y); sendStr += ','; break;
				case 2: sendStr += to_string(sequence[i][j].z); break;
				default: break;
			}
			sendStr += '@';
		}
		if (i < (HAND_COUNT - 1))
			sendStr += '#';
	}


	const char * message = sendStr.c_str();           //将string类型转换成char*
	send(sockClient, message, strlen(message) + 1, 0); //发送消息
}

//接收数据
string Client::recvMessage()
{
	string result = "你好";
	char receiveMessage[50] = {};
	recv(sockClient, receiveMessage, 50, 0);
	result = receiveMessage;
	return result;
}

//关闭socket
void Client::Close()
{
	closesocket(sockClient);
	WSACleanup();
}


