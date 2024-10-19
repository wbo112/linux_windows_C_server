
#include<WinSock2.h>
#include <Windows.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include< ws2tcpip.h >

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib") 

enum CMD {
	CMD_LOGIN,
	CMD_LOGINOUT,
	CMD_LOGIN_RESULT,
	CMD_LOGINOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERR
};
struct DataPackage {
	int age;
	char name[128];
};

struct DataHeader {
	short dataLength;
	short cmd;
};

struct Login :public DataHeader {

	Login() {
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];
};
struct LoginResult :public DataHeader {
	LoginResult() {
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;

};

struct LoginOut :public DataHeader {
	LoginOut() {
		dataLength = sizeof(LoginOut);
		cmd = CMD_LOGINOUT;
	}
	char userName[32];
};
struct LoginOutResult :public DataHeader {
	LoginOutResult() {
		dataLength = sizeof(LoginOutResult);
		cmd = CMD_LOGINOUT_RESULT;
		result = 0;
	}
	int result;
};
struct NewUserJoin :public DataHeader {
	NewUserJoin() {
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		result = 0;
	}
	int result;
};

int processor(SOCKET cSock) {
	char szRecv[1024] = {};
	int nLen = recv(cSock, (char*)szRecv, sizeof(DataHeader), 0);

	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0) {
		std::cout << "server break, client exit" << std::endl;
		return -1;
	}
	if (nLen >= sizeof(DataHeader)) {

	}

	switch (header->cmd) {
	case CMD_LOGIN_RESULT:
	{
		recv(cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LoginResult* loginResult = (LoginResult*)szRecv;
		std::cout << " CMD_LOGIN_RESULT recv data : " << loginResult->cmd << " length = " << loginResult->dataLength << " result = " << loginResult->result << std::endl;

	}
	break;
	case CMD_LOGINOUT:
	{
		recv(cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LoginOutResult* loginOutResult = (LoginOutResult*)szRecv;
		std::cout << " CMD_LOGINOUT recv data : " << loginOutResult->cmd << " length = " << loginOutResult->dataLength << " result = " << loginOutResult->result << std::endl;
	}
	break;
	case CMD_NEW_USER_JOIN:
	{
		recv(cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		NewUserJoin* newUserJoin = (NewUserJoin*)szRecv;
		std::cout << " CMD_NEW_USER_JOIN recv data : " << newUserJoin->cmd << " length = " << newUserJoin->dataLength << " result = " << newUserJoin->result << std::endl;
	}
	break;
	default:
	{

	}
	return 0;
	}
}
int main() {
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);


	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sock) {
		std::cout << " sock fail " << std::endl;
		return -1;
	}
	std::cout << "sock  succ" << std::endl;
	//	sockaddr_in sin1 = {
	//sin1.sin_family = AF_INET,
	//sin1.sin_port = htons(4567),
	//sin1.sin_addr.S_un.S_addr = inet_addr("127.0.0.1") };
	sockaddr_in sin = {

	};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4567);
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//std::cout << sin1.sin_addr.S_un.S_addr << std::endl;
	//std::cout << sin.sin_addr.S_un.S_addr << std::endl;
	int ret = connect(sock, (sockaddr*)&sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret) {
		std::cout << " connect fail" << std::endl;
		return -1;
	}
	while (true) {
		fd_set readFd;
		FD_ZERO(&readFd);
		FD_SET(sock, &readFd);
		timeval t = { 1,0 };
		int ret = select(sock, &readFd, 0, 0, &t);
		if (ret < 0) {
			std::cout << "select fail" << std::endl;
		}

		if (FD_ISSET(sock, &readFd)) {
			FD_CLR(sock, &readFd);
			if (-1 == processor(sock)) {
				std::cout << "select err" << std::endl;
				break;
			}
		}
		std::cout << "idle process business" << std::endl;
		Login login;
		strcpy(login.userName, "lyd");
		strcpy(login.passWord, "pwd");
		send(sock, (const char*)&login, sizeof(Login), 0);
		Sleep(1000);



	}
	closesocket(sock);
	WSACleanup();
	return 0;
}