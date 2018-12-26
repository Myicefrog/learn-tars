#ifndef __HELLO_H_
#define __HELLO_H_

#include <map>
#include <memory>
#include <algorithm>
#include "ServantProxy.h"
#include "Servant.h"

using namespace std;

class HelloPrxCallback: public tars::ServantProxyCallback
{
public:

    virtual ~HelloPrxCallback(){}

    virtual void callback_testHello(const std::string& sRsp)
    {   
        cout<<"callback_testHello override incorrect"<<endl; 
    }   
    
public:

    virtual int onDispatch(tars::ReqMessagePtr msg)
    {   
        string sRsp = msg->response;
    
        callback_testHello(sRsp);

        return 0;
    }   
};

typedef shared_ptr<HelloPrxCallback> HelloPrxCallbackPtr;

class HelloProxy : public tars::ServantProxy
{
public:

	int testHello(const std::string& sReq,std::string &sRsp)
	{
		string method = "testHello";
		tars_invoke(method, sReq, sRsp);
		
		return 1;
	}

    void async_testHello(HelloPrxCallbackPtr callback,const std::string &sReq)
    {
		string method = "testHello";
        tars_invoke_async(method, sReq, callback);
    }


};

typedef shared_ptr<HelloProxy> HelloPrx;

class Hello : public tars::Servant
{
public:
	
	virtual ~Hello(){}

	virtual int testHello(const std::string & sReq,vector<char> &buffer) = 0;
	virtual int test(const std::string & sReq,vector<char> &buffer) = 0;

	int onDispatch(const string &request, vector<char> &buffer)
	{
		static ::std::string __TestApp__Hello_all[]=
		{
			"test",
			"testHello"
		};

		size_t pos = request.find(":");

		string requestBody = request.substr(pos + 1);

		size_t posMethod = requestBody.find(":");

		string method = requestBody.substr(0, posMethod);

		pair<string*, string*> r = equal_range(__TestApp__Hello_all, __TestApp__Hello_all+2, method);

		if(r.first == r.second)
		{
			cout<<"do not find right method "<<method<<endl;
			return -3;
		}

		switch(r.first - __TestApp__Hello_all)
		{

			case 0:
			{
				int ret = test(request, buffer);
				return ret;
			}
			case 1:
			{
				int ret = testHello(request, buffer);
				return ret; 
			}
		}

		return 0;
	}


};

#endif
