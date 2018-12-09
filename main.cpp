#include "tc_epoll_server.h"

using namespace std;
using namespace tars;


int main()
{

	TC_EpollServerPtr _epollServer = make_shared<TC_EpollServer>();

    TC_EpollServer::NetThread* vNetThread = _epollServer->getNetThread();

    string ip = "127.0.0.1";

    int port = 9877;

	TC_EpollServer::BindAdapterPtr lsPtr = make_shared<TC_EpollServer::BindAdapter>(_epollServer.get());

    lsPtr->setEndpoint(ip,port);

    _epollServer->bind(lsPtr);

    vNetThread->createEpoll(1);
    
    TC_EpollServer::Handle handle;

    handle.setEpollServer(_epollServer.get());   

    handle.start();

    vNetThread->run();

    return 0;
}
