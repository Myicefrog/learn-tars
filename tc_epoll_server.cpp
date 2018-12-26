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
#include <limits>

#include <limits.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/un.h>

using namespace std;

namespace tars
{
#define H64(x) (((uint64_t)x) << 32)

TC_EpollServer::TC_EpollServer(unsigned int iNetThreadNum)
: _bTerminate(false)
, _netThreadNum(iNetThreadNum)
, _handleStarted(false)
{
	for (size_t i = 0; i < iNetThreadNum; ++i)
	{
		TC_EpollServer::NetThread* netThreads = new TC_EpollServer::NetThread(this);
		_netThreads.push_back(netThreads);
	}
}

TC_EpollServer::~TC_EpollServer()
{
	terminate();
}

void TC_EpollServer::send(unsigned int uid, const string &s, const string &ip, uint16_t port, int fd)
{
	TC_EpollServer::NetThread* netThread = getNetThreadOfFd(fd);

    netThread->send(uid, s, ip, port);

}

int  TC_EpollServer::bind(TC_EpollServer::BindAdapterPtr& lsPtr)
{
    int iRet = 0;

    for(size_t i = 0; i < _netThreads.size(); ++i)
    {
        if(i == 0)
        {
            iRet = _netThreads[i]->bind(lsPtr);
        }
    }

    return iRet;
}

void TC_EpollServer::startHandle()
{
    if (!_handleStarted)
    {
        _handleStarted = true;

        for (auto& kv : _handleGroups)
        {
            auto& hds = kv.second->handles;

            for (auto& handle : hds)
            {
                if (!handle->isAlive())
                {
                    handle->start();
                }
            }
        }
    }
}

void TC_EpollServer::stopThread()
{
    for (auto& kv : _handleGroups)
    {
        {
            TC_ThreadLock::Lock lock(kv.second->monitor);

            kv.second->monitor.notifyAll();
        }
        auto& hds = kv.second->handles;
        for (auto& handle : hds)
        {
            if (handle->isAlive())
            {
                handle->getThreadControl().join();
            }
        }
    }
}

void TC_EpollServer::addConnection(TC_EpollServer::NetThread::Connection * cPtr, int fd, int iType)
{
    TC_EpollServer::NetThread* netThread = getNetThreadOfFd(fd);

	netThread->addTcpConnection(cPtr);
}

void TC_EpollServer::createEpoll()
{
    for(size_t i = 0; i < _netThreads.size(); ++i)
    {
        _netThreads[i]->createEpoll(i+1);
    }
}

void TC_EpollServer::close(unsigned int uid, int fd)
{
    TC_EpollServer::NetThread* netThread = getNetThreadOfFd(fd);

    netThread->close(uid);
}

void TC_EpollServer::terminate()
{
    if(!_bTerminate)
    {
        tars::TC_ThreadLock::Lock sync(*this);
        _bTerminate = true;
        notifyAll();
    }
}

TC_EpollServer::NetThread::Connection::Connection(TC_EpollServer::BindAdapter *pBindAdapter, int lfd, int timeout, int fd, const string& ip, uint16_t port)
: _pBindAdapter(pBindAdapter)
, _uid(0)
, _lfd(lfd)
, _ip(ip)
, _bClose(false)
, _port(port)
{
    assert(fd != -1);

    _sock.init(fd, true, AF_INET);
}

TC_EpollServer::NetThread::Connection::~Connection()
{
	if(_lfd != -1)
    {
        assert(!_sock.isValid());
    }

}

void TC_EpollServer::NetThread::Connection::close()
{
    if(_lfd != -1)
    {
        if(_sock.isValid())
        {
            _sock.close();
        }
    }
}

int TC_EpollServer::NetThread::Connection::recv(recv_queue::queue_type &o)
{
    o.clear();

    while(true)
    {
        char buffer[32 * 1024];
        int iBytesReceived = 0;

        iBytesReceived = ::read(_sock.getfd(), (void*)buffer, sizeof(buffer));

		cout<<"Connection::recv iBytesReceived is "<<iBytesReceived<<" buffer is "<<buffer<<endl;

        if (iBytesReceived < 0)
        {
            if(errno == EAGAIN)
            {
                //没有数据了
                break;
            }
            else
            {
                //客户端主动关闭
				cout<<"client close itself"<<endl;
                return -1;
            }
        }
        else if( iBytesReceived == 0)
        {
            //客户端主动关闭
			cout<<"client close itself---1"<<endl;
            return -1;
        }

        _recvbuffer.append(buffer, iBytesReceived);

        //接收到数据不超过buffer,没有数据了(如果有数据,内核会再通知你)
        if((size_t)iBytesReceived < sizeof(buffer))
        {
        	break;
        }
    }

    if(_lfd != -1)
    {
        //return parseProtocol(o);
        if(!_recvbuffer.empty())
        {
        	tagRecvData* recv = new tagRecvData();
            recv->buffer           = std::move(_recvbuffer);
           	recv->ip               = _ip;
            recv->port             = _port;
            recv->recvTimeStamp    = 0;
            recv->uid              = getId();
            recv->isOverload       = false;
            recv->isClosed         = false;
            recv->fd               = getfd();

            o.push_back(recv);
        }
		return o.size();
    }

    return o.size();
}


void TC_EpollServer::NetThread::Connection::insertRecvQueue(recv_queue::queue_type &vRecvData)
{
    if(!vRecvData.empty())
    {
        _pBindAdapter->insertRecvQueue(vRecvData);
    }
}

int TC_EpollServer::NetThread::Connection::send()
{
    if(_sendbuffer.empty()) return 0;

    return send("", _ip, _port, true);
}


int TC_EpollServer::NetThread::Connection::send(const string& buffer, const string &ip, uint16_t port, bool byEpollOut)
{

    if (byEpollOut)
    {
        int bytes = this->send(_sendbuffer);
        if (bytes == -1) 
        { 
            return -1; 
        } 

        this->adjustSlices(_sendbuffer, bytes);
    }
    else
    {
		int bytes = this->tcpSend(buffer.data(), buffer.size()); 

		if (bytes == -1) 
		{
			cout<<"close connection by peer"<<endl;
			return -1;
		}

		//TO BE DNOE
		//这里后面要添加缓存，用来保存发送不完的数据
    }

    size_t toSendBytes = 0;
    for (const auto& slice : _sendbuffer)
    {
        toSendBytes += slice.dataLen;
    }

    if (toSendBytes >= 8 * 1024)
    {
		cout<<" buffer too long close."<<endl;
        clearSlices(_sendbuffer);
        return -2;
    }


    //需要关闭链接
    if(_bClose && _sendbuffer.empty())
    {
        cout<<" close connection by user."<<endl;
        return -2;
    }

    return 0;
}


int TC_EpollServer::NetThread::Connection::send(const std::vector<TC_Slice>& slices)
{
    const int kIOVecCount = std::max<int>(sysconf(_SC_IOV_MAX), 16); // be care of IOV_MAX

    size_t alreadySentVecs = 0;
    size_t alreadySentBytes = 0;
    while (alreadySentVecs < slices.size())
    {
        const size_t vc = std::min<int>(slices.size() - alreadySentVecs, kIOVecCount);

        // convert to iovec array
        std::vector<iovec> vecs;
        size_t expectSent = 0;
        for (size_t i = alreadySentVecs; i < alreadySentVecs + vc; ++ i)
        {
            assert (slices[i].dataLen > 0);

            iovec ivc;
            ivc.iov_base = slices[i].data;
            ivc.iov_len = slices[i].dataLen;
            expectSent += slices[i].dataLen;

            vecs.push_back(ivc);
        }

        int bytes = tcpWriteV(vecs);
        if (bytes == -1)
            return -1; // should close
        else if (bytes == 0)
            return alreadySentBytes; // EAGAIN
        else if (bytes == static_cast<int>(expectSent))
        {
            alreadySentBytes += bytes;
            alreadySentVecs += vc; // continue sent
        }
        else
        {
            assert (bytes > 0); // partial send
            alreadySentBytes += bytes;
            return alreadySentBytes;
        }
    }
                
    return alreadySentBytes;
}

int TC_EpollServer::NetThread::Connection::tcpWriteV(const std::vector<iovec>& buffers)
{
    const int kIOVecCount = std::max<int>(sysconf(_SC_IOV_MAX), 16); // be care of IOV_MAX
    const int cnt = static_cast<int>(buffers.size());

    assert (cnt <= kIOVecCount);
        
    const int sock = _sock.getfd();
        
    int bytes = static_cast<int>(::writev(sock, &buffers[0], cnt));
    if (bytes == -1)
    {
        assert (errno != EINVAL);
        if (errno == EAGAIN)
            return 0;

        return -1;  // can not send any more
    }
    else
    {
        return bytes;
    }
}

int TC_EpollServer::NetThread::Connection::tcpSend(const void* data, size_t len)
{
    if (len == 0)
        return 0;

    int bytes = ::send(_sock.getfd(), data, len, 0);
    if (bytes == -1)
    {
        if (EAGAIN == errno)
            bytes = 0; 
                      
        if (EINTR == errno)
            bytes = 0; // try ::send later
    }

    return bytes;
}

void TC_EpollServer::NetThread::Connection::clearSlices(std::vector<TC_Slice>& slices)
{
    adjustSlices(slices, std::numeric_limits<std::size_t>::max());
}

void TC_EpollServer::NetThread::Connection::adjustSlices(std::vector<TC_Slice>& slices, size_t toSkippedBytes)
{
    size_t skippedVecs = 0;
    for (size_t i = 0; i < slices.size(); ++ i)
    {
        assert (slices[i].dataLen > 0);
        if (toSkippedBytes >= slices[i].dataLen)
        {
            toSkippedBytes -= slices[i].dataLen;
            ++ skippedVecs;
        }
        else
        {
            if (toSkippedBytes != 0)
            {
                const char* src = (const char*)slices[i].data + toSkippedBytes;
                memmove(slices[i].data, src, slices[i].dataLen - toSkippedBytes);
                slices[i].dataLen -= toSkippedBytes;
            }

            break;
        }
    }

	//TO BE DNOE
    // free to pool

    slices.erase(slices.begin(), slices.begin() + skippedVecs);
}

bool TC_EpollServer::NetThread::Connection::setClose()
{
    _bClose = true;
    return _sendbuffer.empty();
}

TC_EpollServer::NetThread::NetThread(TC_EpollServer *epollServer)
: _epollServer(epollServer)
, _bTerminate(false)
{
	_shutdown.createSocket();
	_notify.createSocket();
	
	_response.response="";
	_response.uid = 0;
}

TC_EpollServer::NetThread::~NetThread()
{
}

int  TC_EpollServer::NetThread::bind(BindAdapterPtr &lsPtr)
{
    const TC_Endpoint &ep = lsPtr->getEndpoint();

    TC_Socket& s = lsPtr->getSocket();

    bind(ep, s);

    _listeners[s.getfd()] = lsPtr;

    return s.getfd();
}

void TC_EpollServer::NetThread::bind(const TC_Endpoint &ep, TC_Socket &s)
{
    int type = AF_INET;

    s.createSocket(SOCK_STREAM, type);
    
    s.bind(ep.getHost(), ep.getPort());

    s.listen(1024);
    s.setKeepAlive();
    s.setTcpNoDelay();
    //不要设置close wait否则http服务回包主动关闭连接会有问题
    s.setNoCloseWait();

    s.setblock(false);
}

void TC_EpollServer::NetThread::createEpoll(uint32_t iIndex)
{
	//TO BE DONE _bufferPool
	
	uint32_t _total = 200000;
	
	_epoller.create(10240);
	
	_epoller.add(_shutdown.getfd(), H64(ET_CLOSE), EPOLLIN);

	_epoller.add(_notify.getfd(), H64(ET_NOTIFY), EPOLLIN);

	_epoller.add(_bind_listen.getfd(), H64(ET_LISTEN) | _bind_listen.getfd(), EPOLLIN);	

    for (const auto& kv : _listeners)
    {
        _epoller.add(kv.first, H64(ET_LISTEN) | kv.first, EPOLLIN);	
    }

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

	while(!_bTerminate)
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

		Connection *cPtr = new Connection(_listeners[fd].get(), fd, 2, cs.getfd(), ip, port);

		_epollServer->addConnection(cPtr, cs.getfd(), 0);

		return true;

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

	Connection *cPtr = _uid_connection[uid];

	int fd = cPtr->getfd();

	cout<<"processNet uid is "<<uid<<" fd is "<<fd<<endl;

	if (ev.events & EPOLLERR || ev.events & EPOLLHUP)
	{
		cout<<"should delet connection"<<endl;

		delConnection(cPtr,true,EM_SERVER_CLOSE);
		return;
	}

	if(ev.events & EPOLLIN)
	{
		recv_queue::queue_type vRecvData;

		int ret = recvBuffer(cPtr, vRecvData);

		if(ret < 0)
		{
			cout<<"NetThread::processNet delConnection"<<endl;
			delConnection(cPtr,true,EM_CLIENT_CLOSE);
			return;
		}

		if(!vRecvData.empty())
		{
			cout<<"insertRecvQueue"<<endl;
			cPtr->insertRecvQueue(vRecvData);
		}

	}

	if (ev.events & EPOLLOUT)
	{
		cout<<"need to send data"<<endl;
		int ret = sendBuffer(cPtr);
		if (ret < 0)
		{
			cout<<"need delConnection"<<endl;
		}
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
        case 'c':
            {
                Connection *cPtr = _uid_connection[(*it)->uid];

                if(cPtr)
                {
                    if(cPtr->setClose())
                    {
                        delConnection(cPtr,true,EM_SERVER_CLOSE);
                    }
                }
                break;
            }
        case 's':
            {
                uint32_t uid = (*it)->uid;

				Connection *cPtr = _uid_connection[uid];    

				//int fd = cPtr->getfd();           

                cout<<"processPipe uid is "<<cPtr->getId()<<" fd is "<<cPtr->getfd()<<endl;

				int ret = sendBuffer(cPtr, (*it)->buffer, (*it)->ip, (*it)->port);
				
				if(ret < 0)
				{
					cout<<"remember to delete Connection delConnection"<<endl;
				}

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
	cout<<"NetThread::send threadid is "<<id()<<endl;

    if(_bTerminate)
    {
        return;
    }	

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

void TC_EpollServer::NetThread::close(uint32_t uid)
{
    tagSendData* send = new tagSendData();

    send->uid = uid;

    send->cmd = 'c';

    _sbuffer.push_back(send);

    //通知epoll响应, 关闭连接
    _epoller.mod(_notify.getfd(), H64(ET_NOTIFY), EPOLLOUT);
}

void TC_EpollServer::NetThread::addTcpConnection(TC_EpollServer::NetThread::Connection *cPtr)
{
	cout<<"NetThread::addTcpConnection threadid is "<<id()<<endl;

	uint32_t uid = _free.front();

	cPtr->init(uid);

	_free.pop_front();

	--_free_size;

	_uid_connection[uid] = cPtr;

	//注意这里是EPOLLIN 和EPOLLOUT同时有，目的是EPOLLOUT保证在processNet时候发送上次未发送完的结果

    _epoller.add(cPtr->getfd(), cPtr->getId(), EPOLLIN | EPOLLOUT);
	
}

int  TC_EpollServer::NetThread::recvBuffer(TC_EpollServer::NetThread::Connection *cPtr, recv_queue::queue_type &v)
{
    return cPtr->recv(v);
}

int  TC_EpollServer::NetThread::sendBuffer(TC_EpollServer::NetThread::Connection *cPtr)
{
    return cPtr->send();
}

int  TC_EpollServer::NetThread::sendBuffer(TC_EpollServer::NetThread::Connection *cPtr, const string &buffer, const string &ip, uint16_t port)
{
    return cPtr->send(buffer, ip, port);
}

void TC_EpollServer::NetThread::terminate()
{
    _bTerminate = true;

    //通知队列醒过来
	//还没找到谁在等这个通知
    _sbuffer.notifyT();

    //通知epoll响应, 关闭连接
    _epoller.mod(_shutdown.getfd(), H64(ET_CLOSE), EPOLLOUT);
}

void TC_EpollServer::NetThread::delConnection(TC_EpollServer::NetThread::Connection *cPtr, bool bEraseList,EM_CLOSE_T closeType)
{
    //如果是TCP的连接才真正的关闭连接
    if (cPtr->getListenfd() != -1)
    {
        uint32_t uid = cPtr->getId();

        //构造一个tagRecvData，通知业务该连接的关闭事件

        tagRecvData* recv = new tagRecvData();
        recv->uid        =  uid;
        recv->ip         = cPtr->getIp();
        recv->port       = cPtr->getPort();
        recv->isClosed   = true;
        recv->isOverload = false;
        recv->recvTimeStamp = 0;
        recv->fd         = cPtr->getfd();
        recv->closeType = (int)closeType;

        recv_queue::queue_type vRecvData;

        vRecvData.push_back(recv);

        cPtr->getBindAdapter()->insertRecvQueue(vRecvData);

        //从epoller删除句柄放在close之前, 否则重用socket时会有问题
        _epoller.del(cPtr->getfd(), uid, 0);

        cPtr->close();
    }
}


TC_EpollServer::Handle::Handle()
: _pEpollServer(NULL)
, _handleGroup(NULL)
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

void TC_EpollServer::Handle::close(uint32_t uid, int fd)
{
    _pEpollServer->close(uid, fd);
}

void TC_EpollServer::Handle::notifyFilter()
{
    //TC_ThreadLock::Lock lock(_lsPtr->monitor);
    TC_ThreadLock::Lock lock(_handleGroup->monitor);

    //如何做到不唤醒所有handle呢？
    //_lsPtr->monitor.notifyAll();
	_handleGroup->monitor.notifyAll();
}

void TC_EpollServer::Handle::run()
{
    initialize();

    handleImp();
}

void TC_EpollServer::Handle::handleImp()
{
    cout<<"Handle::handleImp"<<endl;

    while(!getEpollServer()->isTerminate())
    {
        {
            //TC_ThreadLock::Lock lock(_lsPtr->monitor);
            //_lsPtr->monitor.timedWait(100);
            TC_ThreadLock::Lock lock(_handleGroup->monitor);
	
			_handleGroup->monitor.timedWait(100);
        }

		heartbeat();

		//BindAdapterPtr& adapters = _lsPtr;
    	tagRecvData* recv = NULL;

		map<string, BindAdapterPtr>& adapters = _handleGroup->adapters;

		for (auto& kv : adapters)
		{
			BindAdapterPtr& adapter = kv.second;

			try
			{

        		while(adapter->waitForRecvQueue(recv, 0))
				{

					tagRecvData& stRecvData = *recv;

					stRecvData.adapter = adapter;

					if(stRecvData.isClosed)
					{
						cout<<"give info to real buisiness to close"<<endl;		
					}
					else
					{
						cout<<"Handle thread id is "<<id()<<endl;
            			cout<<"handleImp recv uid  is "<<recv->uid<<endl;
					//sendResponse(recv->uid,recv->buffer, recv->ip, recv->port, recv->fd);
						handle(stRecvData);
					}
					delete recv;
            		recv = NULL;
        		}
			}
			catch(...)
			{
				if(recv)
            	{
            		close(recv->uid, recv->fd);
                	delete recv;
                	recv = NULL;
            	}
			}
		}
    }

}

void TC_EpollServer::Handle::setEpollServer(TC_EpollServer *pEpollServer)
{
    TC_ThreadLock::Lock lock(*this);

    _pEpollServer = pEpollServer;
}

void TC_EpollServer::Handle::setHandleGroup(TC_EpollServer::BindAdapterPtr& lsPtr)
{
    TC_ThreadLock::Lock lock(*this);

    _lsPtr = lsPtr;
}

void TC_EpollServer::Handle::setHandleGroup(TC_EpollServer::HandleGroupPtr& pHandleGroup)
{
    TC_ThreadLock::Lock lock(*this);

    _handleGroup = pHandleGroup;
}

TC_EpollServer::HandleGroupPtr& TC_EpollServer::Handle::getHandleGroup()
{
    return _handleGroup;
}

TC_EpollServer* TC_EpollServer::Handle::getEpollServer()
{
    return _pEpollServer;
}

TC_EpollServer::BindAdapter::BindAdapter(TC_EpollServer *pEpollServer)
: _pEpollServer(pEpollServer)
, _handleGroup(NULL)
, _handleGroupName("")
, _iHandleNum(0)
, _iHeartBeatTime(0)
{
}

TC_EpollServer::BindAdapter::~BindAdapter()
{
	_pEpollServer->terminate();
}

void TC_EpollServer::BindAdapter::setName(const string &name)
{
    TC_ThreadLock::Lock lock(*this);

    _name = name;
}

string TC_EpollServer::BindAdapter::getName() const
{
    return _name;
}

int TC_EpollServer::BindAdapter::getHandleNum()
{
    return _iHandleNum;
}

void TC_EpollServer::BindAdapter::setHandleNum(int n)
{
    TC_ThreadLock::Lock lock(*this);

    _iHandleNum = n;
}

void TC_EpollServer::BindAdapter::insertRecvQueue(const recv_queue::queue_type &vtRecvData, bool bPushBack)
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

    //TC_ThreadLock::Lock lock(monitor);
    //monitor.notify();
	TC_ThreadLock::Lock lock(_handleGroup->monitor);
	_handleGroup->monitor.notify();
	
}

bool TC_EpollServer::BindAdapter::waitForRecvQueue(tagRecvData* &recv, uint32_t iWaitTime)
{
    bool bRet = false;

    bRet = _rbuffer.pop_front(recv, iWaitTime);

    if(!bRet)
    {
        return bRet;
    }

    return bRet;
}

TC_EpollServer* TC_EpollServer::BindAdapter::getEpollServer()
{
    return _pEpollServer;
}

void TC_EpollServer::BindAdapter::setEndpoint(const string &str, const int &port)
{
    TC_ThreadLock::Lock lock(*this);

    _ep.init(str, port);
}

TC_Endpoint TC_EpollServer::BindAdapter::getEndpoint() const
{
    return _ep;
}

TC_Socket& TC_EpollServer::BindAdapter::getSocket()
{
    return _s;
}

void TC_EpollServer::BindAdapter::setHandleGroupName(const string& handleGroupName)
{
    _handleGroupName = handleGroupName;
}

string TC_EpollServer::BindAdapter::getHandleGroupName() const
{
    return _handleGroupName;
}

void TC_EpollServer::BindAdapter::setHeartBeatTime(time_t t)
{
    TC_ThreadLock::Lock lock(*this);

    _iHeartBeatTime = t;
}

time_t TC_EpollServer::BindAdapter::getHeartBeatTime() const
{
    return _iHeartBeatTime;
}

}

