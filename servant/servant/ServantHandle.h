#ifndef __TARS_SERVANT_HANDLE_H
#define __TARS_SERVANT_HANDLE_H

#include <map>
#include <string>
#include <memory>
#include <deque>
#include "tc_monitor.h"
#include "tc_epoll_server.h"
#include "Servant.h"
#include <ucontext.h>
#include "CoroutineScheduler.h"


namespace tars
{

class ServantHandle : public TC_EpollServer::Handle
{
public:

    enum
    {
        HEART_BEAT_INTERVAL = 10, /**心跳间隔时间**/
    };

	ServantHandle();
	
	~ServantHandle();

	virtual void run();

	virtual void handle(const TC_EpollServer::tagRecvData &stRecvData);

protected:

	virtual void handleRequest();

	virtual void handleRecvData(TC_EpollServer::tagRecvData *stRecvData);

	virtual void stopHandle() {}

	virtual void heartbeat();


protected:

	void initialize();

	map<string, ServantPtr> _servants;

	TC_ThreadLock           _monitor;

	CoroutineScheduler     *_coroSched;

};

typedef shared_ptr<ServantHandle> ServantHandlePtr;

}
#endif
