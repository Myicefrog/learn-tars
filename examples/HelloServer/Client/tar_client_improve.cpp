#include <iostream>
#include <string>
#include "Communicator.h"
#include "Hello.h"

using namespace std;
using namespace tars;

int main()
{
	Communicator comm;

	//HelloPrx prx;	

	//HelloProxy* prx = NULL;

	ServantProxy* prx = NULL;

	string host = "127.0.0.1";

	uint16_t port = 9877;		

	comm.stringToProxy(host, port, &prx);

    string sReq("hello");
    string sRsp("");

	HelloProxy* prx1 = (HelloProxy*)(prx);
	
	if(prx1)
	{

		//for(int i = 0; i < 10; i++)
		{
    		int iRet = prx1->testHello(sReq, sRsp);
    		cout<<"sRsp:"<<sRsp<<endl;
			cout<<"iRet is "<<iRet<<endl;

		}
	}
	else
	{
		cout<<"prx1 null"<<endl;
	}
	return 0;
}
