#pragma once
#pragma once
#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP
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
#include <vector>
//#include <ranges>
#include "MessageHeader.hpp"



#ifndef RECV_BUF_SIZE
#define RECV_BUF_SIZE 409600
#endif // !RECV_BUF_SIZE


class ClientSocket {
private:
	SOCKET sockfd;

	char szMsgBuf[RECV_BUF_SIZE];

	int lastPos;
public:
	ClientSocket(SOCKET sockfd = INVALID_SOCKET) :sockfd(sockfd), lastPos(0) {
		memset(szMsgBuf, 0, sizeof(szMsgBuf));
	}
	SOCKET  getSockefd() {
		return sockfd;
	}
	char* getMsgBuf() {
		return szMsgBuf;
	}
	int getLastPos() {
		return lastPos;
	}
	void setLastPos(int lastPos)
	{
		this->lastPos = lastPos;
	}



};
class EasyTcpServer {

private:
	bool run;
	SOCKET serverSock;
	std::vector<ClientSocket*> client_set;
public:
	EasyTcpServer() {
		serverSock = INVALID_SOCKET;
	}
	virtual ~EasyTcpServer() {

	}

	void init() {
#ifdef  _WIN32

		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif

		serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}

	int bindd(const char* ip, unsigned short port) {
		sockaddr_in sin = {
		};
		sin.sin_family = AF_INET;
		sin.sin_port = htons(port);
#ifdef  _WIN32
		if (ip) {
			sin.sin_addr.S_un.S_addr = inet_addr(ip);
		}
		else {
			sin.sin_addr.S_un.S_addr = INADDR_ANY;
		}
#else
		if (ip) {
			sin.sin_addr.s_addr = inet_addr(ip);
		}else {
			sin.sin_addr.s_addr = INADDR_ANY;
			}
#endif

			//绑定网络端口
			if (SOCKET_ERROR == bind(serverSock, (sockaddr*)&sin, sizeof(sockaddr))) {
				std::cout << " bind fail" << std::endl;
				return -1;
			}
			else {
				std::cout << " bind succ" << std::endl;
				run = true;
				return 0;
			}
	}

	void closeServer() {
		if (INVALID_SOCKET != serverSock) {
#ifdef  _WIN32
			closesocket(serverSock);
			WSACleanup();
#else
			close(serverSock);
#endif
			run = false;

		}
	}

