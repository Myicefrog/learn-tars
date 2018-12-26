#include <cerrno>
#include <iostream>
#include "tc_clientsocket.h"

namespace tars
{

TC_Endpoint::TC_Endpoint()
{
    _host = "0.0.0.0";
    _port = 0;
}

void TC_Endpoint::init(const string& host, int port)
{
    _host = host;
    _port = port;
}




}

