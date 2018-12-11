#include "NetworkUtil.h"
#include "Global.h"
//#include "tc_epoller.h"

#include <sys/epoll.h>
#include <sstream>
#include <assert.h>
#include <string.h>

using namespace std;
using namespace tars;

int NetworkUtil::createSocket(bool udp, bool isLocal/* = false*/)
{
    int fd = socket((isLocal ? PF_LOCAL : PF_INET), SOCK_STREAM, IPPROTO_TCP);

    if (fd == INVALID_SOCKET)
    {
        ostringstream os;
        os << "createSocket ex:(" << errorToString(errno) << ")" << __FILE__ << ":" << __LINE__;
        throw TarsNetSocketException(os.str());
    }

    if(!udp)
    {
        setTcpNoDelay(fd);

        setKeepAlive(fd);
    }

    return fd;
}

void NetworkUtil::closeSocketNoThrow(int fd)
{
    int error = errno;
    close(fd);
    errno = error;
}

void NetworkUtil::setBlock(int fd, bool block)
{
    if (block)
    {
        int flags = fcntl(fd, F_GETFL);
        flags &= ~O_NONBLOCK;
        if (fcntl(fd, F_SETFL, flags) == SOCKET_ERROR)
        {
            closeSocketNoThrow(fd);
            ostringstream os;
            os << "setBlock ex:(" << errorToString(errno) << ")" << __FILE__ << ":" << __LINE__;
            throw TarsNetSocketException(os.str());
        }
    }
    else
    {
        int flags = fcntl(fd, F_GETFL);
        flags |= O_NONBLOCK;
        if (fcntl(fd, F_SETFL, flags) == SOCKET_ERROR)
        {
            closeSocketNoThrow(fd);
            ostringstream os;
            os << "setBlock ex:(" << errorToString(errno) << ")" << __FILE__ << ":" << __LINE__;
            throw TarsNetSocketException(os.str());
        }
    }
}

void NetworkUtil::setTcpNoDelay(int fd)
{
    int flag = 1;
    if(setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, int(sizeof(int))) == SOCKET_ERROR)
    {
        closeSocketNoThrow(fd);
        ostringstream os;
        os << "setTcpNoDelay ex:(" << errorToString(errno) << ")" << __FILE__ << ":" << __LINE__;
        throw TarsNetSocketException(os.str());
    }
}

void NetworkUtil::setKeepAlive(int fd)
{
    int flag = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&flag, int(sizeof(int))) == SOCKET_ERROR)
    {
        closeSocketNoThrow(fd);
        ostringstream os;
        os << "setKeepAlive ex:(" << errorToString(errno) << ")" << __FILE__ << ":" << __LINE__;
        throw TarsNetSocketException(os.str());
    }
}

void NetworkUtil::doBind(int fd, struct sockaddr_in& addr)
{
    if(bind(fd, reinterpret_cast<struct sockaddr*>(&addr), int(sizeof(addr))) == SOCKET_ERROR)
    {
        closeSocketNoThrow(fd);
        ostringstream os;
        os << "doBind ex:(" << errorToString(errno) << ")" << __FILE__ << ":" << __LINE__;
        throw TarsNetSocketException(os.str());
    }
    socklen_t len = static_cast<socklen_t>(sizeof(addr));
    getsockname(fd, reinterpret_cast<struct sockaddr*>(&addr), &len);
}

bool NetworkUtil::doConnect(int fd, const struct sockaddr_in& addr)
{
    bool bConnected = false;

	cout<<"doConnect fd is "<<fd<<endl;

    int iRet = ::connect(fd, (struct sockaddr*)(&addr), int(sizeof(addr)));

	cout<<"doConnect iRet is "<<iRet<<endl;
	cout<<"doConnect errno is "<<errno<<endl;

    if (iRet == 0)
    {
        bConnected  = true;
    }
	//EINPROGRESS=115 当链接设置为非阻塞时，目标没有及时应答，返回此错误，socket可以继续使
    else if (iRet == -1 && errno != EINPROGRESS)
    {
        ::close(fd);
        throw TarsNetConnectException(strerror(errno));
    }

    return bConnected;
}

void NetworkUtil::getAddress(const string& host, int port, struct sockaddr_in& addr)
{
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    addr.sin_addr.s_addr = inet_addr(host.c_str());

    if(addr.sin_addr.s_addr == INADDR_NONE)
    {
        struct addrinfo* info = 0;
        int retry = 5;

        struct addrinfo hints = { 0 };
        hints.ai_family = PF_INET;

        int rs = 0;
        do
        {
            rs = getaddrinfo(host.c_str(), 0, &hints, &info);
        }
        while(info == 0 && rs == EAI_AGAIN && --retry >= 0);

        if(rs != 0)
        {
            ostringstream os;
            os << "DNSException ex:(" << errorToString(errno) << ")" << rs << ":" << host << ":" << __FILE__ << ":" << __LINE__;
            if(info != NULL)
            {
                freeaddrinfo(info);
            }
            throw TarsNetSocketException(os.str());
        }

        assert(info != NULL);
        assert(info->ai_family == PF_INET);
        struct sockaddr_in* sin = reinterpret_cast<sockaddr_in*>(info->ai_addr);

        addr.sin_addr.s_addr = sin->sin_addr.s_addr;
        freeaddrinfo(info);
    }
}

string NetworkUtil::errorToString(int error)
{
    return strerror(error);
}
