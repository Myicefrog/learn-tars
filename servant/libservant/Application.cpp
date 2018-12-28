#include "Application.h"
#include "TarsNodeF.h"
#include "AdminServant.h"

namespace tars
{

TC_EpollServerPtr               Application::_epollServer  = NULL;
CommunicatorPtr                 Application::_communicator = NULL;

std::string ServerConfig::Application;      //应用名称
std::string ServerConfig::ServerName;       //服务名称,一个服务名称含一个或多个服务标识
std::string ServerConfig::Node;             //本机node地址

std::string ServerConfig::adapterName;
std::string ServerConfig::servantName;
std::string ServerConfig::adapterIp;
int         ServerConfig::adapterPort;

std::string ServerConfig::adapterName1;
std::string ServerConfig::servantName1;
std::string ServerConfig::adapterIp1;
int         ServerConfig::adapterPort1;

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

CommunicatorPtr& Application::getCommunicator()
{
    return _communicator;
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

	initializeClient();

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

void Application::initializeClient()
{
	CommunicatorPtr _commptr = make_shared<Communicator>();

	_communicator = _commptr;
}

void Application::initializeServer()
{
/*
	ServerConfig::Application  = "TestApp";

	ServerConfig::ServerName   = "HelloServer";

	ServerConfig::Node         = "127.0.0.1:19386";

	ServerConfig::OpenCoroutine = true;
*/

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

	TarsNodeFHelper::getInstance()->setNodeInfo(_communicator, ServerConfig::Node, ServerConfig::Application, ServerConfig::ServerName);

	ServantHelperManager::getInstance()->addServant<AdminServant>("AdminObj");

	ServantHelperManager::getInstance()->setAdapterServant("AdminAdapter", "AdminObj");

    TC_EpollServer::BindAdapterPtr lsPtr = make_shared<TC_EpollServer::BindAdapter>(_epollServer.get());

    lsPtr->setName("AdminAdapter");

	lsPtr->setEndpoint("127.0.0.1", 20001);

	lsPtr->setHandleGroupName("AdminAdapter");

	lsPtr->setHandleNum(1);

	lsPtr->setHandle<ServantHandle>();

	 _epollServer->bind(lsPtr);
	
}

void Application::bindAdapter(vector<TC_EpollServer::BindAdapterPtr>& adapters)
{
	int handleNum = 4;
	
/*
	string ip = "127.0.0.1";

	int port = 9877;

	string adapterName = "TestApp.HelloServer.HelloObjAdapter";

	string servantName = "TestApp.HelloServer.HelloObj";
*/

///////////////////////////////////////////////////////////////

	ServantHelperManager::getInstance()->setAdapterServant(ServerConfig::adapterName, ServerConfig::servantName);


///////////////////////////////////////////////////////////////

	TC_EpollServer::BindAdapterPtr bindAdapter = make_shared<TC_EpollServer::BindAdapter>(_epollServer.get());

	bindAdapter->setName(ServerConfig::adapterName);

	bindAdapter->setEndpoint(ServerConfig::adapterIp, ServerConfig::adapterPort);

	bindAdapter->setHandleGroupName(ServerConfig::adapterName);

	bindAdapter->setHandleNum(handleNum);


////////////////////////////////////////////////////////////////

	_epollServer->bind(bindAdapter);

	adapters.push_back(bindAdapter);

/////////////////////////////////////////////////////////////////

	if( ServerConfig::servantName1 != "")
	{
		ServantHelperManager::getInstance()->setAdapterServant(ServerConfig::adapterName1, ServerConfig::servantName1);

		TC_EpollServer::BindAdapterPtr bindAdapter1 = make_shared<TC_EpollServer::BindAdapter>(_epollServer.get());

		bindAdapter1->setName(ServerConfig::adapterName1);

		bindAdapter1->setEndpoint(ServerConfig::adapterIp1, ServerConfig::adapterPort1);

		bindAdapter1->setHandleGroupName(ServerConfig::adapterName1);

		bindAdapter1->setHandleNum(handleNum);

		_epollServer->bind(bindAdapter1);

		adapters.push_back(bindAdapter1);
	}
}

void Application::setHandle(TC_EpollServer::BindAdapterPtr& adapter)
{
    adapter->setHandle<ServantHandle>();
}


}
