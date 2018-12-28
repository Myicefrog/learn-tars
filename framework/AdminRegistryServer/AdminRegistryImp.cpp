#include "AdminRegistryImp.h"
#include "Application.h"
#include "AdminRegistryServer.h"
#include "Node.h"

void AdminRegistryImp::initialize()
{
}

int AdminRegistryImp::startServer( const std::string & sReq,vector<char> &buffer )
{
    size_t pos = sReq.find(":");
    string requestId = sReq.substr(0,pos);


	string host = "127.0.0.1";

	uint16_t port = 19385;

	ServantProxy* prx = NULL;

	g_app.getCommunicator()->stringToProxy(host, port, &prx);

	NodeProxy* nodePrx = (NodeProxy*)(prx);

	string response = "right now";

	if(nodePrx)
	{
		NodePrxCallback* call = new StartServerCallbackImp("", "", "");	
		shared_ptr<NodePrxCallback> callback(call);

		nodePrx->async_startServer(callback, response);	
	
		//response = "successfull";
	}
	
	response = requestId + ":" + response;
	buffer.assign(response.begin(),response.end());

	return 1;

}
