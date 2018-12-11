#ifndef _HelloImp_H_
#define _HelloImp_H_

#include "Servant.h"

class HelloImp : public tars::Servant
{
public:

    virtual ~HelloImp() {}

    virtual void initialize();

	virtual void destroy();

	virtual int doRequest(const string& request, vector<char> &buffer);
};

#endif
