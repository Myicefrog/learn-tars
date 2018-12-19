#ifndef __TARS_SERVANT_H_
#define __TARS_SERVANT_H_

#include "tc_epoll_server.h"
#include "ServantProxy.h"

using namespace std;

namespace tars
{
	
class Servant
{
public:

	Servant();

	~Servant();

	void setHandle(TC_EpollServer::Handle* handle);

	TC_EpollServer::Handle* getHandle();

	void setName(const string &name);

	string getName() const;

	virtual void initialize() = 0;

	virtual void destroy() = 0;

	virtual int dispatch(const string &request, vector<char> &buffer);

	virtual int onDispatch(const string &request, vector<char> &buffer) { return -1; }

	virtual int doRequest(const string &request, vector<char> &buffer) { return -1; }

protected:

	TC_EpollServer::Handle* _handle;

	string                  _name;
};
	
typedef shared_ptr<Servant> ServantPtr;

}

#endif
