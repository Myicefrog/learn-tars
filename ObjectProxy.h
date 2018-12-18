#ifndef __TARS_OBJECT_PROXY_H_
#define __TARS_OBJECT_PROXY_H_

#include "Message.h"
#include "CommunicatorEpoll.h"
#include "Transceiver.h"
#include "tc_timeout_queue_new.h"

namespace tars
{
class ObjectProxy
{
public:
	
	ObjectProxy(CommunicatorEpoll * pCommunicatorEpoll, const string& host, const uint16_t& _port);

	~ObjectProxy();

	void invoke(ReqMessage* msg);

	void finishInvoke(const string& rsp);

	void finishInvoke(ReqMessage * msg);

	void doInvoke();

	CommunicatorEpoll * getCommunicatorEpoll()
    {
        return _communicatorEpoll;
    }

	inline uint32_t generateId()
    {
        _id++;

        if(_id == 0)
        {
            ++_id;
        }

        return _id;
    }

    inline void setServantProxy(ServantProxy * pServantProxy)
    {
        _servantProxy = pServantProxy;
    }

protected:
	
	int                                   _fd;

	CommunicatorEpoll *                   _communicatorEpoll;

	std::unique_ptr<Transceiver>          _trans;
	
	std::unique_ptr<TC_TimeoutQueueNew<ReqMessage*>> _timeoutQueue;

	uint32_t                              _id;

	ServantProxy *                        _servantProxy;
};

}

#endif

