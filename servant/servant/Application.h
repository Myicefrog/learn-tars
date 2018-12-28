#ifndef __TARS_APPLICATION_H
#define __TARS_APPLICATION_H

#include <iostream>
#include "tc_epoll_server.h"

#include "ServantHelper.h"
#include "ServantHandle.h"

#include "Global.h"
#include "Communicator.h"

#include "TarsNodeF.h"

namespace tars
{

#define TARS_KEEPALIVE(adapter)      {TarsNodeFHelper::getInstance()->keepAlive(adapter);}

struct ServerConfig
{
    static std::string Application;         //应用名称
    static std::string ServerName;          //服务名称,一个服务名称含一个或多个服务标识
 	static std::string Node;                //本机node地址
    
	static bool        OpenCoroutine;        //是否启用协程处理方式
    static size_t      CoroutineMemSize;    //协程占用内存空间的最大大小
    static uint32_t    CoroutineStackSize;    //每个协程的栈大小(默认128k)

	static std::string adapterName;
	static std::string servantName;
	static std::string adapterIp;
	static int         adapterPort;

	static std::string adapterName1;
	static std::string servantName1;
	static std::string adapterIp1;
	static int         adapterPort1;
};

class Application
{
public:

	Application();

	virtual ~Application();

	void main(int argc, char *argv[]);

	void waitForShutdown();

	static CommunicatorPtr& getCommunicator();

    static TC_EpollServerPtr& getEpollServer();

	static void terminate();

	void waitForQuit();

	virtual void destroyApp() = 0;

protected:

	void initializeClient();

	virtual void initialize() = 0;

	void initializeServer();

	void bindAdapter(vector<TC_EpollServer::BindAdapterPtr>& adapters);

	virtual void setHandle(TC_EpollServer::BindAdapterPtr& adapter);

	template<typename T> void addServant(const string &id)
    {
        ServantHelperManager::getInstance()->addServant<T>(id,true);
    }

protected:

	static TC_EpollServerPtr   _epollServer;

	static CommunicatorPtr     _communicator;
};

}

#endif
