#include <iostream>
#include <string>
#include "Communicator.h"
#include "Hello.h"

using namespace std;
using namespace tars;

class HelloCallBack: public HelloPrxCallback
{
public:

	HelloCallBack(){}

	virtual ~HelloCallBack(){}

    virtual void callback_testHello(const std::string& sRsp)
    {
        cout<<"callback_testHello :"<< "sRsp:" << sRsp <<endl; 
    }	
};

int main()
{
	Communicator comm;

	ServantProxy* prx = NULL;	

	string host = "127.0.0.1";

	uint16_t port = 9877;

	comm.stringToProxy(host, port, &prx);

	HelloProxy* prx1 = (HelloProxy*)(prx);

	string sReq("hello world");

	HelloPrxCallback* cbptr = new HelloCallBack();

	shared_ptr<HelloPrxCallback> cb(cbptr); 	

	prx1->async_testHello(cb, sReq);

	getchar();

	return 0;
}
