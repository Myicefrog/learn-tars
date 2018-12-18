#include "AsyncProcThread.h"
#include "Communicator.h"
#include "ServantProxy.h"
#include <iostream>

using namespace std;

namespace tars
{

AsyncProcThread::AsyncProcThread(size_t iQueueCap)
: _terminate(false)
{
     _msgQueue = new ReqInfoQueue(iQueueCap);
}

AsyncProcThread::~AsyncProcThread()
{
    terminate();

    if(_msgQueue)
    {
        delete _msgQueue;
        _msgQueue = NULL;
    }
}

void AsyncProcThread::terminate()
{
    Lock lock(*this);

    _terminate = true;

    notifyAll();
}

void AsyncProcThread::push_back(ReqMessage * msg)
{
    bool bFlag = _msgQueue->push_back(msg);
    
    {
        TC_ThreadLock::Lock lock(*this);
        notify();
    }

    if(!bFlag)
    {
        cout<<"[TARS][AsyncProcThread::push_back] async_queue full."<<endl;
        delete msg;
        msg = NULL;
    }
}

void AsyncProcThread::run()
{
    while (!_terminate)
    {
        ReqMessage * msg = NULL;

        //异步请求回来的响应包处理
        if(_msgQueue->empty())
        {
            TC_ThreadLock::Lock lock(*this);
            timedWait(1000);
        }

        if (_msgQueue->pop_front(msg))
        {
            ServantProxyThreadData * pServantProxyThreadData = ServantProxyThreadData::getData();
            assert(pServantProxyThreadData != NULL);

            try
            {
				std::shared_ptr<ReqMessage> msgPtr(msg);
                msg->callback->onDispatch(msgPtr);
            }
            catch (exception& e)
            {
                cout<<"[TARS][AsyncProcThread exception]:" << e.what() << endl;
            }
            catch (...)
            {
                cout<<"[TARS][AsyncProcThread exception.]" << endl;
            }
        }
    }
}


}
