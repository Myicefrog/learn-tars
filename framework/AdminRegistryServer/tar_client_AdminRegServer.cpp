#include <iostream>
#include <string>
#include "Communicator.h"
#include "AdminReg.h"

using namespace std;
using namespace tars;

int main()
{
	Communicator comm;

	ServantProxy* prx = NULL;

	string host = "127.0.0.1";

	uint16_t port = 12000;		

	comm.stringToProxy(host, port, &prx);

    string sReq("start:HelloServer");
    string sRsp("");

	AdminRegProxy * prx1 = (AdminRegProxy*)(prx);
	
	if(prx1)
	{
    	int iRet = prx1->startServer(sReq, sRsp);
    	cout<<"sRsp:"<<sRsp<<endl;
		cout<<"iRet is "<<iRet<<endl;
	}
	else
	{
		cout<<"prx1 null"<<endl;
	}
	return 0;
}
