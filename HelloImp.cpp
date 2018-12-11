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
	
	if(request == "hello")
	{
		response = "hello response";
	}
	else
	{
		response = "buddy, what are you doing";
	}

	buffer.assign(response.begin(),response.end());

	return 0;
}
