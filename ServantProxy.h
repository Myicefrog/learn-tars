#ifndef _TARS_SERVANT_PROXY_H_
#define _TARS_SERVANT_PROXY_H_

#include <string>
#include "Message.h"


using namespace std;

namespace tars
{

class ServantProxyCallback
{
public:
	
	ServantProxyCallback();

	virtual ~ServantProxyCallback() {}

	virtual const string& getType() { return _type; }

	virtual void setType(const string& type) { _type = type; }

	inline void setNetThreadProcess(bool bNetThreadProcess)
    {
        _bNetThreadProcess = bNetThreadProcess;
    }

	inline bool getNetThreadProcess()
    {
        return _bNetThreadProcess;
    }

public:

    virtual int onDispatch(ReqMessagePtr msg) = 0;

protected:

	string _type;

	bool _bNetThreadProcess;

};

}

#endif
