#ifndef __AMIN_REGISTRY_H__
#define __AMIN_REGISTRY_H__

#include "AdminReg.h"

using namespace tars;

class AdminRegistryImp: public AdminReg
{
public:

	AdminRegistryImp(){};

	virtual void initialize();

	virtual void destroy() {};

public:

	virtual int startServer( const std::string & sReq,vector<char> &buffer );


};

class StartServerCallbackImp: public NodePrxCallback
{
public:
    StartServerCallbackImp(string application, string serverName, string nodeName )
    : _application(application)
    , _serverName(serverName)
    , _nodeName(nodeName)
    {
    }

    virtual void callback_startServer(int ret,  const std::string& result)
	{
		cout<<"StartServerCallbackImp callback_startServer"<<endl;
	}

private:
    string _application;
    string _serverName;
    string _nodeName;
};


#endif
