#ifndef __HELLO_H_
#define __HELLO_H_

using namespace std;
#include "ServantProxy.h"
#include "Servant.h"
#include <memory>

class HelloProxy : public tars::ServantProxy
{
public:

	int testHello(const std::string& sReq,std::string &sRsp)
	{
		string method = "testHello";
		tars_invoke(method, sReq, sRsp);
		
		return 1;
	}


};

typedef shared_ptr<HelloProxy> HelloPrx;

#endif
