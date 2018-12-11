#ifndef __TARS_OBJECT_PROXY_H_
#define __TARS_OBJECT_PROXY_H_

#include "Message.h"
#include "CommunicatorEpoll.h"

namespace tars
{
class ObjectProxy
{
public:
	
	ObjectProxy(CommunicatorEpoll * pCommunicatorEpoll);

	~ObjectProxy();

	void invoke(ReqMessage* msg);

	inline CommunicatorEpoll * getCommunicatorEpoll()
    {
        return _communicatorEpoll;
    }

protected:
	
	int _fd;

	CommunicatorEpoll *                   _communicatorEpoll;

};

}

#endif
