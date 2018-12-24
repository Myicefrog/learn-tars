#ifndef __TARS_GLOBAL_H_
#define __TARS_GLOBAL_H_

#include <inttypes.h>
#include <iostream>
#include <sstream>
#include <map>
#include <list>
#include <deque>
#include <vector>
#include <memory>
#include <netinet/in.h>
#include <errno.h>
#include <iomanip>
#include <sys/ipc.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

#include "tc_ex.h"

using namespace std;

namespace tars
{
class Communicator;
class AdapterProxy;
class ServantProxy;
class ServantProxyCallback;
class ObjectProxy;
class Transceiver;
class ServantProxyFactory;
class ObjectProxyFactory;
class AsyncProcThread;

typedef shared_ptr<ServantProxy> ServantPrx;
typedef shared_ptr<ServantProxyCallback> ServantProxyCallbackPtr;
typedef shared_ptr<ObjectProxyFactory> ObjectProxyFactoryPtr;
typedef shared_ptr<Communicator> CommunicatorPtr;

struct TarsException : public TC_Exception
{
    TarsException(const string &buffer) : TC_Exception(buffer){};
    TarsException(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TarsException() throw(){};
    /**
     * 根据返回值抛出异常
     * @param ret
     * @param desc
     */
    static void throwException(int ret, const string& desc = "");
};

struct TarsNetSocketException : public TarsException
{
    TarsNetSocketException(const string &buffer) : TarsException(buffer){};
    TarsNetSocketException(const string &buffer, int err) : TarsException(buffer, err){};
    ~TarsNetSocketException() throw(){};
};

struct TarsNetConnectException : public TarsException
{
    TarsNetConnectException(const string &buffer) : TarsException(buffer){};
    TarsNetConnectException(const string &buffer, int err) : TarsException(buffer, err){};
    ~TarsNetConnectException() throw(){};
};

}

#endif
