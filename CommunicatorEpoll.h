#ifndef __TARS__COMMUNICATOR_EPOLL_H_
#define __TARS__COMMUNICATOR_EPOLL_H_

#include "tc_thread.h"
#include "tc_thread_mutex.h"
#include "tc_epoller.h"
#include "tc_socket.h"
#include "Message.h"
#include <set>

namespace tars
{

class Communicator;
class ObjectProxy;
class ObjectProxyFactory;

struct FDInfo
{
    enum
    {
        ET_C_NOTIFY = 1,
        ET_C_NET    = 2,
    };

    /**
     * 构造函数
     */
    FDInfo()
    : iSeq(0)
    , fd(-1)
    , iType(ET_C_NOTIFY)
    , p(NULL)
    {
    }

    /**
     * 析构函数
     */
    ~FDInfo()
    {
    }

    size_t iSeq;

    int    fd;

    int    iType;

    void * p;
};

class Communicator;


class CommunicatorEpoll : public TC_Thread
{

    struct NotifyInfo
    {
        /**
         * 构造函数
         */
        NotifyInfo()
        : eventFd(-1)
        , bValid(false)
        {
        }

        /**
         * 析构函数
         */
        ~NotifyInfo()
        {
        }

        
        FDInfo    stFDInfo;   //通知FD信息
        
        TC_Socket notify;     //通知fd

        int       eventFd;    //eventfd,目前未使用
        
        bool      bValid;     //是否有效
    };

public:
	
	CommunicatorEpoll(size_t _netThreadSeq);

	CommunicatorEpoll(Communicator * pCommunicator, size_t _netThreadSeq);

	virtual ~CommunicatorEpoll();

	ObjectProxy * getObjectProxy(const string& ip, const uint16_t& port);

	virtual void run();

	void terminate();

	void addFd(int fd,FDInfo * info, uint32_t events);

	void delFd(int fd,FDInfo * info, uint32_t events);

	void notify(size_t iSeq,ReqInfoQueue * pReqQueue);

	void notifyDel(size_t iSeq);

	void pushAsyncThreadQueue(ReqMessage * msg);

protected:

	void handle(FDInfo * pFDInfo, uint32_t events);

	void handleInputImp(Transceiver * pTransceiver);

	void handleOutputImp(Transceiver * pTransceiver);
	
protected:

	Communicator *         _communicator;

	NotifyInfo             _notify[2048];

	TC_Socket              _shutdown;

	bool                   _terminate;

	TC_Epoller             _ep;

	size_t 				   _asyncThreadNum;

	size_t                 _asyncSeq;

	size_t                 _netThreadSeq;

	ObjectProxyFactory *   _objectProxyFactory;

	AsyncProcThread *      _asyncThread[1024];
};


}
#endif
