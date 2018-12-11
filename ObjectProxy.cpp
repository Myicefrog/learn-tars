#include "ObjectProxy.h"
#include "NetworkUtil.h"

namespace tars
{

ObjectProxy::ObjectProxy(CommunicatorEpoll * pCommunicatorEpoll)
:_communicatorEpoll(pCommunicatorEpoll)
{


}

ObjectProxy::~ObjectProxy()
{
}


void ObjectProxy::invoke(ReqMessage * msg)
{
//_endpointManger->selectAdapterProxy
//pAdapterProxy = getNextValidProxy();
//adapterProxy->checkActive(true);
//_trans->reconnect();

	int fd = -1;
	
	fd = NetworkUtil::createSocket(false);
        
	NetworkUtil::setBlock(fd, false);

	struct sockaddr_in     _addr;

	string _host = "127.0.0.1";

	uint16_t _port = 9877;

	NetworkUtil::getAddress(_host, _port, _addr);

    bool bConnected = NetworkUtil::doConnect(fd, _addr);
        
	if(bConnected)
    {
		cout<<"bConnect successful"<<endl;
    }
	else
	{
		cout<<"bConnect connecting"<<endl;
	}
	
	_fd = fd;

	//值得学习地方：设置网络qos的dscp标志
	//int iQos;
    //::setsockopt(fd,SOL_IP,IP_TOS,&iQos,sizeof(iQos));

	FDInfo                   _fdInfo;
	_fdInfo.iType = FDInfo::ET_C_NET;
    //_fdInfo.p     = (void *)this;
    _fdInfo.fd    = -1;

	getCommunicatorEpoll()->addFd(fd, &_fdInfo, EPOLLIN|EPOLLOUT);
	
}

}

