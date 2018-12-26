#ifndef __TARS_NETWORK_UTIL_H_
#define __TARS_NETWORK_UTIL_H_

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <vector>

namespace tars
{
struct NetworkUtil
{
    static const int INVALID_SOCKET = -1;
    static const int SOCKET_ERROR = -1;

    static int createSocket(bool, bool isLocal = false);

    static void closeSocketNoThrow(int);

    static void setBlock(int, bool);

    static void setTcpNoDelay(int);

    static void setKeepAlive(int);

    static void doBind(int, struct sockaddr_in&);

    static bool doConnect(int, const struct sockaddr_in&);

    static void getAddress(const std::string&, int, struct sockaddr_in&);

    static std::string errorToString(int);
};

}

#endif
