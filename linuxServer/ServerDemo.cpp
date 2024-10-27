

#include "TcpServer.hpp"

int main() {
	std::cout << "easyTcpServer start" << std::endl;
	EasyTcpServer  easyTcpServer;
	easyTcpServer.init();
	easyTcpServer.bindd(nullptr, 4567);
	easyTcpServer.list();

	while (easyTcpServer.isRun()) {
		easyTcpServer.onRun();
	}
	easyTcpServer.closeClient2All();
	easyTcpServer.closeServer();

	std::cout << "easyTcpServer exit" << std::endl;

	return 0;
}