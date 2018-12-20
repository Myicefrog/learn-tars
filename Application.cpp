#include "Application.h"

namespace tars
{

TC_EpollServerPtr               Application::_epollServer  = NULL;

bool        ServerConfig::OpenCoroutine;    //是否启用协程处理方式
size_t      ServerConfig::CoroutineMemSize; //协程占用内存空间的最大大小
uint32_t    ServerConfig::CoroutineStackSize;   //每个协程的栈大小(默认128k)


Application::Application()
{
}

Application::~Application()
{
    terminate();
}

TC_EpollServerPtr& Application::getEpollServer()
{
    return _epollServer;
}

void Application::terminate()
{
    if(_epollServer)
    {
        _epollServer->terminate();
    }
}

void Application::waitForShutdown()
{
    waitForQuit();

    //destroyApp();
}

void Application::waitForQuit()
{

    unsigned int iNetThreadNum = _epollServer->getNetThreadNum();
    vector<TC_EpollServer::NetThread*> vNetThread = _epollServer->getNetThread();

    for (size_t i = 0; i < iNetThreadNum; ++i)
    {
        vNetThread[i]->start();
    }

    while(!_epollServer->isTerminate())
    {
        {
            TC_ThreadLock::Lock sync(*_epollServer);
            _epollServer->timedWait(5000);
        }
	}

    if(_epollServer->isTerminate())
    {
        for(size_t i = 0; i < iNetThreadNum; ++i)
        {
            vNetThread[i]->terminate();
            vNetThread[i]->getThreadControl().join();
        }

        _epollServer->stopThread();
    }
	
}

void Application::main(int argc, char *argv[])
{

	initializeServer();

	vector<TC_EpollServer::BindAdapterPtr> adapters;

	bindAdapter(adapters);

	initialize();

	for (size_t i = 0; i < adapters.size(); ++i)
	{
		setHandle(adapters[i]);
	}

	_epollServer->startHandle();

	_epollServer->createEpoll();


}

void Application::initializeServer()
{
	ServerConfig::OpenCoroutine = true;

	ServerConfig::CoroutineMemSize = 1073741824;

	ServerConfig::CoroutineStackSize = 1310272;

	unsigned int iNetThreadNum = 3;

	if(iNetThreadNum < 1)
	{
		iNetThreadNum = 1;
	}

	if(iNetThreadNum > 15)
	{
		iNetThreadNum = 15;
	}

	_epollServer = make_shared<TC_EpollServer>(iNetThreadNum);
	
}

void Application::bindAdapter(vector<TC_EpollServer::BindAdapterPtr>& adapters)
{
	int handleNum = 4;
	
	string ip = "127.0.0.1";

	int port = 9877;

	string adapterName = "TestApp.HelloServer.HelloObjAdapter";

	string servantName = "TestApp.HelloServer.HelloObj";

///////////////////////////////////////////////////////////////

	ServantHelperManager::getInstance()->setAdapterServant(adapterName, servantName);

///////////////////////////////////////////////////////////////

	TC_EpollServer::BindAdapterPtr bindAdapter = make_shared<TC_EpollServer::BindAdapter>(_epollServer.get());

	bindAdapter->setName(adapterName);

	bindAdapter->setEndpoint(ip,port);

	bindAdapter->setHandleGroupName(adapterName);

	bindAdapter->setHandleNum(handleNum);

////////////////////////////////////////////////////////////////

	_epollServer->bind(bindAdapter);

	adapters.push_back(bindAdapter);
}

void Application::setHandle(TC_EpollServer::BindAdapterPtr& adapter)
{
    adapter->setHandle<ServantHandle>();
}


}
