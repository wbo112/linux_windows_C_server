﻿
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

int main() {
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);


	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in sin = {
	sin.sin_family = AF_INET,
	sin.sin_port = htons(4567),
	sin.sin_addr.S_un.S_addr = INADDR_ANY
	};

	//绑定网络端口
	if (SOCKET_ERROR == bind(sock, (sockaddr*)&sin, sizeof(sockaddr))) {
		std::cout << " bind fail" << std::endl;
	}
	else {
		std::cout << " bind succ" << std::endl;
	}
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	// 监听套接字
	if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "listen failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	std::cout << "listen succ" << std::endl;
	SOCKET cSock = INVALID_SOCKET;
	cSock = accept(sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == cSock) {
		std::cout << "accept fail" << std::endl;

	}
	else {
		std::cout << "accept succ" << std::endl;
	}
	std::cout << "new client ip = " << inet_ntoa(clientAddr.sin_addr) << std::endl;;
	while (true) {
		DataHeader  header;
		int nLen = recv(cSock, (char*)&header, sizeof(DataHeader), 0);
		if (nLen <= 0) {
			std::cout << "client exit" << std::endl;
			break;
		}

		switch (header.cmd) {
		case CMD_LOGIN:
		{

			Login login;
			recv(cSock, (CHAR*)&login + sizeof(DataHeader), sizeof(Login) - sizeof(DataHeader), 0);
			std::cout << "recv data : " << login.cmd << " length = " << login.dataLength << " username = " << login.userName << " password = " << login.passWord << std::endl;
			LoginResult loginResult;

			//send(cSock, (char*)&header, sizeof(DataHeader), 0);
			send(cSock, (char*)&loginResult, sizeof(LoginResult), 0);
		}
		break;
		case CMD_LOGINOUT:
		{

			LoginOut loginOut = {};
			recv(cSock, (CHAR*)&loginOut + sizeof(DataHeader), sizeof(LoginOut) - sizeof(DataHeader), 0);
			std::cout << "recv data : " << loginOut.cmd << " length = " << loginOut.dataLength << " username = " << loginOut.userName << std::endl;
			LoginOutResult loginOutResult = {};
			loginOutResult.result = 1;

			send(cSock, (char*)&loginOutResult, sizeof(LoginOutResult), 0);
		}
		break;
		default:
		{

			header.cmd = CMD_ERR;
			header.dataLength = 0;
			send(cSock, (char*)&header, sizeof(DataHeader), 0);

			std::cout << "switch default" << std::endl;
		}
		}



	}
	closesocket(cSock);
	closesocket(sock);
	WSACleanup();
	return 0;
}