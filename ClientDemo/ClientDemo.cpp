
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
	char recvBuf[256] = {};
	int nLen = recv(sock, recvBuf, 256, 0);
	if (nLen > 0) {
		std::cout << "recvBuf" << recvBuf << std::endl;
	}
	closesocket(sock);
	WSACleanup();
	return 0;
}