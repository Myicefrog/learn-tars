#include "ServantProxy.h"
#include "ObjectProxy.h"

namespace tars
{

TC_ThreadMutex ServantProxyThreadData::_mutex;

pthread_key_t ServantProxyThreadData::_key = 0;

SeqManager * ServantProxyThreadData::_pSeq = new SeqManager(2048);

/////////////////////////////////////////////////////////////////////////////////////

SeqManager::SeqManager(size_t iNum)
{
    assert(iNum < MAX_UNSIGN_SHORT);
    assert(iNum > 0);
    _p = NULL;
    _p = new SeqInfo[iNum];
    assert(_p);

    //0xff表示结束
    _free = 0;
    _freeTail = iNum -1;

    for(uint16_t i=0;i<(uint16_t)iNum;i++)
    {
        _p[i].free = true;
        _p[i].next = i+1;
    }
    _p[iNum-1].next = MAX_UNSIGN_SHORT;
    _num = iNum;
}

uint16_t SeqManager::get()
{
    assert(_free != MAX_UNSIGN_SHORT);

    assert(_p[_free].free);

    uint16_t buf = _free;

    if(_free == _freeTail)
    {
        assert(_p[buf].next == MAX_UNSIGN_SHORT);
        _freeTail = MAX_UNSIGN_SHORT;
    }

    _free = _p[buf].next;

    _p[buf].free = false;

    return buf;
}

void SeqManager::del(uint16_t iSeq)
{
    assert(iSeq < _num);
    assert(!_p[iSeq].free);

    _p[iSeq].next = MAX_UNSIGN_SHORT;
    if(MAX_UNSIGN_SHORT == _freeTail)
    {
        _free = iSeq;
    }
    else
    {
        _p[_freeTail].next = iSeq;
    }
    _p[iSeq].free = true;
    _freeTail = iSeq;
} 

////////////////////////////////////////////////////////////////////////////////


ServantProxyThreadData::ServantProxyThreadData()
: _queueInit(false)
, _reqQNo(0)
, _netSeq(0)
, _netThreadSeq(-1)
, _objectProxyNum(0)
, _objectProxy(NULL)
, _sched(NULL)
{
}

ServantProxyThreadData::~ServantProxyThreadData()
{
}

void ServantProxyThreadData::destructor(void* p)
{
    ServantProxyThreadData * pSptd = (ServantProxyThreadData*)p;
    if(pSptd)
    {
        delete pSptd;
        pSptd = NULL;
    }
}

ServantProxyThreadData * ServantProxyThreadData::getData()
{
    if(_key == 0)
    {
        TC_LockT<TC_ThreadMutex> lock(_mutex);
        if(_key == 0)
        {
            int iRet = ::pthread_key_create(&_key, ServantProxyThreadData::destructor);

            if (iRet != 0)
            {
                //TLOGERROR("[TARS][ServantProxyThreadData pthread_key_create fail:" << errno << ":" << strerror(errno) << "]" << endl);
                return NULL;
            }
        }
    }

    ServantProxyThreadData * pSptd = (ServantProxyThreadData*)pthread_getspecific(_key);

    if(!pSptd)
    {
        TC_LockT<TC_ThreadMutex> lock(_mutex);

        pSptd = new ServantProxyThreadData();
        pSptd->_reqQNo = _pSeq->get();

        int iRet = pthread_setspecific(_key, (void *)pSptd);

        assert(iRet == 0);
    }

    return pSptd;
}


ServantProxyCallback::ServantProxyCallback()
: _bNetThreadProcess(false)
{
}

ServantProxy::ServantProxy(Communicator * pCommunicator, ObjectProxy ** ppObjectProxy, size_t iClientThreadNum)
: _communicator(pCommunicator)
, _objectProxy(ppObjectProxy)
, _objectProxyNum(iClientThreadNum)
, _queueSize(1000)
{

    for(size_t i = 0;i < _objectProxyNum; ++i)
    {
       (*(_objectProxy + i))->setServantProxy(this);
    }

}

ServantProxy::~ServantProxy()
{
}

void ServantProxy::tars_invoke(const string& sFuncName,
                             const string& request,
                             string& resp)
{
	ReqMessage * msg = new ReqMessage();

	msg->init(ReqMessage::SYNC_CALL);

	msg->request = sFuncName + ":" + request;

	invoke(msg);

	resp = msg->response;

	delete msg;
	
	msg = NULL;

}

void ServantProxy::tars_invoke_async(const string& sFuncName,
                            const string& request,
                            const ServantProxyCallbackPtr& callback)
{

	ReqMessage * msg = new ReqMessage();

	msg->init(ReqMessage::ASYNC_CALL);

	msg->callback = callback;

	msg->request = sFuncName + ":" + request;

	invoke(msg);

}

void ServantProxy::invoke(ReqMessage * msg, bool bCoroAsync)
{

	ServantProxyThreadData * pSptd = ServantProxyThreadData::getData();

	ObjectProxy * pObjProxy = NULL;
    ReqInfoQueue * pReqQ    = NULL;


	selectNetThreadInfo(pSptd,pObjProxy,pReqQ);

    struct timeval tv; 
    ::gettimeofday(&tv, NULL);
    msg->iBeginTime = tv.tv_sec * (int64_t)1000 + tv.tv_usec/1000;

	msg->pObjectProxy = pObjProxy;

    if(msg->eType == ReqMessage::SYNC_CALL)
    {
        msg->bMonitorFin = false;

        msg->pMonitor = new ReqMonitor;
    }

    //通知网络线程
    bool bEmpty = false;
    bool bSync  = (msg->eType == ReqMessage::SYNC_CALL);

    if(!pReqQ->push_back(msg,bEmpty))
    {
        delete msg;
        msg = NULL;

        pObjProxy->getCommunicatorEpoll()->notify(pSptd->_reqQNo, pReqQ);

        cout<<"client queue full"<<endl;
    }

	pObjProxy->getCommunicatorEpoll()->notify(pSptd->_reqQNo, pReqQ);


	if(bSync)
	{
		if(!msg->bMonitorFin)
        {
        	TC_ThreadLock::Lock lock(*(msg->pMonitor));

            //等待直到网络线程通知过来
            if(!msg->bMonitorFin)
            {
            	msg->pMonitor->wait();
            }
        }
	}


}

//选取一个网络线程对应的信息
void ServantProxy::selectNetThreadInfo(ServantProxyThreadData * pSptd, ObjectProxy * & pObjProxy, ReqInfoQueue * & pReqQ)
{
    //指针为空 就new一个
    if(!pSptd->_queueInit)
    {
        for(size_t i=0;i<_objectProxyNum;++i)
        {
            pSptd->_reqQueue[i] = new ReqInfoQueue(_queueSize);
        }
        pSptd->_objectProxyNum = _objectProxyNum;
        pSptd->_objectProxy    = _objectProxy;
        pSptd->_queueInit      = true;
    }

    if(_objectProxyNum == 1)
    {
        pObjProxy = *_objectProxy;
        pReqQ     = pSptd->_reqQueue[0];
    }
    else
    {
		cout<<"pSptd->_netThreadSeq is "<<pSptd->_netThreadSeq<<endl;
        if(pSptd->_netThreadSeq >= 0)
        {
            //网络线程发起的请求
            assert(pSptd->_netThreadSeq < static_cast<int>(_objectProxyNum));

            pObjProxy = *(_objectProxy + pSptd->_netThreadSeq);
            pReqQ     = pSptd->_reqQueue[pSptd->_netThreadSeq];
        }
        else
        {
            //用线程的私有数据来保存选到的seq
            pObjProxy = *(_objectProxy + pSptd->_netSeq);
            pReqQ     = pSptd->_reqQueue[pSptd->_netSeq];
            pSptd->_netSeq++;

			cout<<"pSptd->_netSeq is "<<pSptd->_netSeq<<endl;

            if(pSptd->_netSeq == _objectProxyNum)
                pSptd->_netSeq = 0;
        }
    }
}

}
