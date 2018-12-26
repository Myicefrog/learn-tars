#include "ObjectProxyFactory.h"
#include <string>

namespace tars
{

ObjectProxyFactory::ObjectProxyFactory(CommunicatorEpoll * pCommunicatorEpoll)
: _communicatorEpoll(pCommunicatorEpoll)
, _objNum(0)
{
}


ObjectProxyFactory::~ObjectProxyFactory()
{
    for(size_t i = 0; i < _vObjectProxys.size(); i++)
    {
        if(_vObjectProxys[i])
        {
            delete _vObjectProxys[i];
            _vObjectProxys[i] = NULL;
        }
    }
}

ObjectProxy * ObjectProxyFactory::getObjectProxy(const string& ip, const uint16_t& port)
{
    string tmpObjName = ip + ":" + to_string(port);
    map<string, ObjectProxy*>::iterator it = _objectProxys.find(tmpObjName);
    if(it != _objectProxys.end())
    {
        return it->second;
    }

    ObjectProxy * pObjectProxy = new ObjectProxy(_communicatorEpoll, ip, port);

    //pObjectProxy->initialize();

    _objectProxys[tmpObjName] = pObjectProxy;
    //_objectProxys.insert(make_pair<string,ObjectProxy*>(tmpObjName,pObjectProxy));

    _vObjectProxys.push_back(pObjectProxy);

    _objNum++;

    return pObjectProxy;
}


}
