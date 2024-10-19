
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

struct Login {
	char userName[32];
	char passWord[32];
};
struct LoginResult {
	int result;

};

struct LoginOut {
	char userName[32];
};
struct LoginOutResult {
	int result;
};


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
		char cmdBuf[128] = {};
		std::cout << "sizeof " << sizeof cmdBuf << std::endl;
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "login")) {
			Login login = { "lyd","pwd" };
			DataHeader dataHeader = { sizeof(Login),CMD_LOGIN };
			send(sock, (const char*)&dataHeader, sizeof(DataHeader), 0);
			send(sock, (const char*)&login, sizeof(Login), 0);


			DataHeader resultDataHeader = {};
			LoginResult loginResult = {  };
			recv(sock, (char*)&resultDataHeader, sizeof(DataHeader), 0);
			recv(sock, (char*)&loginResult, sizeof(LoginResult), 0);
			std::cout << "loginResult = " << loginResult.result << std::endl;
		}
		else if (0 == strcmp(cmdBuf, "loginOut")) {
			LoginOut loginOut = { "lyd" };
			DataHeader dataHeader = { sizeof(LoginOut), CMD_LOGINOUT };
			send(sock, (const char*)&dataHeader, sizeof(DataHeader), 0);
			send(sock, (const char*)&loginOut, sizeof(LoginOut), 0);


			DataHeader resultDataHeader = {};
			LoginOutResult loginOutResult = { };
			recv(sock, (char*)&resultDataHeader, sizeof(DataHeader), 0);
			recv(sock, (char*)&loginOutResult, sizeof(LoginOutResult), 0);
			std::cout << "loginOutResult = " << loginOutResult.result << std::endl;
		}
		else if (0 == strcmp(cmdBuf, "exit")) {
			break;
		}
		else {

		}

	}
	closesocket(sock);
	WSACleanup();
	return 0;
}