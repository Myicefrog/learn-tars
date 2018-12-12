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
			msg->eStatus = ReqMessage::REQ_EXC;
			
			finishInvoke(msg);
		}
		cout<<"_trans->sendRequest"<<endl;
	}
	else
	{
		bool bFlag = _timeoutQueue->push(msg,msg->iRequestId, 1000+msg->iBeginTime, false);
        if(!bFlag)
		{
			msg->eStatus = ReqMessage::REQ_EXC;
			finishInvoke(msg);
		}	
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
	if(msg->eType == ReqMessage::SYNC_CALL)
	{
		TC_ThreadLock::Lock sync(*(msg->pMonitor));
    	msg->pMonitor->notify();
    	msg->bMonitorFin = true;	
		return ;
	}

	if(msg->eType == ReqMessage::ASYNC_CALL)
	{
		getCommunicatorEpoll()->pushAsyncThreadQueue(msg);
	}
}

void ObjectProxy::doInvoke()
{
	if(_timeoutQueue->sendListEmpty())
	{
		return ;
	}

	while(!_timeoutQueue->sendListEmpty())
	{
		ReqMessage * msg = NULL;
		
		_timeoutQueue->getSend(msg);
		
		int iRet = _trans->sendRequest(msg->sReqData.c_str(), msg->sReqData.size());

		if(iRet == Transceiver::eRetError)
		{
			cout<<"doInvoke send fail"<<endl;
			return;
		}

		if(iRet == Transceiver::eRetFull)
		{
			return;
		}
	}

}

}
