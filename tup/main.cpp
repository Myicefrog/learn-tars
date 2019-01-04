#include "tup.h"
#include "BaseF.h"
#include <iostream>
#include <string>
#include "AppProtocal.h"

using namespace std;
using namespace tars;

int main()
{
//序列化

	TarsOutputStream<BufferWriter> os;

	string res = "I am ok";

	vector<char> buffer;
	
	buffer.assign(res.begin(),res.end());

	map<string, string> status;

	status["test"] = "test";

	map<string, string> _responseContext;
	
	_responseContext["test1"] = "test1";

	ResponsePacket response;

	response.iRequestId     = 1;
    response.iMessageType   = 0;
    response.cPacketType    = TARSNORMAL;
    response.iVersion       = TARSVERSION;
    response.status         = status;
    response.sBuffer        = buffer;
    response.sResultDesc    = "123";
    response.context        = _responseContext;
    response.iRet           = 0;

	response.writeTo(os);

	tars::Int32 iHeaderLen = htonl(sizeof(tars::Int32) + os.getLength());

	cout<<"iHeaderLen is "<<iHeaderLen<<endl;

    string s = "";

    s.append((const char*)&iHeaderLen, sizeof(tars::Int32));

    s.append(os.getBuffer(), os.getLength());

	cout<<"s is "<<s<<endl;

//反序列化

	list<ResponsePacket> done;

	ProxyProtocol	_proxyProtocol;	

	_proxyProtocol.responseFunc = ProxyProtocol::tarsResponse;

	const char* data = s.c_str();

	size_t len = s.size();

	size_t pos = _proxyProtocol.responseFunc(data, len, done);

	cout<<"pos is "<<pos<<endl;	
	for(auto doneptr : done)
	{
		cout<<doneptr.iRequestId<<endl;
	}

	return 0;
}
