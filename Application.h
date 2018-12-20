#ifndef __TARS_APPLICATION_H
#define __TARS_APPLICATION_H

#include <iostream>
#include "tc_epoll_server.h"

#include "ServantHelper.h"
#include "ServantHandle.h"


namespace tars
{

struct ServerConfig
{
    static bool        OpenCoroutine;        //是否启用协程处理方式
    static size_t      CoroutineMemSize;    //协程占用内存空间的最大大小
    static uint32_t    CoroutineStackSize;    //每个协程的栈大小(默认128k)

};

class Application
{
public:

	Application();

	virtual ~Application();

	void main(int argc, char *argv[]);

	void waitForShutdown();

    static TC_EpollServerPtr& getEpollServer();

	static void terminate();

	void waitForQuit();

	virtual void destroyApp() = 0;

protected:

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


};

}

#endif
