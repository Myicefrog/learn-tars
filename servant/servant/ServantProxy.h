#ifndef _TARS_SERVANT_PROXY_H_
#define _TARS_SERVANT_PROXY_H_

#include <string>
#include "Message.h"
#include "tc_monitor.h"
#include "CommunicatorEpoll.h"


using namespace std;

namespace tars
{

class SeqManager
{
public:
    const static uint16_t MAX_UNSIGN_SHORT = 0xffff;

    struct SeqInfo
    {
        uint16_t next;
        bool     free;
    };

    /**
     * 构造函数
     */
    SeqManager(size_t iNum);

    /**
     * 获取seq
     */
    uint16_t get();

    /**
     * 删除seq
     */
    void del(uint16_t iSeq);

private:
    uint16_t    _num;

    uint16_t    _free;

    uint16_t    _freeTail;

    SeqInfo  *  _p;
};

class ServantProxyThreadData
{
public:
    static TC_ThreadMutex _mutex;  //全局的互斥锁
    static pthread_key_t  _key;    //私有线程数据key
	static SeqManager *   _pSeq;

    /**
     * 构造函数
     */
    ServantProxyThreadData();

    /**
     * 析构函数
     */
    ~ServantProxyThreadData();

    /**
     * 数据资源释放
     * @param p
     */
    static void destructor(void* p);

    /**
     * 获取线程数据，没有的话会自动创建
     * @return ServantProxyThreadData*
     */
    static ServantProxyThreadData * getData();

public:

    ReqInfoQueue * _reqQueue[64]; //队列数组
    bool           _queueInit;                       //是否初始化
    size_t         _reqQNo;                          //请求事件通知的seq
    size_t         _netSeq;                          //轮训选择网络线程的偏移量
    int            _netThreadSeq;                     //网络线程发起的请求回到自己的网络线程来处理,其值为网络线程的id

    size_t         _objectProxyNum;                  //ObjectProxy对象的个数，其个数由客户端的网络线程数决定，每个网络线程有一个ObjectProxy
    ObjectProxy ** _objectProxy;                    //保存ObjectProxy对象的指针数组

    CoroutineScheduler*        _sched;                   //协程调度器

};

class ServantProxyCallback
{
public:
	
	ServantProxyCallback();

	virtual ~ServantProxyCallback() {}

	virtual const string& getType() { return _type; }

	virtual void setType(const string& type) { _type = type; }

	inline void setNetThreadProcess(bool bNetThreadProcess)
    {
        _bNetThreadProcess = bNetThreadProcess;
    }

	inline bool getNetThreadProcess()
    {
        return _bNetThreadProcess;
    }


public:

    virtual int onDispatch(ReqMessagePtr msg) = 0;

protected:

	string _type;

	bool _bNetThreadProcess;

};

class ServantProxy : public TC_ThreadMutex
{
public:

	ServantProxy(Communicator * pCommunicator, ObjectProxy ** ppObjectProxy, size_t iClientThreadNum);

	virtual ~ServantProxy();

    virtual void tars_invoke(const string& sFuncName,
                            const string& request,
                            string& resp);

	virtual void tars_invoke_async(const string& sFuncName,
							const string& request,
							const ServantProxyCallbackPtr& callback);


public:

	Communicator* tars_communicator() { return _communicator; }

private:

	void invoke(ReqMessage * msg, bool bCoroAsync = false);

	void selectNetThreadInfo(ServantProxyThreadData * pSptd, ObjectProxy * & pObjProxy, ReqInfoQueue * & pReqQ);


private:
    
	friend class ObjectProxy;

	Communicator *            _communicator;

	ObjectProxy **            _objectProxy;

	size_t                    _objectProxyNum;

	int                       _queueSize;

};

}

#endif
