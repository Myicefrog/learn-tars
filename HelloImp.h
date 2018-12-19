#ifndef _HelloImp_H_
#define _HelloImp_H_

#include "Hello.h"

class HelloImp : public Hello
{
public:

    virtual ~HelloImp() {}

    virtual void initialize();

	virtual void destroy();

	virtual int doRequest(const string& request, vector<char> &buffer);

	virtual int test(const std::string & sReq,vector<char> &buffer);

	virtual int testHello(const std::string & sReq,vector<char> &buffer);
	
};

#endif
