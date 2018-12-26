#ifndef __TARS_COMMUNICATOR_H_
#define __TARS_COMMUNICATOR_H_

#include <memory>

#include "tc_thread.h"
#include "Global.h"

#include "ServantProxy.h"
#include "ObjectProxyFactory.h"
#include "AsyncProcThread.h"
#include "CommunicatorEpoll.h"

using namespace std;

namespace tars
{

class Communicator
{
public:

	Communicator();

	~Communicator();

/*
    template<class T> void stringToProxy(const string& ip, const uint16_t& port, T& proxy)
    {
        ServantProxy * pServantProxy = getServantProxy(ip, port);
		proxy = (typename T::element_type*)(pServantProxy);

		//proxy = shared_ptr<typename T::element_type>(pServantProxy);

    }
*/

	void  stringToProxy(const string& ip, const uint16_t& port, ServantProxy** proxy);

	inline size_t getClientThreadNum()
    {
        return _clientThreadNum;
    }


    inline CommunicatorEpoll * getCommunicatorEpoll(size_t iNum)
    {
        assert(iNum<_clientThreadNum);
        return _communicatorEpoll[iNum];
    }


	void terminate();

	ServantProxy * getServantProxy(const string& ip, const uint16_t& port);

protected:

	void initialize();

	bool isTerminating();

	ServantProxyFactory * servantProxyFactory();


	friend class AdapterProxy;

    friend class ServantProxy;

    friend class ObjectProxy;

    friend class ServantProxyFactory;

    friend class ObjectProxyFactory;

    friend class AsyncProcThread;

    friend class CommunicatorEpoll;

protected:

	bool  _initialized;

	bool  _terminating;

	//ServantProxyFactoryPtr _servantProxyFactory;

	CommunicatorEpoll *    _communicatorEpoll[64];

	size_t                 _clientThreadNum;
};


}
#endif
