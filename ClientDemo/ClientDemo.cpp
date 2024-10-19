
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
			Login login = {  };
			strcpy(login.userName, "lyd");
			strcpy(login.passWord, "pwd");
			std::cout << "send  = login" << std::endl;
			send(sock, (const char*)&login, sizeof(Login), 0);



			LoginResult loginResult = {  };

			recv(sock, (char*)&loginResult, sizeof(LoginResult), 0);
			std::cout << "loginResult = " << loginResult.result << std::endl;
		}
		else if (0 == strcmp(cmdBuf, "loginOut")) {
			LoginOut loginOut = {  };

			strcpy(loginOut.userName, "lyd");

			send(sock, (const char*)&loginOut, sizeof(LoginOut), 0);


			LoginOutResult loginOutResult = { };

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