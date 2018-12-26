#include <iostream>
#include <string>
#include "CommunicatorEpoll.h"
#include "ObjectProxy.h"

using namespace std;
using namespace tars;

int main()
{

	CommunicatorEpoll* _communicatorEpoll =  new CommunicatorEpoll(3);
	
	_communicatorEpoll->start();

//////////////////////////////////////////////////////////////////////
	
	ReqMessage * msg = new ReqMessage();

	msg->init(ReqMessage::SYNC_CALL);

	msg->request = "testHello:hello";

	msg->pMonitor = new ReqMonitor;

	string host = "127.0.0.1";
    uint16_t port = 9877;

	ObjectProxy * pObjectProxy = new ObjectProxy(_communicatorEpoll, host, port);

	msg->pObjectProxy = pObjectProxy;

	struct timeval tv;
	::gettimeofday(&tv, NULL);
    msg->iBeginTime = tv.tv_sec * (int64_t)1000 + tv.tv_usec/1000;

	msg->bMonitorFin = false;

	msg->pMonitor = new ReqMonitor;

/////////////////////////////////////////////////////////////////////

	ReqInfoQueue * pReqQ    = new ReqInfoQueue(1000);

	bool bEmpty = false;	

	pReqQ->push_back(msg,bEmpty);

	_communicatorEpoll->notify(0, pReqQ);

/////////////////////////////////////////////////////////////////////

	if(!msg->bMonitorFin)
    {
    	TC_ThreadLock::Lock lock(*(msg->pMonitor));

        //等待直到网络线程通知过来
        if(!msg->bMonitorFin)
        {
        	msg->pMonitor->wait();
        }
	}

	cout<<"msg->response is "<<msg->response<<endl;

	getchar();	

	return 0;
}
