//#include "util/tc_epoll_server.h"
#include "AppProtocal.h"
#include "Tars.h"
#include <iostream>

namespace tars
{

//TARSServer的协议解析器
int AppProtocol::parseAdmin(string &in, string &out)
{
    return parse(in, out);
}

void ProxyProtocol::tarsRequest(const RequestPacket& request, string& buff)
{
    TarsOutputStream<BufferWriter> os;

    request.writeTo(os);

    tars::Int32 iHeaderLen = htonl(sizeof(tars::Int32) + os.getLength());

    buff.clear();

    buff.reserve(sizeof(tars::Int32) + os.getLength());

    buff.append((const char*)&iHeaderLen, sizeof(tars::Int32));

    buff.append(os.getBuffer(), os.getLength());
}
}
