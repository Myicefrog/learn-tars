#include "ServantHandle.h"
#include "ServantHelper.h"
#include "Application.h"

//extern bool        OpenCoroutine;
//extern size_t CoroutineMemSize;
//extern uint32_t CoroutineStackSize;

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
	map<string, TC_EpollServer::BindAdapterPtr>::iterator adpit;

	map<string, TC_EpollServer::BindAdapterPtr>& adapters = _handleGroup->adapters;

	for (adpit = adapters.begin(); adpit != adapters.end(); ++adpit)
	{

    	ServantPtr servant = ServantHelperManager::getInstance()->create(adpit->first);
    	
		_servants[servant->getName()] = servant;
	}

	map<string, ServantPtr>::iterator it = _servants.begin();

	while(it != _servants.end())
	{

    	//servant->setHandle(this);
    	//servant->initialize();
		it->second->setHandle(this);
		it->second->initialize();

		++it;
	}

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

	if(!ServerConfig::OpenCoroutine)
	{
		cout<<"OpenCoroutine false"<<endl;
		handleImp();
	}
	else
	{
		cout<<"OpenCoroutine right"<<endl;		

		size_t iCoroutineNum = (ServerConfig::CoroutineMemSize > ServerConfig::CoroutineStackSize) ? (ServerConfig::CoroutineMemSize / (ServerConfig::CoroutineStackSize * 4)) : 1;

		if(iCoroutineNum < 1)
			iCoroutineNum = 1;

		cout<<"iCoroutineNum is "<<iCoroutineNum<<endl;

		_coroSched = new CoroutineScheduler();
        _coroSched->init(iCoroutineNum, ServerConfig::CoroutineStackSize);
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
            //TC_ThreadLock::Lock lock(_lsPtr->monitor);
            TC_ThreadLock::Lock lock(_handleGroup->monitor);

           // if (allAdapterIsEmpty() && allFilterIsEmpty())
            {
                if(_coroSched->getResponseCoroSize() > 0)
                {
                    bServerReqEmpty = true;
                }
                else
                {
                    //_lsPtr->monitor.timedWait(3000);
                    _handleGroup->monitor.timedWait(3);
                }
            }
        }

		heartbeat();

        if(bServerReqEmpty)
        {
            _coroSched->yield();

            continue;
        }
	
		bYield = false;

		TC_EpollServer::tagRecvData* recv = NULL;

		//TC_EpollServer::BindAdapterPtr& adapter = _lsPtr;

		map<string, TC_EpollServer::BindAdapterPtr>& adapters = _handleGroup->adapters;

		for (map<string, TC_EpollServer::BindAdapterPtr>::iterator it = adapters.begin(); it != adapters.end(); ++it)
        {

			TC_EpollServer::BindAdapterPtr& adapter = it->second;

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

					if(stRecvData.isClosed)
					{
						cout<<"give info to real buisiness to close"<<endl;
						delete recv;
						recv = NULL;
					}
					else
					{

						uint32_t iRet = _coroSched->createCoroutine(std::bind(&ServantHandle::handleRecvData, this, recv));

						if(iRet == 0)
						{
							delete recv;
							recv = NULL;
						} 
					}
				}
				else
				{
					bFlag = false;
					bYield = false;
				}
			
	   		}

	   		if(iLoop == 0)
			{
       			bYield = false;
			}
		}

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

void ServantHandle::heartbeat()
{
	struct timeval tv; 
    ::gettimeofday(&tv, NULL);
    
	time_t fcur = tv.tv_sec;

	map<string, TC_EpollServer::BindAdapterPtr>::iterator it;

    map<string, TC_EpollServer::BindAdapterPtr>& adapters = _handleGroup->adapters;

	for (it = adapters.begin(); it != adapters.end(); ++it)
	{
		if (abs(fcur - it->second->getHeartBeatTime()) > HEART_BEAT_INTERVAL)
		{
			it->second->setHeartBeatTime(fcur);
			TARS_KEEPALIVE(it->second->getName());
		}

	}
}

}
