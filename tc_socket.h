#ifndef __TC_SOCKET_H
#define __TC_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <string>
#include <sys/un.h>
#include <iostream>

using namespace std;

namespace tars
{

class TC_Socket
{
public:
	TC_Socket();
	virtual ~TC_Socket();

	void init(int fd, bool bOwner, int iDomain = AF_INET);

	void createSocket(int iSocketType = SOCK_STREAM, int iDomain = AF_INET);

	int getfd() const { return _sock; }

	bool isValid() const { return _sock != -1; }

	void close();

	void setOwner(bool bOwner)  { _bOwner = bOwner; }

	int setSockOpt(int opt, const void *pvOptVal, socklen_t optLen, int level = SOL_SOCKET);

	int accept(TC_Socket &tcSock, struct sockaddr *pstSockAddr, socklen_t &iSockLen);

	void bind(const string &sServerAddr, int port);

	//void bind(const char *sPathName);

	void bind(struct sockaddr *pstBindAddr, socklen_t iAddrLen);

	void connect(const string &sServerAddr, uint16_t port);

	int connect(struct sockaddr *pstServerAddr, socklen_t serverLen);  

	int connectNoThrow(const string &sServerAddr, uint16_t port);

	void listen(int connBackLog);

	void setKeepAlive();

	static void setblock(int fd, bool bBlock);

	void setblock(bool bBlock = false);

	void parseAddr(const string &sAddr, struct in_addr &stSinAddr);

	void setTcpNoDelay();

	void setNoCloseWait();

	void setCloseWaitDefault();

	int recvfrom(void *pvBuf, size_t iLen, string &sFromAddr, uint16_t &iFromPort, int iFlags = 0);

	int recvfrom(void *pvBuf, size_t iLen, struct sockaddr *pstFromAddr, socklen_t &iFromLen, int iFlags = 0);

protected:
    	static const int INVALID_SOCKET = -1;

    	int  _sock;

    	bool _bOwner;

    	int  _iDomain;

};

}

#endif
