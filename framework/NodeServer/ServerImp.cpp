#include "ServerImp.h"
#include <iostream>

int ServerImp::keepAlive( const std::string & sReq,vector<char> &buffer )
{
	cout<<"Node Server have recieve heartbeat from "<<sReq<<endl;

	string response = "I have received";
	
	buffer.assign(response.begin(),response.end());

    return -1;
}

