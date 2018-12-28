#ifndef __ADMINREG_H_
#define __ADMINREG_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

#include "ServantProxy.h"
#include "Servant.h"
#include "Node.h"

class AdminRegPrxCallback: public tars::ServantProxyCallback
{
public:
    virtual ~AdminRegPrxCallback(){}
};

typedef shared_ptr<AdminRegPrxCallback> AdminRegPrxCallbackPtr;

/* proxy for client */
class AdminRegProxy : public tars::ServantProxy
{
public:

    int startServer(const std::string& sReq,std::string &sRsp)
    {   
        string method = "startServer";

        tars_invoke(method, sReq, sRsp);

        return 1;
    }   

    void async_startServer(AdminRegPrxCallbackPtr callback,const std::string &sReq)
    {   
        string method = "startServer";
        tars_invoke_async(method, sReq, callback);
    }   
};

typedef shared_ptr<AdminRegProxy> AdminRegPrx;

/* servant for server */
class AdminReg : public tars::Servant
{
public:

	virtual ~AdminReg(){}

	virtual int startServer(const std::string & sReq,vector<char> &buffer) = 0;

    int onDispatch(const string &request, vector<char> &buffer)
    {
        cout<<"class AdminReg request is "<<request<<endl;

        static ::std::string __tars__AdminReg_all[]=
        {
            "startServer",
            "stopServer"
        };

        size_t pos = request.find(":");

        string requestBody = request.substr(pos + 1);

        size_t posMethod = requestBody.find(":");

        string method = requestBody.substr(0, posMethod);

        pair<string*, string*> r = equal_range(__tars__AdminReg_all, __tars__AdminReg_all+2, method);

        if(r.first == r.second)
        {
            cout<<"AdminReg do not find right method "<<method<<endl;
            return -3;
        }

        switch(r.first - __tars__AdminReg_all)
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

#endif
