#include "Servant.h"

namespace tars
{

Servant::Servant():_handle(NULL)
{
}

Servant::~Servant()
{
}

void Servant::setName(const string &name)
{
    _name = name;
}

string Servant::getName() const
{
    return _name;
}

void Servant::setHandle(TC_EpollServer::Handle* handle)
{
    _handle = handle;
}

TC_EpollServer::Handle* Servant::getHandle()
{
    return _handle;
}

int Servant::dispatch(const string &request, vector<char> &buffer)
{
	cout<<"Servant::dispatch"<<endl;

	int ret = 0;

	ret = onDispatch(request, buffer);

/*	
    {
		//这里为什么要加锁
        //TC_LockT<TC_ThreadRecMutex> lock(*this);

        ret = doRequest(request, buffer);
    }
*/
    return ret;
}

}
