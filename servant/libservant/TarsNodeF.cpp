#include "TarsNodeF.h"
#include "NodeF.h"
#include "Communicator.h"

namespace tars
{

void TarsNodeFHelper::setNodeInfo(const CommunicatorPtr &comm, const string &obj, const string &app, const string &server)
{
    _comm           = comm;
    if(!obj.empty())
    {

		size_t pos = obj.find(":");

		string host = obj.substr(0,pos);

		string portstr = obj.substr(pos+1);

		uint16_t port = (uint16_t)atoi(portstr.c_str());		

		cout<<"setNodeInfo host is "<<host<<" port is "<<port<<endl;

		ServantProxy* prx = NULL;

		_comm->stringToProxy(host, port, &prx);

		_nodePrx = (ServerFProxy*)(prx);

		//ServerFProxy* prx1 = (ServerFProxy*)(prx);

		//shared_ptr<ServerFProxy> _nodeTmp(prx1);

		//_nodePrx = _nodeTmp;

    }

    _si.application = app;
    _si.serverName  = server;
    _si.pid         = getpid();
}

void TarsNodeFHelper::keepAlive(const string &adapter)
{
        try
        {
            if(_nodePrx)
            {
				cout<<"_nodePrx is not null"<<endl;
                set<string> s;
                {
                    TC_LockT<TC_ThreadMutex> lock(*this);

                    _adapterSet.insert(adapter);

                    if(adapter != "AdminAdapter")
                    {
                        return;
                    }
                    s.swap(_adapterSet);
                }
                ServerInfo si   = _si;
                set<string>::const_iterator it = s.begin();
                while(it != s.end())
                {
                    si.adapter      = *it;
                    _nodePrx->async_keepAlive(NULL,si);
                    ++it;
                }

            }
        }
        catch(exception &ex)
        {
                //TLOGERROR("TarsNodeFHelper::keepAlive error:" << ex.what() << endl);
        }
        catch(...)
        {
                //TLOGERROR("TarsNodeFHelper::keepAlive unknown error" << endl);
        }
}


}
