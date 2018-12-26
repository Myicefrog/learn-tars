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

#include "tc_socket.h"

namespace tars
{

TC_Socket::TC_Socket() : _sock(INVALID_SOCKET), _bOwner(true), _iDomain(AF_INET)
{
}

TC_Socket::~TC_Socket()
{
    if(_bOwner)
    {
        close();
    }
}

void TC_Socket::init(int fd, bool bOwner, int iDomain)
{
    if(_bOwner)
    {
        close();
    }

    _sock       = fd;
    _bOwner     = bOwner;
    _iDomain    = iDomain;
}

void TC_Socket::createSocket(int iSocketType, int iDomain)
{
    assert(iSocketType == SOCK_STREAM || iSocketType == SOCK_DGRAM);
    close();

    _iDomain    = iDomain;
    _sock       = socket(iDomain, iSocketType, 0);

    if(_sock < 0)
    {
        _sock = INVALID_SOCKET;
	cout<<"create socket error"<<endl;
    }
}


int TC_Socket::accept(TC_Socket &tcSock, struct sockaddr *pstSockAddr, socklen_t &iSockLen)
{
    assert(tcSock._sock == INVALID_SOCKET);

    int ifd;

    while ((ifd = ::accept(_sock, pstSockAddr, &iSockLen)) < 0 && errno == EINTR);

    tcSock._sock    = ifd;
    tcSock._iDomain = _iDomain;

    return tcSock._sock;
}


void TC_Socket::bind(const string &sServerAddr, int port)
{
    assert(_iDomain == AF_INET);

    struct sockaddr_in bindAddr;

    bzero(&bindAddr, sizeof(bindAddr));

    bindAddr.sin_family   = _iDomain;
    bindAddr.sin_port     = htons(port);

    if (sServerAddr == "")
    {
        bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        parseAddr(sServerAddr, bindAddr.sin_addr);
    }

    try
    {
        bind((struct sockaddr *)(&bindAddr), sizeof(bindAddr));
    }
    catch(...)
    {
	cout<<"[TC_Socket::bind] bind error"<<endl;
    }
}

void TC_Socket::parseAddr(const string &sAddr, struct in_addr &stSinAddr)
{
    int iRet = inet_pton(AF_INET, sAddr.c_str(), &stSinAddr);
    if(iRet < 0)
    {
        cout<<"[TC_Socket::parseAddr] inet_pton error"<<endl;;
    }
    else if(iRet == 0)
    {
        struct hostent stHostent;
        struct hostent *pstHostent;
        char buf[2048] = "\0";
        int iError;

        gethostbyname_r(sAddr.c_str(), &stHostent, buf, sizeof(buf), &pstHostent, &iError);

        if (pstHostent == NULL)
        {
            cout<<"[TC_Socket::parseAddr] gethostbyname_r error! :"<<endl;
        }
        else
        {
            stSinAddr = *(struct in_addr *) pstHostent->h_addr;
        }
    }
}

void TC_Socket::bind(struct sockaddr *pstBindAddr, socklen_t iAddrLen)
{
    //如果服务器终止后,服务器可以第二次快速启动而不用等待一段时间
    int iReuseAddr = 1;

    //设置
    setSockOpt(SO_REUSEADDR, (const void *)&iReuseAddr, sizeof(int), SOL_SOCKET);

    if(::bind(_sock, pstBindAddr, iAddrLen) < 0)
    {
        cout<<"[TC_Socket::bind] bind error"<<endl;
    }
}

void TC_Socket::close()
{
    if (_sock != INVALID_SOCKET)
    {
        ::close(_sock);
        _sock = INVALID_SOCKET;
    }
}


int TC_Socket::connectNoThrow(const string &sServerAddr, uint16_t port)
{
    assert(_iDomain == AF_INET);

    if (sServerAddr == "")
    {
        cout<<"[TC_Socket::connect] server address is empty error!"<<endl;
    }

    struct sockaddr_in serverAddr;
    bzero(&serverAddr, sizeof(serverAddr));

    serverAddr.sin_family = _iDomain;
    parseAddr(sServerAddr, serverAddr.sin_addr);
    serverAddr.sin_port = htons(port);

    return connect((struct sockaddr *)(&serverAddr), sizeof(serverAddr));
}


void TC_Socket::connect(const string &sServerAddr, uint16_t port)
{
    int ret = connectNoThrow(sServerAddr, port);

    if(ret < 0)
    {
        cout<<"[TC_Socket::connect] connect error"<<endl;
    }
}

int TC_Socket::connect(struct sockaddr *pstServerAddr, socklen_t serverLen)
{
    return ::connect(_sock, pstServerAddr, serverLen);

}

void TC_Socket::listen(int iConnBackLog)
{
    if (::listen(_sock, iConnBackLog) < 0)
    {
        cout<<"[TC_Socket::listen] listen error"<<endl;
    }
}


void TC_Socket::setblock(bool bBlock)
{
    assert(_sock != INVALID_SOCKET);

    setblock(_sock, bBlock);
}

void TC_Socket::setblock(int fd, bool bBlock)
{
    int val = 0;

    if ((val = fcntl(fd, F_GETFL, 0)) == -1)
    {
        cout<<"[TC_Socket::setblock] fcntl [F_GETFL] error"<<endl;
    }

    if(!bBlock)
    {
        val |= O_NONBLOCK;
    }
    else
    {
        val &= ~O_NONBLOCK;
    }

    if (fcntl(fd, F_SETFL, val) == -1)
    {
        cout<<"[TC_Socket::setblock] fcntl [F_SETFL] error"<<endl;
    }
}


int TC_Socket::setSockOpt(int opt, const void *pvOptVal, socklen_t optLen, int level)
{
    return setsockopt(_sock, level, opt, pvOptVal, optLen);
}


void TC_Socket::setKeepAlive()
{
    int flag = 1;
    if(setSockOpt(SO_KEEPALIVE, (char*)&flag, int(sizeof(int)), SOL_SOCKET) == -1)
    {
        cout<<"[TC_Socket::setKeepAlive] error"<<endl;
    }
}

void TC_Socket::setTcpNoDelay()
{
    int flag = 1;

    if(setSockOpt(TCP_NODELAY, (char*)&flag, int(sizeof(int)), IPPROTO_TCP) == -1)
    {
        cout<<"[TC_Socket::setTcpNoDelay] error"<<endl;
    }
}

void TC_Socket::setNoCloseWait()
{
    linger stLinger;
    stLinger.l_onoff = 1;  //在close socket调用后, 但是还有数据没发送完毕的时候容许逗留
    stLinger.l_linger = 0; //容许逗留的时间为0秒

    if(setSockOpt(SO_LINGER, (const void *)&stLinger, sizeof(linger), SOL_SOCKET) == -1)
    {
        cout<<"[TC_Socket::setNoCloseWait] error"<<endl;
    }
}

void TC_Socket::setCloseWaitDefault()
{
    linger stLinger;
    stLinger.l_onoff  = 0;
    stLinger.l_linger = 0;

    if(setSockOpt(SO_LINGER, (const void *)&stLinger, sizeof(linger), SOL_SOCKET) == -1)
    {
        cout<<"[TC_Socket::setCloseWaitDefault] error"<<endl;
    }
}

int TC_Socket::recvfrom(void *pvBuf, size_t iLen, string &sFromAddr, uint16_t &iFromPort, int iFlags)
{
    struct sockaddr stFromAddr;
    socklen_t iFromLen = sizeof(struct sockaddr);
    struct sockaddr_in *p = (struct sockaddr_in *)&stFromAddr;

    bzero(&stFromAddr, sizeof(struct sockaddr));

    int iBytes = recvfrom(pvBuf, iLen, &stFromAddr, iFromLen, iFlags);
    if (iBytes >= 0)
    {
        char sAddr[INET_ADDRSTRLEN] = "\0";

        inet_ntop(_iDomain, &p->sin_addr, sAddr, sizeof(sAddr));

        sFromAddr = sAddr;
        iFromPort = ntohs(p->sin_port);
    }

    return iBytes;
}

int TC_Socket::recvfrom(void *pvBuf, size_t iLen, struct sockaddr *pstFromAddr, socklen_t &iFromLen, int iFlags)
{
    return ::recvfrom(_sock, pvBuf, iLen, iFlags, pstFromAddr, &iFromLen);
}

}
