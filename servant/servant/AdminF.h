#ifndef __ADMINF_H_
#define __ADMINF_H_

#include <map>
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

#include "ServantProxy.h"
#include "Servant.h"

namespace tars
{

class AdminF : public tars::Servant
{
public:

	virtual ~AdminF(){}

	virtual std::string notify(const std::string & command) = 0;

	virtual void shutdown() = 0;

public:

	int onDispatch(const string &request, vector<char> &buffer)
	{
		static ::std::string __tars__AdminF_all[]=
		{
			"notify",
			"shutdown"
		};

        size_t pos = request.find(":");

        string requestBody = request.substr(pos + 1); 

        size_t posMethod = requestBody.find(":");

        string method = requestBody.substr(0, posMethod);
		
		pair<string*, string*> r = equal_range(__tars__AdminF_all, __tars__AdminF_all+2, method);

		if(r.first == r.second)
		{
			cout<<"can not find AdminF method"<<endl;
			return -3;
		}

		switch(r.first - __tars__AdminF_all)
		{
			case 0:
			{
				std::string _ret = notify(request);
				cout<<"_ret is "<<_ret<<endl;
				return 0;
			}
			case 1:
			{
				shutdown();
				return 0;
			}
		}

		return 0;
	}

};

}

#endif
