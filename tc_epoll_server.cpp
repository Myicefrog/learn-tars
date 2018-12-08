#include "tc_epoll_server.h"

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cassert>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>

using namespace std;

namespace tars
{
#define H64(x) (((uint64_t)x) << 32)

TC_EpollServer::TC_EpollServer()
{
    _netThreads = new TC_EpollServer::NetThread(this);
}

TC_EpollServer::~TC_EpollServer()
{
	delete _netThreads;
}

void TC_EpollServer::send(unsigned int uid, const string &s, const string &ip, uint16_t port, int fd)
{

    _netThreads->send(uid, s, ip, port);

}

TC_EpollServer::NetThread::NetThread(TC_EpollServer *epollServer)
: _epollServer(epollServer)
{
	_shutdown.createSocket();
	_notify.createSocket();
	
	_response.response="";
	_response.uid = 0;
}

TC_EpollServer::NetThread::~NetThread()
{
}

int  TC_EpollServer::NetThread::bind(string& ip, int& port)
{
    //建立server端接收请求用的socket
    //socket-->bind-->listen

	int type = AF_INET;
	
	_bind_listen.createSocket(SOCK_STREAM, type);

	_bind_listen.bind(ip,port);

    _bind_listen.listen(1024);

    cout<<"server alreay listen fd is "<<_bind_listen.getfd()<<endl;

    _bind_listen.setKeepAlive();
    _bind_listen.setTcpNoDelay();
    //不要设置close wait否则http服务回包主动关闭连接会有问题
    _bind_listen.setNoCloseWait();
    _bind_listen.setblock(false);

	return _bind_listen.getfd();

}


void TC_EpollServer::NetThread::createEpoll(uint32_t iIndex)
{
	int _total = 200000;
	
	_epoller.create(10240);
	
	_epoller.add(_shutdown.getfd(), H64(ET_CLOSE), EPOLLIN);

	_epoller.add(_notify.getfd(), H64(ET_NOTIFY), EPOLLIN);

	_epoller.add(_bind_listen.getfd(), H64(ET_LISTEN) | _bind_listen.getfd(), EPOLLIN);	

	for(uint32_t i = 1; i <= _total; i++)
	{
		_free.push_back(i);

		++_free_size;
	}

	cout<<"epoll create successful"<<endl;
}

void TC_EpollServer::NetThread::run()
{
	cout<<"NetThread run"<<endl;

	while(true)
	{
		int iEvNum = _epoller.wait(2000);
	
		for(int i = 0; i < iEvNum; ++i)
		{
			const epoll_event &ev = _epoller.get(i);	

			uint32_t h = ev.data.u64 >> 32;

			switch(h)
			{
			case ET_LISTEN:
				cout<<"ET_LISTEN"<<endl;
				{
					if(ev.events & EPOLLIN)
					{
						bool ret;
						do
						{
							ret = accept(ev.data.u32);
						}while(ret);
					}
				}
				break;
			case ET_CLOSE:
				cout<<"ET_CLOSE"<<endl;
				break;
			case ET_NOTIFY:
				cout<<"ET_NOTIFY"<<endl;	
				processPipe();
				break;
			case ET_NET:
				cout<<"ET_NET"<<endl;
				processNet(ev);
				break;
			default:
				assert(true);
			}
		}
	}
}

bool TC_EpollServer::NetThread::accept(int fd)
{
	struct sockaddr_in stSockAddr;

	socklen_t iSockAddrSize = sizeof(sockaddr_in);	

	TC_Socket cs;
    cs.setOwner(false);

    	//接收连接
	TC_Socket s;
	s.init(fd, false, AF_INET);

	int iRetCode = s.accept(cs, (struct sockaddr *) &stSockAddr, iSockAddrSize);

	if (iRetCode > 0)
	{
		string  ip;

		uint16_t port;

		char sAddr[INET_ADDRSTRLEN] = "\0";

		struct sockaddr_in *p = (struct sockaddr_in *)&stSockAddr;

		inet_ntop(AF_INET, &p->sin_addr, sAddr, sizeof(sAddr));

		ip      = sAddr;
		port    = ntohs(p->sin_port);

		cout<<"accept ip is "<<ip<<" port is "<<port<<endl;

		cs.setblock(false);
		cs.setKeepAlive();
		cs.setTcpNoDelay();
		cs.setCloseWaitDefault();

		uint32_t uid = _free.front();

		_free.pop_front();

		--_free_size;

		_listen_connect_id[uid] = cs.getfd();

		cout<<"server accept successful fd is "<<cs.getfd()<<endl;

		_epoller.add(cs.getfd(), uid, EPOLLIN | EPOLLOUT);

	}
	else
	{
		if(errno == EAGAIN)
		{
			return false;
		}
		return true;
	}
	return true;
}

void TC_EpollServer::NetThread::processNet(const epoll_event &ev)
{

	uint32_t uid = ev.data.u32;	

	int fd = _listen_connect_id[uid];

	cout<<"processNet uid is "<<uid<<" fd is "<<fd<<endl;

	if (ev.events & EPOLLERR || ev.events & EPOLLHUP)
	{
		cout<<"should delet connection"<<endl;
		return;
	}

	if(ev.events & EPOLLIN)
	{
		recv_queue::queue_type vRecvData;

		while(true)
		{
			char buffer[32*1024];
			int iBytesReceived = 0;
			
			iBytesReceived = ::read(fd, (void*)buffer, sizeof(buffer));
			cout<<"server recieve "<<iBytesReceived<<" bytes buffer is "<<buffer<<endl;
			
			if(iBytesReceived < 0)
			{
				if(errno == EAGAIN)
				{
					break;
				}
				else
				{
					cout<<"client close"<<endl;
					return ;
				}
			}
			else if( iBytesReceived == 0 )
			{
				cout<<"1 client close"<<endl;
				return ;
			}

			_recvbuffer.append(buffer, iBytesReceived);

		}

		if(!_recvbuffer.empty())
		{
			tagRecvData* recv = new tagRecvData();
			recv->buffer           = std::move(_recvbuffer);
			recv->ip               = "";
			recv->port             = 0;
			recv->recvTimeStamp    = 0;
			recv->uid              = uid;
			recv->isOverload       = false;
			recv->isClosed         = false;
			recv->fd               = fd;

			vRecvData.push_back(recv);
		}

		if(!vRecvData.empty())
		{
			cout<<"insertRecvQueue"<<endl;
			insertRecvQueue(vRecvData);
		}

	}

	if (ev.events & EPOLLOUT)
	{
		cout<<"need to send data"<<endl;
	}	
}

void TC_EpollServer::NetThread::processPipe()
{	

    send_queue::queue_type deSendData;

    _sbuffer.swap(deSendData);

    send_queue::queue_type::iterator it = deSendData.begin();

    send_queue::queue_type::iterator itEnd = deSendData.end();

    while(it != itEnd)
    {
        switch((*it)->cmd)
        {
        case 's':
            {
                uint32_t uid = (*it)->uid;
               
                int fd = _listen_connect_id[uid];

                cout<<"processPipe uid is "<<uid<<" fd is "<<fd<<endl;

                int bytes = ::send(fd, (*it)->buffer.c_str(), (*it)->buffer.size(), 0);

                cout<<"send byte is "<<bytes<<endl;

                break;
           }
        default:
            assert(false);
        }
        delete (*it);
        ++it;
    }
                

}

void TC_EpollServer::NetThread::send(uint32_t uid, const string &s, const string &ip, uint16_t port)
{
    tagSendData* send = new tagSendData();

    send->uid = uid;

    send->cmd = 's';

    send->buffer = s;

    send->ip = ip;

    send->port = port;

    _sbuffer.push_back(send);

    //通知epoll响应, 有数据要发送
    _epoller.mod(_notify.getfd(), H64(ET_NOTIFY), EPOLLOUT);
}

TC_EpollServer::Handle::Handle()
: _pEpollServer(NULL)
, _iWaitTime(100)
{
}

TC_EpollServer::Handle::~Handle()
{
}

void TC_EpollServer::Handle::sendResponse(uint32_t uid, const string &sSendBuffer, const string &ip, int port, int fd)
{
    _pEpollServer->send(uid, sSendBuffer, ip, port, fd);
}


bool TC_EpollServer::Handle::waitForRecvQueue(tagRecvData* &recv, uint32_t iWaitTime)
{

   TC_EpollServer::NetThread* netThread = _pEpollServer->getNetThread();

    return netThread->waitForRecvQueue(recv,iWaitTime);
}


void TC_EpollServer::Handle::close(uint32_t uid, int fd)
{
    //_pEpollServer->close(uid, fd);
}

void TC_EpollServer::Handle::run()
{
    initialize();

    handleImp();
}

void TC_EpollServer::Handle::handleImp()
{
    cout<<"Handle::handleImp"<<endl;
    tagRecvData* recv = NULL;

    while(true)
    {
        {
            TC_EpollServer::NetThread* netThread = _pEpollServer->getNetThread();

            TC_ThreadLock::Lock lock(netThread->monitor);

            netThread->monitor.timedWait(100);

        }

        while(waitForRecvQueue(recv, 0))
        {

            cout<<"handleImp recv uid  is "<<recv->uid<<endl;
            _pEpollServer->send(recv->uid,recv->buffer, "0", 0, 0);

        }
    }

}

void TC_EpollServer::Handle::setEpollServer(TC_EpollServer *pEpollServer)
{
    TC_ThreadLock::Lock lock(*this);

    _pEpollServer = pEpollServer;
}


bool TC_EpollServer::NetThread::waitForRecvQueue(tagRecvData* &recv, uint32_t iWaitTime)
{
    //cout<<"NetThread::waitForRecvQueue"<<endl;

    bool bRet = false;

    bRet = _rbuffer.pop_front(recv, iWaitTime);

    if(!bRet)
    {
        return bRet;
    }

    return bRet;
}

void TC_EpollServer::NetThread::insertRecvQueue(const recv_queue::queue_type &vtRecvData, bool bPushBack)
{
    {
        if (bPushBack)
        {
            _rbuffer.push_back(vtRecvData);
        }
        else
        {
            _rbuffer.push_front(vtRecvData);
        }
    }

    TC_ThreadLock::Lock lock(monitor);

    monitor.notify();
}

}

