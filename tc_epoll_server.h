#ifndef __TARS_TC_EPOLL_SERVER_H_
#define __TARS_TC_EPOLL_SERVER_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>

#include <netinet/in.h>

#include "tc_epoller.h"
#include "tc_socket.h"

using namespace std;

namespace tars
{

class NetThread
{
public:

    NetThread();
    
    virtual ~NetThread();	

    int bind(string& ip, int& port);

    void run();		

    void createEpoll(uint32_t iIndex = 0);

    bool accept(int fd);

    void processNet(const epoll_event &ev);

    void processPipe();

    enum
    {
        ET_LISTEN = 1,
        ET_CLOSE  = 2,
        ET_NOTIFY = 3,
        ET_NET    = 0,
    };

    struct
    {
        string response;
        uint32_t uid;	
    }_response;

private:

    TC_Socket                 _shutdown;
    TC_Socket                 _notify;

    TC_Socket                 _bind_listen;

    TC_Epoller                _epoller;

    string                    _recvbuffer;

    string                    response;

    map<int,int>              _listen_connect_id;

    list<uint32_t>            _free;

    volatile size_t           _free_size;
};

}

#endif
