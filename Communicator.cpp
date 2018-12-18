#include "Communicator.h"

namespace tars
{

Communicator::Communicator()
: _initialized(false)
, _terminating(false)
, _clientThreadNum(1)
{
    memset(_communicatorEpoll,0,sizeof(_communicatorEpoll));
}


Communicator::~Communicator()
{
    terminate();
}


void Communicator::initialize()
{

    if (_initialized)
        return;

    _initialized = true;


	_clientThreadNum = 3;

    for(size_t i = 0; i < _clientThreadNum; ++i)
    {
        _communicatorEpoll[i] = new CommunicatorEpoll(this, i);
        _communicatorEpoll[i]->start();
    }

}

void Communicator::terminate()
{
    {
        _terminating = true;
    }

    if(_initialized)
    {
        for(size_t i = 0; i < _clientThreadNum; ++i)
        {
            _communicatorEpoll[i]->terminate();
            _communicatorEpoll[i]->getThreadControl().join();
        }

    }

}

ServantProxy * Communicator::getServantProxy(const string& ip, const uint16_t& port)
{
    Communicator::initialize();

	ObjectProxy ** ppObjectProxy = new ObjectProxy * [getClientThreadNum()];

    for(size_t i = 0; i < getClientThreadNum(); ++i)
    {
        ppObjectProxy[i] = getCommunicatorEpoll(i)->getObjectProxy(ip, port);
    }

	ServantProxy* sp = new ServantProxy(this, ppObjectProxy, getClientThreadNum());

// 这里使用智能指针就会core down
//	shared_ptr<ServantProxy> sp1(sp); 
//	shared_ptr<ServantProxy> sp1 = make_shared<ServantProxy>(this, ppObjectProxy, getClientThreadNum()); 
/*
	ServantProxy* check = sp1.get();
	cout<<"check1 is "<<check<<endl;

	cout<<"check plus "<<sp<<endl;

	string req = "hello";
	string res;
	string method = "123";

	check->tars_invoke(method,req,res);

	cout<<"res1 "<<res<<endl;
	//return sp;	

	//return sp1.get();	
	return check;	
*/

	return sp;

}

void Communicator::stringToProxy(const string& ip, const uint16_t& port, ServantProxy** proxy)
{   
	*proxy = getServantProxy(ip, port);

/*
	cout<<"check2 is "<<(*proxy)<<endl;

	string res;
	(*proxy)->tars_invoke("123","hello",res);
	cout<<"res2 "<<res<<endl;
*/

}

}
