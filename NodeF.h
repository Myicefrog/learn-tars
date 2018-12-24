#ifndef __NODEF_H_
#define __NODEF_H_

#include <map>
#include <memory>
#include <algorithm>
#include <string>
#include "ServantProxy.h"
#include "Servant.h"

using namespace std;

namespace tars
{

struct ServerInfo
{
    public:
        static string className()
        {
            return "tars.ServerInfo";
        }
        static string MD5()
        {
            return "34368b1d85d52126f8efa6e27af8f151";
        }
        ServerInfo()
        :application(""),serverName(""),pid(0),adapter("")
        {
        }
        void resetDefautlt()
        {
            application = "";
            serverName = "";
            pid = 0;
            adapter = "";
        }

    public:
        std::string application;
        std::string serverName;
        int pid;
        std::string adapter;
};

class ServerFPrxCallback: public tars::ServantProxyCallback
{
public:

	virtual ~ServerFPrxCallback(){}

    virtual void callback_keepAlive(const std::string& sRsp)
    { 
		cout<<"callback_keepAlive() override incorrect."<<endl;
	}

	virtual void callback_keepAlive(int ret)
	{
		cout<<"callback_keepAlive() override incorrect."<<endl;
	}

public:

    virtual int onDispatch(tars::ReqMessagePtr msg)
    {   
        string sRsp = msg->response;
    
        callback_keepAlive(sRsp);

        return 0;
    }   	

};

typedef shared_ptr<ServerFPrxCallback> ServerFPrxCallbackPtr;


class ServerFProxy : public tars::ServantProxy
{
public:

	int keepAlive(const std::string& sReq,std::string &sRsp)
	{
		string method = "keepAlive";
		
		tars_invoke(method, sReq, sRsp);

		return 1;
	}

	void async_keepAlive(ServerFPrxCallbackPtr callback, const std::string &sReq)
	{
		string method = "keepAlive";

		tars_invoke_async(method, sReq, callback);
	}

	void async_keepAlive(ServerFPrxCallbackPtr callback, const tars::ServerInfo &serverInfo)
	{
		string method = "keepAlive";

		string sReq = serverInfo.application + ":" + serverInfo.serverName + ":"
						+ to_string(serverInfo.pid) + ":" + serverInfo.adapter; 

		cout<<"async_keepAlive is "<<sReq<<endl;

		tars_invoke_async(method, sReq, callback);
	}
};

typedef shared_ptr<ServerFProxy> ServerFPrx;


/* servant for server */
class ServerF : public tars::Servant
{
public:

	virtual ~ServerF(){}

	virtual int keepAlive( const std::string & sReq,vector<char> &buffer ) = 0;

public:

    int onDispatch(const string &request, vector<char> &buffer)
	{
		static ::std::string __tars__ServerF_all[]=
        {
        	"keepAlive",
            "reportVersion"
        };

        size_t pos = request.find(":");

        string requestBody = request.substr(pos + 1); 

        size_t posMethod = requestBody.find(":");

        string method = requestBody.substr(0, posMethod);


        pair<string*, string*> r = equal_range(__tars__ServerF_all, __tars__ServerF_all+2, method);
        if(r.first == r.second)
		{
			cout<<"do not find right method "<<method<<endl;
            return -3;
		}

        switch(r.first - __tars__ServerF_all)
        {
			case 0:
			{
				int ret = keepAlive(request,buffer);
				return ret;
			}
			case 1:
			{
				cout<<"do not have reportVersion yet"<<endl;
				return -3;
			}
		}
		return 0;

	}	

};

}
#endif
