#include "HelloImp.h"

using namespace std;

void HelloImp::initialize()
{
    //initialize servant here:
    //...
	cout<<"HelloImp::initialize"<<endl;
}


void HelloImp::destroy()
{
    //destroy servant here:
    //...
	cout<<"HelloImp::destroy"<<endl;
}

int HelloImp::doRequest(const string& request, vector<char> &buffer)
{
	cout<<"HelloImp::doRequest coming request is "<<request<<endl;

	string response = "very good man";

	size_t pos = request.find(":");	

	string requestId = request.substr(0,pos);
	string requestBody = request.substr(pos + 1);
	
	if(requestBody == "hello")
	{
		response = "hello response";
	}
	else
	{
		response = "buddy, what are you doing";
	}

	response = requestId + ":" + response;

	cout<<"response is "<<response<<endl;

	buffer.assign(response.begin(),response.end());

	return 0;
}

int HelloImp::test(const std::string & sReq,vector<char> &buffer)
{
	size_t pos = sReq.find(":");
	string requestId = sReq.substr(0,pos);
	string response = requestId + ":" + "test response";
	cout<<"response is "<<response<<endl;
	buffer.assign(response.begin(),response.end());
	return 0;
}

int HelloImp::testHello(const std::string & sReq,vector<char> &buffer)
{

	size_t pos = sReq.find(":");
	string requestId = sReq.substr(0,pos);
	string response = requestId + ":" + "testHello response";
	cout<<"response is "<<response<<endl;
	buffer.assign(response.begin(),response.end());
	return 0;

}
