#ifndef __TARS_SERVANT_HANDLE_H
#define __TARS_SERVANT_HANDLE_H

#include <map>
#include <string>
#include <memory>
#include <deque>
#include "tc_monitor.h"
#include "tc_epoll_server.h"
#include "Servant.h"


namespace tars
{

class ServantHandle : public TC_EpollServer::Handle
{
public:

	ServantHandle();
	
	~ServantHandle();

	virtual void handle(const TC_EpollServer::tagRecvData &stRecvData);

protected:

	void initialize();

	map<string, ServantPtr> _servants;

	TC_ThreadLock           _monitor;

};

typedef shared_ptr<ServantHandle> ServantHandlePtr;

}
#endif
