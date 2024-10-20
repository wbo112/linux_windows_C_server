
#ifdef  _WIN32

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib") 

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
#include "TcpClient.hpp"


void cmdThread(EsayTcpClient* tcpClient) {
	while (true) {


		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "login")) {
			Login login = {  };
			strcpy(login.userName, "lyd");
			strcpy(login.passWord, "pwd");
			std::cout << "send  = login" << std::endl;
			tcpClient->sendData(&login);
			//std::cout << " cmd login " << std::endl;
			//LoginResult loginResult = {  };

			//recv(sock, (char*)&loginResult, sizeof(LoginResult), 0);
			//std::cout << "loginResult = " << loginResult.result << std::endl;
		}
		else if (0 == strcmp(cmdBuf, "loginOut")) {
			LoginOut loginOut = {  };

			strcpy(loginOut.userName, "lyd");
			std::cout << " send = loginOut " << std::endl;
			tcpClient->sendData(&loginOut);
			//send(sock, (const char*)&loginOut, sizeof(LoginOut), 0);


			//LoginOutResult loginOutResult = { };

			//recv(sock, (char*)&loginOutResult, sizeof(LoginOutResult), 0);
			//std::cout << "loginOutResult = " << loginOutResult.result << std::endl;
		}
		else if (0 == strcmp(cmdBuf, "exit")) {
			tcpClient->clo();
			std::cout << "exit" << std::endl;
			return;
		}
		else {
			std::cout << "no support cmd" << std::endl;
		}
	}
}

int main() {
	EsayTcpClient tcpClient = {};
	tcpClient.initSocket();
	tcpClient.conn("127.0.0.1", 4567);
	std::thread t1(cmdThread, &tcpClient);
	while (tcpClient.isRun()) {
		tcpClient.onRun();
	}
	if (t1.joinable()) {

		t1.join();
	}
	tcpClient.clo();

	return 0;
}