#include "tc_epoll_server.h"
#include "HelloImp.h"
#include "ServantHelper.h"
#include "ServantHandle.h"

using namespace std;
using namespace tars;


int main()
{

	TC_EpollServerPtr _epollServer = make_shared<TC_EpollServer>(3);

    string ip = "127.0.0.1";

    int port = 9877;

///////////////////////////////////////////////

	string adapterName = "TestApp.HelloServer.HelloObjAdapter";
	string servantName = "TestApp.HelloServer.HelloObj";

	ServantHelperManager::getInstance()->setAdapterServant(adapterName, servantName);

	TC_EpollServer::BindAdapterPtr lsPtr = make_shared<TC_EpollServer::BindAdapter>(_epollServer.get());

	lsPtr->setName(adapterName);

    lsPtr->setEndpoint(ip,port);

    _epollServer->bind(lsPtr);

//////////////////////////////////////////////

	ServantHelperManager::getInstance()->addServant<HelloImp>(servantName,true); 

/////////////////////////////////////////////
    
    vector<ServantHandlePtr>          handles;

	int handleNum = 4;

	for (int32_t i = 0; i < handleNum; ++i)
	{
		ServantHandlePtr handle = make_shared<ServantHandle>();
		handle->setEpollServer(_epollServer.get());
		handle->setHandleGroup(lsPtr);
		handles.push_back(handle);
	}
    
	for(auto& handle : handles)
	{
		handle->start();
	}

/////////////////////////////////////////////

	_epollServer->createEpoll();

    unsigned int iNetThreadNum = _epollServer->getNetThreadNum();
    vector<TC_EpollServer::NetThread*> vNetThread = _epollServer->getNetThread();

	for (size_t i = 0; i < iNetThreadNum; ++i)
    {
        vNetThread[i]->start();
    }

    while(!_epollServer->isTerminate())
    {

	}	

	if(_epollServer->isTerminate())
    {
        for(size_t i = 0; i < iNetThreadNum; ++i)
        {
            vNetThread[i]->terminate();
            vNetThread[i]->getThreadControl().join();
        }

        //_epollServer->stopThread();
    
		for(auto& handle : handles)
		{
			if (handle->isAlive())
            {
                handle->getThreadControl().join();
            }
		}
		
    }

    return 0;
}
