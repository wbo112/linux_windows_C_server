

#include "TcpServer.hpp"

int main() {
	EasyTcpServer  easyTcpServer;
	easyTcpServer.init();
	easyTcpServer.bindd(nullptr, 4567);
	easyTcpServer.list();

	while (easyTcpServer.isRun()) {
		easyTcpServer.onRun();
		}
	easyTcpServer.closeClient2All();
	easyTcpServer.closeServer();

	

	return 0;
}