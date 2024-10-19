
#include<WinSock2.h>
#include <Windows.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include< ws2tcpip.h >

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

//#pragma comment(lib,"ws2_32.lib") 
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
	while (true) {

		cSock = accept(sock, (sockaddr*)&clientAddr, &nAddrLen);
		if (INVALID_SOCKET == cSock) {
			std::cout << "accept fail" << std::endl;
			break;
		}
		else {
			std::cout << "accept succ" << std::endl;
		}
		std::cout << "new client ip = " << inet_ntoa(clientAddr.sin_addr) << std::endl;;
		char msgBuf[] = "Hello ,I'm Server.";
		send(cSock, msgBuf, strlen(msgBuf) + 1, 0);
		closesocket(cSock);
	}
	WSACleanup();
	return 0;
}