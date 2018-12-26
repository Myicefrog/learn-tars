#ifndef __NODE_H_
#define __NODE_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

#include "ServantProxy.h"
#include "Servant.h"

namespace tars
{

/* callback of async proxy for client */
class NodePrxCallback: public tars::ServantProxyCallback
{
public:

	virtual ~NodePrxCallback(){}

	virtual void callback_startServer(int ret,  const std::string& result)
	{
		cout<<"callback_startServer() override incorrect."<<endl;
	}

public:

	virtual int onDispatch(tars::ReqMessagePtr msg)
	{
		string sRsp = msg->response;
	
		int ret = 0;
		
		callback_startServer(ret, sRsp);

		return ret;
	}
};

typedef shared_ptr<NodePrxCallback> NodePrxCallbackPtr;

/* proxy for client */
class NodeProxy : public tars::ServantProxy
{
public:

	int startServer(const std::string& sReq,std::string &sRsp)
	{
		string method = "startServer";

		tars_invoke(method, sReq, sRsp);

		return 1;
	}

	void async_startServer(NodePrxCallbackPtr callback,const std::string &sReq)
	{
		string method = "startServer";
		tars_invoke_async(method, sReq, callback);
	}

};

typedef shared_ptr<NodeProxy> NodePrx;

/* servant for server */
class Node : public tars::Servant
{
public:

	virtual ~Node(){}

	virtual int startServer(const std::string & sReq,vector<char> &buffer) = 0;

	int onDispatch(const string &request, vector<char> &buffer)
	{
		cout<<"class Node request is "<<request<<endl;
		
		static ::std::string __tars__Node_all[]=
		{
			"startServer",
			"stopServer"
		};

		size_t pos = request.find(":");

        string requestBody = request.substr(pos + 1); 

        size_t posMethod = requestBody.find(":");

        string method = requestBody.substr(0, posMethod);
		
		pair<string*, string*> r = equal_range(__tars__Node_all, __tars__Node_all+2, method);

		if(r.first == r.second)
        {   
            cout<<"Node do not find right method "<<method<<endl;
            return -3; 
        }   


		switch(r.first - __tars__Node_all)
		{
			case 0:
			{
				int ret = startServer(request, buffer);
				return ret;
			
			}
		}

		return 0;
	}

};


}

#endif

