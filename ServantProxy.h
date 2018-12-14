#ifndef _TARS_SERVANT_PROXY_H_
#define _TARS_SERVANT_PROXY_H_

#include <string>
#include "Message.h"
#include "tc_monitor.h"


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

	size_t         _reqQNo;                          //请求事件通知的seq

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

}

#endif