	int list() {
		sockaddr_in clientAddr = {};
		int nAddrLen = sizeof(sockaddr_in);
		// 监听套接字
		if (listen(serverSock, SOMAXCONN) == SOCKET_ERROR) {

#ifdef  _WIN32
			std::cerr << "listen failed: " << WSAGetLastError() << std::endl;
			closeServer();
			WSACleanup();
#else
			std::cout << "listen fail " << std::endl;
#endif
			return 1;
		}
		std::cout << "listen succ" << std::endl;
		return 0;

	}
	bool isRun() {
		return run;
	}
	int onRun() {

		fd_set readFd;
		fd_set writeFd;
		fd_set expFd;
		FD_ZERO(&readFd);
		FD_ZERO(&writeFd);
		FD_ZERO(&expFd);

		FD_SET(serverSock, &readFd);
		FD_SET(serverSock, &writeFd);
		FD_SET(serverSock, &expFd);
		for (size_t n = 0; n < client_set.size(); n++) {
			FD_SET(client_set[n]->getSockefd(), &readFd);
		}
		int max_fd = serverSock; // 初始化最大文件描述符为监听socket
		// 遍历socket_set，更新最大文件描述符
		for (size_t n = 0; n < client_set.size(); n++) {
			if (client_set[n]->getSockefd() > max_fd) {
				max_fd = client_set[n]->getSockefd();
			}
		}
		timeval t = { 10,0 };
		int ret = select(max_fd + 1, &readFd, &writeFd, &expFd, &t);
		if (ret < 0) {
			std::cout << "select fail " << std::endl;
			return -1;
		}

		if (FD_ISSET(serverSock, &readFd)) {
			FD_CLR(serverSock, &readFd);
			acc();

		}
		/*	for (auto it = client_set.begin(); it < client_set.end(); it++) {
				std::cout << "a clientSock lastPost = " << (*it)->getLastPos() << std::endl;
			}*/

		auto it = client_set.begin();
		while (it != client_set.end()) {
			if (FD_ISSET((*it)->getSockefd(), &readFd) && -1 == recvData((*it))) {
				/*		for (auto ita = client_set.begin(); ita < client_set.end(); ita++) {
							std::cout << "b clientSock lastPost = " << (*ita)->getLastPos() << std::endl;
						}*/
				std::cout << "socket = " << *it << "  exit  " << std::endl;
				delete* it;
				it = client_set.erase(it);  // 移除当前元素并更新迭代器
				/*	 for (auto ita = client_set.begin(); ita < client_set.end(); ita++) {
						 std::cout << "b clientSock lastPost = " << (*ita)->getLastPos() << std::endl;
					 }*/

			}
			else {
				++it;  // 移动到下一个元素
			}
		}




	};
	int acc() {
		sockaddr_in clientAddr = {};
		//#ifdef _WIN32

					//int nAddrLen = sizeof(sockaddr_in);
		socklen_t nAddrLen = sizeof(sockaddr_in);
		//#endif // _WIN32

		SOCKET sock_client = INVALID_SOCKET;
		sock_client = accept(serverSock, (sockaddr*)&clientAddr, &nAddrLen);


		std::cout << "socket = " << sock_client << " new client ip = " << inet_ntoa(clientAddr.sin_addr) << std::endl;;
		if (INVALID_SOCKET == sock_client) {
			std::cout << "accept fail" << std::endl;
			return -1;
		}
		std::cout << "new socket add" << std::endl;
		for (size_t n = 0; n < client_set.size(); n++) {
			NewUserJoin newUserJoin = {};
			send(client_set[n]->getSockefd(), (const char*)&newUserJoin, sizeof(NewUserJoin), 0);
		}
		client_set.push_back(new ClientSocket(sock_client));
		//for (auto it = client_set.begin(); it < client_set.end(); it++) {
		//	std::cout << "b clientSock lastPost = " << (*it)->getLastPos() << std::endl;
		//}
		return 0;

	}
	int recvData(ClientSocket* clientSock) {
		//char* szRecv = new char[RECV_BUF_SIZE];
		//std::cout << "a lastPos = " << clientSock->getLastPos() << std::endl;
		char szRecv[RECV_BUF_SIZE / 10] = {};
		int nLen = recv(clientSock->getSockefd(), szRecv, sizeof(szRecv), 0);
		//std::cout << "c lastPos = " << clientSock->getLastPos() << std::endl;
		if (nLen <= 0) {
			std::cout << "client exit, socket fd = " << clientSock->getSockefd() << std::endl;

			return -1;
		}
		if (nLen >= sizeof(DataHeader)) {

		}
		memcpy(clientSock->getMsgBuf() + clientSock->getLastPos(), szRecv, nLen);
		/*recv(clientSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		onNetMsg(clientSock, header);*/



		DataHeader* header = (DataHeader*)szRecv;
		//std::cout << "b lastPos = " << clientSock->getLastPos() << std::endl;
		clientSock->setLastPos(clientSock->getLastPos() + nLen);
		while (clientSock->getLastPos() >= sizeof(DataHeader)) {
			DataHeader* header = (DataHeader*)clientSock->getMsgBuf();
			if (clientSock->getLastPos() >= header->dataLength) {

				int  nSize = clientSock->getLastPos() - header->dataLength;
				//recv(sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
				onNetMsg(clientSock->getSockefd(), header);
				memcpy(clientSock->getMsgBuf(), clientSock->getMsgBuf() + header->dataLength, nSize);

				clientSock->setLastPos(nSize);
			}
			else {
				break;
			}
		}




		std::cout << "thread end" << std::endl;
		//delete szRecv;

	}

	int onNetMsg(SOCKET clientSock, DataHeader* header) {


		switch (header->cmd) {
		case CMD_LOGIN:
		{


			Login* login = (Login*)header;
			std::cout << "recv data : " << login->cmd << " length = " << login->dataLength << " username = " << login->userName << " password = " << login->passWord << std::endl;
			LoginResult loginResult;

			//send(cSock, (char*)&header, sizeof(DataHeader), 0);
			send(clientSock, (char*)&loginResult, sizeof(LoginResult), 0);
		}
		break;
		case CMD_LOGINOUT:
		{


			LoginOut* loginOut = (LoginOut*)header;
			std::cout << "recv data : " << loginOut->cmd << " length = " << loginOut->dataLength << " username = " << loginOut->userName << std::endl;
			LoginOutResult loginOutResult = {};
			loginOutResult.result = 1;

			send(clientSock, (char*)&loginOutResult, sizeof(LoginOutResult), 0);
		}
		break;
		default:
		{
			DataHeader header;
			header.cmd = CMD_ERR;
			header.dataLength = 0;
			send(clientSock, (char*)&header, sizeof(DataHeader), 0);

			std::cout << "switch default" << std::endl;
		}
		}
		return 0;
	}
	void closeClient2All() {
		for (auto it = client_set.begin(); it != client_set.end(); ++it) {
#ifdef _WIN32
			closesocket((*it)->getSockefd());
			delete* it;

#else
			close((*it)->getSockefd());
			delete* it;
#endif

		}
		client_set.clear();
	}

	int sendData(SOCKET clientSock, DataHeader* header) {
		if (run && header) {

			return send(clientSock, (const char*)header, header->dataLength, 0); //header->dataLength
		}
		return SOCKET_ERROR;
	}
	int sendData2All(DataHeader* header) {
		for (auto it = client_set.begin(); it != client_set.end(); ++it) {
			if (SOCKET_ERROR == sendData((*it)->getSockefd(), header)) {
				return SOCKET_ERROR;
			}
		}

		return 0;
	}
};
#endif