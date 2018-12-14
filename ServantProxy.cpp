#include "ServantProxy.h"

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
: _reqQNo(0)
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

}
