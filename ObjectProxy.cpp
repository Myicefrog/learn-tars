#include "ObjectProxy.h"

namespace tars
{

ObjectProxy::ObjectProxy(CommunicatorEpoll * pCommunicatorEpoll, const string& host, const uint16_t& port)
:_communicatorEpoll(pCommunicatorEpoll)
,_id(0)
{
	_timeoutQueue.reset(new TC_TimeoutQueueNew<ReqMessage*>());	

	_trans.reset(new Transceiver(this, host, port));
}

ObjectProxy::~ObjectProxy()
{
}

void ObjectProxy::invoke(ReqMessage * msg)
{
//_endpointManger->selectAdapterProxy
//pAdapterProxy = getNextValidProxy();
//adapterProxy->checkActive(true);
//_trans->reconnect();

	_trans->connect();

	msg->iRequestId = generateId();
	
//序列化msg中有用的内容，传递给服务端
	msg->sReqData = to_string(msg->iRequestId) + ":" + msg->request;		
	
	if(_timeoutQueue->sendListEmpty() && _trans->sendRequest(msg->sReqData.c_str(),msg->sReqData.size()) != Transceiver::eRetError)
	{
		bool bFlag = _timeoutQueue->push(msg, msg->iRequestId, 1000 + msg->iBeginTime);
		if(!bFlag)
		{
			cout<<"_timeoutQueue->push fail"<<endl;
		}
		cout<<"_trans->sendRequest"<<endl;
	}
}

//void AdapterProxy::finishInvoke(ResponsePacket & rsp)
void ObjectProxy::finishInvoke(const string& rsp)
{
	ReqMessage * msg = NULL;

	size_t pos = rsp.find(":");	
 
	string requestId = rsp.substr(0,pos);
	bool retErase = _timeoutQueue->erase(atoi(requestId.c_str()), msg);

	if (!retErase)
	{
		cout<<"get msg from timeoutQueue fail"<<endl;
		return ;
	}

	msg->response = rsp;

	finishInvoke(msg);
}

void ObjectProxy::finishInvoke(ReqMessage * msg)
{
	TC_ThreadLock::Lock sync(*(msg->pMonitor));
    msg->pMonitor->notify();
    msg->bMonitorFin = true;	
	return ;
}

}
