#include "tc_epoll_server.h"

using namespace std;
using namespace tars;


int main()
{

	NetThread* vNetThread = new NetThread();
	string ip = "127.0.0.1";
	int port = 9877;
	vNetThread->bind(ip,port);
	vNetThread->createEpoll(1);
	vNetThread->run();

	delete vNetThread;
	return 0;
}

