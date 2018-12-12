#ifndef __TARS_ASYNC_PROC_THREAD_H_
#define __TARS_ASYNC_PROC_THREAD_H_

#include "Message.h"
#include "tc_thread.h"

namespace tars
{

class AsyncProcThread : public TC_Thread, public TC_ThreadLock
{
public:

	AsyncProcThread(size_t iQueueCap = 10000);

	virtual ~AsyncProcThread();
	
	void terminate();

	void push_back(ReqMessage * msg);

	void run();

    size_t getSize()
    {
        return _msgQueue->size();
    }

private:

	bool            _terminate;

	ReqInfoQueue *  _msgQueue;

};

}

#endif
