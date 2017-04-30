#include "Client.h"
#include <winsock2.h>
#include <cstring>

SOCKET sockClient;

void Client::Connect()
{
	WSADATA wsaData;
	SOCKADDR_IN addrServer;//����˵�ַ
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//�½��ͻ���socket
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	//����Ҫ���ӵķ���˵�ַ
	addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.2"); //Ŀ��IP(127.0.0.2�ǻ��͵�ַ)
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(6000);//���Ӷ˿�6000
	//���ӵ������
	connect(sockClient, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
}

//��������
void Client::sendMessage(point sequence[][MAXFRAME], int count[])
{
	string sendStr;
	for (int i = 0; i < HAND_COUNT; i++)           //������0��������1
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


	const char * message = sendStr.c_str();           //��string����ת����char*
	send(sockClient, message, strlen(message) + 1, 0); //������Ϣ
}

//��������
string Client::recvMessage()
{
	string result = "���";
	char receiveMessage[50] = {};
	recv(sockClient, receiveMessage, 50, 0);
	result = receiveMessage;
	return result;
}

//�ر�socket
void Client::Close()
{
	closesocket(sockClient);
	WSACleanup();
}


