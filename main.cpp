#include "tc_epoll_server.h"

using namespace std;
using namespace tars;


int main()
{

    TC_EpollServer*  _epollServer = new TC_EpollServer();

    TC_EpollServer::NetThread* vNetThread = _epollServer->getNetThread();

    string ip = "127.0.0.1";

    int port = 9877;

    vNetThread->bind(ip,port);

    vNetThread->createEpoll(1);
    
    TC_EpollServer::Handle handle;

    handle.setEpollServer(_epollServer);   

    handle.start();

    vNetThread->run();
    return 0;
}
