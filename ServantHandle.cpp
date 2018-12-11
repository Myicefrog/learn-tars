#include "ServantHandle.h"
#include "ServantHelper.h"

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


}
