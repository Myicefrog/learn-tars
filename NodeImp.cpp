#include "NodeImp.h"
#include "NodeServer.h"
#include "CommandStart.h"
#include "ServerObject.h"
#include <iostream>

void NodeImp::initialize()
{
}

int NodeImp::startServer(const std::string & sReq,vector<char> &buffer)
{
	size_t pos = sReq.find(":");
    string requestId = sReq.substr(0,pos);

	int iRet =	-1;

	cout<<"startServer sReq is "<<sReq<<endl;

	string s;

	string result = "have notify to startServer";
	
	ServerObjectPtr pServerObjectPtr = make_shared<ServerObject>(sReq);

	pServerObjectPtr->setScript("./NodeServer-start.sh");

	if( pServerObjectPtr )
	{
		bool bByNode = true;

		CommandStart command(pServerObjectPtr,bByNode);

		iRet = command.doProcess(s);

		if (iRet == 0 )
		{
			result = "server is activating";
		}	
	} 

	result = requestId + ":" + result; 
	buffer.assign(result.begin(),result.end());

	return iRet;
}
