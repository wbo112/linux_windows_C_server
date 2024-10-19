
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

#include <vector>
std::vector<SOCKET> socket_set;


int processor(SOCKET cSock) {
	char szRecv[1024] = {};
	int nLen = recv(cSock, (char*)szRecv, sizeof(DataHeader), 0);

	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0) {
		std::cout << "client exit" << std::endl;
		return -1;
	}
	if (nLen >= sizeof(DataHeader)) {

	}

	switch (header->cmd) {
	case CMD_LOGIN:
	{

		recv(cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		Login* login = (Login*)szRecv;
		std::cout << "recv data : " << login->cmd << " length = " << login->dataLength << " username = " << login->userName << " password = " << login->passWord << std::endl;
		LoginResult loginResult;

		//send(cSock, (char*)&header, sizeof(DataHeader), 0);
		send(cSock, (char*)&loginResult, sizeof(LoginResult), 0);
	}
	break;
	case CMD_LOGINOUT:
	{

		recv(cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LoginOut* loginOut = (LoginOut*)szRecv;
		std::cout << "recv data : " << loginOut->cmd << " length = " << loginOut->dataLength << " username = " << loginOut->userName << std::endl;
		LoginOutResult loginOutResult = {};
		loginOutResult.result = 1;

		send(cSock, (char*)&loginOutResult, sizeof(LoginOutResult), 0);
	}
	break;
	default:
	{
		DataHeader header;
		header.cmd = CMD_ERR;
		header.dataLength = 0;
		send(cSock, (char*)&header, sizeof(DataHeader), 0);

		std::cout << "switch default" << std::endl;
	}
	return 0;
	}
}
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
	while (true) {
		fd_set readFd;
		fd_set writeFd;
		fd_set expFd;
		FD_ZERO(&readFd);
		FD_ZERO(&writeFd);
		FD_ZERO(&expFd);

		FD_SET(sock, &readFd);
		FD_SET(sock, &writeFd);
		FD_SET(sock, &expFd);
		for (size_t n = 0; n < socket_set.size(); n++) {
			FD_SET(socket_set[n], &readFd);
		}
		int max_fd = sock; // 初始化最大文件描述符为监听socket
		// 遍历socket_set，更新最大文件描述符
		for (size_t n = 0; n < socket_set.size(); n++) {
			if (socket_set[n] > max_fd) {
				max_fd = socket_set[n];
			}
		}
		int ret = select(max_fd + 1, &readFd, &writeFd, &expFd, NULL);
		if (ret < 0) {
			std::cout << "select fail " << std::endl;
			break;
		}

		if (FD_ISSET(sock, &readFd)) {
			FD_CLR(sock, &readFd);
			sockaddr_in clientAddr = {};
			int nAddrLen = sizeof(sockaddr_in);
			SOCKET sock_client = INVALID_SOCKET;
			sock_client = accept(sock, (sockaddr*)&clientAddr, &nAddrLen);


			std::cout << "new client ip = " << inet_ntoa(clientAddr.sin_addr) << std::endl;;
			if (INVALID_SOCKET == sock_client) {
				std::cout << "accept fail" << std::endl;
			}
			std::cout << "new socket add" << std::endl;
			socket_set.push_back(sock_client);
		}
		for (size_t n = 0; n < readFd.fd_count; n++) {

			if (-1 == processor(readFd.fd_array[n])) {
				auto iter = find(socket_set.begin(), socket_set.end(), readFd.fd_array[n]);
				if (iter != socket_set.end()) {
					socket_set.erase(iter);
				}
			}
		}



	}
	for (size_t n = 0; n < socket_set.size(); n++) {
		closesocket(socket_set[n]);

	}
	closesocket(sock);
	WSACleanup();
	return 0;
}