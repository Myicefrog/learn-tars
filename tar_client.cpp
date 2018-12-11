#include <iostream>
#include <string>
#include "CommunicatorEpoll.h"
#include "ObjectProxy.h"

using namespace std;
using namespace tars;

int main()
{

	CommunicatorEpoll* _communicatorEpoll =  new CommunicatorEpoll();
	
	_communicatorEpoll->start();

//////////////////////////////////////////////////////////////////////
	
	ReqMessage * msg = new ReqMessage();

	msg->init(ReqMessage::SYNC_CALL);

	msg->request = "hello";

	msg->pMonitor = new ReqMonitor;

	ObjectProxy * pObjectProxy = new ObjectProxy(_communicatorEpoll);

	msg->pObjectProxy = pObjectProxy;

	ReqInfoQueue * pReqQ    = new ReqInfoQueue(1000);

	bool bEmpty = false;	

	pReqQ->push_back(msg,bEmpty);

	_communicatorEpoll->notify(0, pReqQ);

/////////////////////////////////////////////////////////////////////

	while(true)
	{
	
	}

	return 0;
}
