#ifndef __TARS_MESSAGE_H_
#define __TARS_MESSAGE_H_
#include "Global.h"
#include "tc_loop_queue.h"
#include "tc_monitor.h"
#include "CoroutineScheduler.h"
#include <string>
#include <memory>

using namespace std;

namespace tars
{

struct ReqMonitor : public TC_ThreadLock
{
};

struct ReqMessage
{

    //调用类型
    enum CallType
    {
        SYNC_CALL = 1, //同步
        ASYNC_CALL,    //异步
        ONE_WAY,       //单向
        THREAD_EXIT    //线程退出的标识
    };

    //请求状态
    enum ReqStatus
    {
        REQ_REQ = 0,    //状态正常,正在请求中
        REQ_RSP,        //请求已经发出去
        REQ_TIME,       //请求超时
        REQ_BUSY,       //请求队列满了
        REQ_EXC         //客户端请求异常
    };

    /*
     * 构造函数
     */
    ReqMessage()
    : eStatus(ReqMessage::REQ_REQ)
    , eType(SYNC_CALL)
	, callback(NULL)
	, pObjectProxy(NULL)
    , pMonitor(NULL)
    , bMonitorFin(false)
    , iBeginTime(0)
	, iRequestId(1)
    {
    }

    /*
     * 析构函数
     */
    ~ReqMessage()
    {
        if(pMonitor != NULL)
        {
            delete pMonitor;
            pMonitor = NULL;
        }
    }

    /*
     * 初始化
     */
	void init(CallType eCallType)
    {
        eStatus        = ReqMessage::REQ_REQ;
        eType          = eCallType;
		callback       = NULL;
        sReqData.clear();
        pMonitor       = NULL;
        bMonitorFin    = false;

		iBeginTime     = 0;
		iRequestId     = 1;
    }


    ReqStatus                   eStatus;        //调用的状态
    CallType                    eType;          //调用类型

	ServantProxyCallbackPtr     callback;

	ObjectProxy *               pObjectProxy;   //调用端的proxy对象

    string                      request;        //请求消息体
    string                      response;       //响应消息体

    string                      sReqData;       //请求消息体

    ReqMonitor *                pMonitor;        //用于同步的monitor
    bool                        bMonitorFin;    //同步请求timewait是否结束

	int64_t                     iBeginTime;

	int                         iRequestId;
};
typedef shared_ptr<ReqMessage> ReqMessagePtr;
typedef TC_LoopQueue< ReqMessage*,1000 >  ReqInfoQueue;
}

#endif
