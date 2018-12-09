#ifndef _TC_CLIENTSOCKET_H__
#define _TC_CLIENTSOCKET_H__

#include <sstream>
#include <string>
#include "tc_ex.h"

using namespace std;

namespace tars
{

struct TC_EndpointParse_Exception : public TC_Exception
{
    TC_EndpointParse_Exception(const string &buffer) : TC_Exception(buffer){};
    ~TC_EndpointParse_Exception() throw() {};
};

class TC_Endpoint
{
public:
	TC_Endpoint();

	TC_Endpoint(const string& host, int port)
	{
		init(host,port);
	}

	TC_Endpoint(const TC_Endpoint& l)
	{
        _host   = l._host;
        _port   = l._port;
	}

    TC_Endpoint& operator = (const TC_Endpoint& l)
    {
        if(this != &l)
        {
            _host   = l._host;
            _port   = l._port;
        }

        return *this;
    }

	void setHost(const string& host)    { _host = host; }

    string getHost() const              { return _host; }

	void setPort(int port)              { _port = port; }

    int getPort() const                 { return _port; }

	void init(const string& host, int port);

protected:

	std::string _host;

    int         _port;

};


}

#endif
