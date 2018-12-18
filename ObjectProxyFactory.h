#ifndef __TARS_OBJECT_PROXY_FACTORY_H_
#define __TARS_OBJECT_PROXY_FACTORY_H_

#include "Communicator.h"
#include "ObjectProxy.h"
#include <vector>

namespace tars
{

class ObjectProxyFactory
{
public:

	ObjectProxyFactory(CommunicatorEpoll * pCommunicatorEpoll);

	ObjectProxy * getObjectProxy(const string& ip, const uint16_t& port);

	 ~ObjectProxyFactory();

    inline size_t getObjNum()
    {
        return _objNum;
    }

    inline ObjectProxy * getObjectProxy(size_t iNum)
    {
        assert(iNum < _objNum);
        return _vObjectProxys[iNum];
    }

private:

	CommunicatorEpoll *         _communicatorEpoll;

	map<string,ObjectProxy*>    _objectProxys;

	vector<ObjectProxy *>       _vObjectProxys;

	size_t                      _objNum;

};


}
#endif
