#pragma once
#pragma once
#ifndef TCP_CLIENT_HPP
#define TCP_CLIENT_HPP
#ifdef  _WIN32
#pragma comment(lib,"ws2_32.lib") 

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<WinSock2.h>
#include <Windows.h>
#include< ws2tcpip.h >
#else
#include <unistd.h>
#include<arpa/inet.h>
#include<string.h>
#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <thread>
#include "MessageHeader.hpp"
class EsayTcpClient {
	SOCKET sock;
	bool run;
public:
	EsayTcpClient() {
		sock = INVALID_SOCKET;
	}
	virtual ~EsayTcpClient() {

		clo();
	}

	// 初始化socket
	int initSocket() {
#ifdef  _WIN32
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif
		clo();
		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == sock) {
			std::cout << " sock fail " << std::endl;
			return -1;
		}
		else {

			std::cout << "sock  succ" << std::endl;
			return 0;
		}
	}
	bool isRun() {
		return run;
	}

	// 连接服务器
	int   conn(const char* ip, unsigned short port) {
		if (INVALID_SOCKET == sock) {
			std::cout << " sock fail ,connect err" << std::endl;
			return -1;
		}
		sockaddr_in sin = {

		};
		sin.sin_family = AF_INET;
		sin.sin_port = htons(4567);
#ifdef  _WIN32
		sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		sin.sin_addr.s_addr = inet_addr(ip);

#endif //  _WIN32

		//std::cout << sin1.sin_addr.S_un.S_addr << std::endl;
		//std::cout << sin.sin_addr.S_un.S_addr << std::endl;
		int ret = connect(sock, (sockaddr*)&sin, sizeof(sockaddr_in));
		if (SOCKET_ERROR == ret) {
			std::cout << " connect fail" << std::endl;
			return -1;
		}
		run = true;
		return 0;
	}


	void clo() {
		if (INVALID_SOCKET != sock) {
#ifdef  _WIN32
			closesocket(sock);
			WSACleanup();
#else
			close(sock);
#endif
			run = false;

		}
	}


	bool  onRun() {
		if (run) {

			fd_set readFd;
			FD_ZERO(&readFd);
			FD_SET(sock, &readFd);
			timeval t = { 1,0 };
			int ret = select(sock + 1, &readFd, 0, 0, &t);
			if (ret < 0) {
				std::cout << "select fail" << std::endl;
			}

			if (FD_ISSET(sock, &readFd)) {
				FD_CLR(sock, &readFd);
				if (-1 == recvData()) {
					std::cout << "select err" << std::endl;
					return false;
				}
			}
			//std::cout << "idle process business" << std::endl;
			//Login login;
			//strcpy(login.userName, "lyd");
			//strcpy(login.passWord, "pwd");
			//send(sock, (const char*)&login, sizeof(Login), 0);
#ifdef  _WIN32
			Sleep(1000);
#else
			sleep(1);
#endif
			return true;


		}
	}

	// 接收数据
	int recvData() {
		std::cout << "thread start" << std::endl;
		char szRecv[1024] = {};
		int nLen = recv(sock, (char*)szRecv, sizeof(DataHeader), 0);

		DataHeader* header = (DataHeader*)szRecv;
		if (nLen <= 0) {
			std::cout << "server break, client exit" << std::endl;
			return -1;
		}
		if (nLen >= sizeof(DataHeader)) {

		}

		recv(sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);

		onNetData(header);




		std::cout << "thread end" << std::endl;
		return 0;
	}
	void onNetData(DataHeader* header) {
		switch (header->cmd) {
		case CMD_LOGIN_RESULT:
		{
			LoginResult* loginResult = (LoginResult*)header;
			std::cout << " CMD_LOGIN_RESULT recv data : " << loginResult->cmd << " length = " << loginResult->dataLength << " result = " << loginResult->result << std::endl;

		}
		break;
		case CMD_LOGINOUT:
		{

			LoginOutResult* loginOutResult = (LoginOutResult*)header;
			std::cout << " CMD_LOGINOUT recv data : " << loginOutResult->cmd << " length = " << loginOutResult->dataLength << " result = " << loginOutResult->result << std::endl;
		}
		break;
		case CMD_NEW_USER_JOIN:
		{

			NewUserJoin* newUserJoin = (NewUserJoin*)header;
			std::cout << " CMD_NEW_USER_JOIN recv data : " << newUserJoin->cmd << " length = " << newUserJoin->dataLength << " result = " << newUserJoin->result << std::endl;
		}
		break;
		default:
		{

		}
		}
	}

	int sendData(DataHeader* header) {
		if (run && header) {

			return send(sock, (const char*)header, header->dataLength, 0); //header->dataLength
		}
		return SOCKET_ERROR;
	}
private:



};



#endif // !TCP_CLIENT_HPP
