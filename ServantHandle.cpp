#include "ServantHandle.h"
#include "ServantHelper.h"

extern bool        OpenCoroutine;
extern size_t CoroutineMemSize;
extern uint32_t CoroutineStackSize;

namespace tars
{

ServantHandle::ServantHandle()
{
    
}

ServantHandle::~ServantHandle()
{
    map<string, ServantPtr>::iterator it = _servants.begin();

    while(it != _servants.end())
    {
        try
        {
            it->second->destroy();
        }
        catch(exception &ex)
        {
            //TLOGERROR("[TARS]ServantHandle::destroy error:" << ex.what() << endl);
        }
        catch(...)
        {
            //TLOGERROR("[TARS]ServantHandle::destroy unknown exception error" << endl);
        }
        ++it;
    }

}


void ServantHandle::initialize()
{
    ServantPtr servant = ServantHelperManager::getInstance()->create(_lsPtr->getName());
    _servants[servant->getName()] = servant;

    servant->setHandle(this);
    servant->initialize();

}

void ServantHandle::handle(const TC_EpollServer::tagRecvData &stRecvData)
{

	map<string, ServantPtr>::iterator sit = _servants.find(ServantHelperManager::getInstance()->getAdapterServant(stRecvData.adapter->getName()));

	assert(sit != _servants.end());

	vector<char> buffer;

	sit->second->dispatch(stRecvData.buffer, buffer);

	string response;
	response.resize(buffer.size());
    response.assign(buffer.begin(),buffer.end());

	//_pEpollServer->send(stRecvData.uid, response, stRecvData.ip, stRecvData.port, stRecvData.fd);

	sendResponse(stRecvData.uid, response, stRecvData.ip, stRecvData.port, stRecvData.fd);

}

void ServantHandle::run()
{
	initialize();

	if(!OpenCoroutine)
	{
		cout<<"OpenCoroutine false"<<endl;
		handleImp();
	}
	else
	{
		cout<<"OpenCoroutine right"<<endl;		

		size_t iCoroutineNum = (CoroutineMemSize > CoroutineStackSize) ? (CoroutineMemSize / (CoroutineStackSize * 4)) : 1;

		if(iCoroutineNum < 1)
			iCoroutineNum = 1;

		cout<<"iCoroutineNum is "<<iCoroutineNum<<endl;

		_coroSched = new CoroutineScheduler();
        _coroSched->init(iCoroutineNum, CoroutineStackSize);
        _coroSched->setHandle(this);

        uint32_t iRet1 = _coroSched->createCoroutine(std::bind(&ServantHandle::handleRequest, this));

		cout<<"ServantHandle::run iRet1 is "<<iRet1<<endl;

		ServantProxyThreadData * pSptd = ServantProxyThreadData::getData();

		pSptd->_sched = _coroSched;

		cout<<"_coroSched->tars_run()"<<endl;

        while (!getEpollServer()->isTerminate())
        {
            _coroSched->tars_run();
        }

		cout<<"_coroSched->terminate()"<<endl;
        _coroSched->terminate();

        _coroSched->destroy();

        stopHandle();	    				

	}

}

void ServantHandle::handleRequest()
{
	cout<<"enter ServantHandle::handleRequest()"<<endl;

	bool bYield = false;
	while (!getEpollServer()->isTerminate())
	{
		bool bServerReqEmpty = false;
        
		{
            TC_ThreadLock::Lock lock(_lsPtr->monitor);

           // if (allAdapterIsEmpty() && allFilterIsEmpty())
            {
                if(_coroSched->getResponseCoroSize() > 0)
                {
                    bServerReqEmpty = true;
                }
                else
                {
                    _lsPtr->monitor.timedWait(3000);
                }
            }
        }

        if(bServerReqEmpty)
        {
            _coroSched->yield();

            continue;
        }
	
		bYield = false;

		TC_EpollServer::tagRecvData* recv = NULL;

		TC_EpollServer::BindAdapterPtr& adapter = _lsPtr;

		bool bFlag = true;

		int    iLoop = 100;

		while(bFlag && iLoop > 0)
		{
			--iLoop;
		
			if(adapter->waitForRecvQueue(recv, 0))
			{
				cout<<"ServantHandle::handleRequest::waitForRecvQueue"<<endl;
				bYield = true;

				TC_EpollServer::tagRecvData& stRecvData = *recv;

				stRecvData.adapter = adapter;

				uint32_t iRet = _coroSched->createCoroutine(std::bind(&ServantHandle::handleRecvData, this, recv));

				//handle(*recv);
                
				cout<<"ServantHandle::handleRequest iRet is "<<iRet<<endl;

				if(iRet == 0)
				{
					delete recv;
					recv = NULL;
				} 
				
			}
			else
			{
				bFlag = false;
				bYield = false;
			}
			
	   }

	   if(iLoop == 0)
       		bYield = false;

		if(!bYield)
    	{
    		_coroSched->yield();
    	}
	}
}

void ServantHandle::handleRecvData(TC_EpollServer::tagRecvData *stRecvData)
{
	cout<<"enter ServantHandle::handleRecvData"<<endl;
	handle(*stRecvData);
}

}
