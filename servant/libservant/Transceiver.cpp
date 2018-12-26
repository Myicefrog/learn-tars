#include "Transceiver.h"
#include "ObjectProxy.h"
#include<sys/uio.h>

namespace tars
{

Transceiver::Transceiver(ObjectProxy *objectProxy,const string& ip, const uint16_t& port)
: _fd(-1)
{
    _fdInfo.iType = FDInfo::ET_C_NET;
    _fdInfo.p     = (void *)this;
    _fdInfo.fd    = -1;

	_ip = ip;
	_port = port;

	_objectProxy = objectProxy;
}

Transceiver::~Transceiver()
{
    close();
}

void Transceiver::close()
{
    if(!isValid()) return;

    getObjProxy()->getCommunicatorEpoll()->delFd(_fd,&_fdInfo,EPOLLIN|EPOLLOUT);

    NetworkUtil::closeSocketNoThrow(_fd);

    _fd = -1;

	_sendBuffer.Clear();

	_recvBuffer.Clear();

}

void Transceiver::connect()
{
	int fd = -1;

    fd = NetworkUtil::createSocket(false);

    NetworkUtil::setBlock(fd, false);

    struct sockaddr_in     _addr;

    NetworkUtil::getAddress(_ip, _port, _addr);

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

	getObjProxy()->getCommunicatorEpoll()->addFd(fd, &_fdInfo, EPOLLIN|EPOLLOUT);
}

int Transceiver::doRequest()
{
	int iRet = 0;

	if(!_sendBuffer.IsEmpty())
	{
		size_t length = 0;
		void* data = NULL;
		_sendBuffer.PeekData(data, length);

		iRet = this->send(data, length, 0);

		if(iRet < 0)
		{
			return iRet;
		}

		if(iRet > 0)
		{
			_sendBuffer.Consume(iRet);
			if (_sendBuffer.IsEmpty())
				_sendBuffer.Shrink();
			else
				return 0;
		}
	}

	getObjProxy()->doInvoke();
	
	return 0;	
}

int Transceiver::sendRequest(const char * pData, size_t iSize, bool forceSend)
{
	//保证发送的顺序，这里的数据不会丢失，而是放在了ObjectProxy里的_timeoutQueue中
	if(!_sendBuffer.IsEmpty())
    {
        return eRetError;
    }

	int iRet = this->send(pData,iSize,0);

	if(iRet < 0)
    {
        return eRetError;
    }

	//发送不完再放入_sendBuffer
	if(iRet < (int)iSize)
    {
        _sendBuffer.PushData(pData+iRet,iSize-iRet);
        return eRetFull;
    }
	return iRet;
}

int Transceiver::send(const void* buf, uint32_t len, uint32_t flag)
{
	int iRet = ::send(_fd, buf, len, flag);	
	if (iRet < 0 && errno != EAGAIN)
    {
		cout<<"Transceiver::send fail"<<endl;
		return iRet;

	}

	if (iRet < 0 && errno == EAGAIN)
        iRet = 0;

	return iRet;

}

int Transceiver::doResponse(list<string>& done)
{
	int iRet = 0;
	
	done.clear();
	
	do
	{
		_recvBuffer.AssureSpace(8 * 1024);
		char stackBuffer[64 * 1024];

		struct  iovec vecs[2];
		vecs[0].iov_base = _recvBuffer.WriteAddr();
		vecs[0].iov_len = _recvBuffer.WritableSize();
		vecs[1].iov_base = stackBuffer;
		vecs[1].iov_len = sizeof stackBuffer;

		if( (iRet = this->readv(vecs,2)) > 0 )
		{
			if(static_cast<size_t>(iRet) <= vecs[0].iov_len)
			{
				_recvBuffer.Produce(iRet);
			}
			else
			{
				_recvBuffer.Produce(vecs[0].iov_len);
				size_t stackBytes = static_cast<size_t>(iRet) - vecs[0].iov_len;
				_recvBuffer.PushData(stackBuffer,stackBytes);
			}
		}
	
	}
	while (iRet>0);	

	if(!_recvBuffer.IsEmpty())
	{
			const char* data = _recvBuffer.ReadAddr();
			size_t len = _recvBuffer.ReadableSize();
			done.push_back(string(data,len));

			if(len > 0)
			{
				_recvBuffer.Consume(len);
				if(_recvBuffer.Capacity() > 8 * 1024 * 1024)
				{
					_recvBuffer.Shrink();
				}
			}

	}
	
	return done.empty()?0:1;
}

int Transceiver::readv(const struct iovec* vecs, int32_t vcnt)
{
    int iRet = ::readv(_fd, vecs, vcnt);

    if (iRet == 0 || (iRet < 0 && errno != EAGAIN))
    {

        close();

        return 0;
    }

    return iRet;
}


}
