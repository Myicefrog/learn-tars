#ifndef __TC_THREAD_H_
#define __TC_THREAD_H_

#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include "tc_monitor.h"
#include "tc_ex.h"


namespace tars
{

struct TC_ThreadThreadControl_Exception : public TC_Exception
{
    TC_ThreadThreadControl_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_ThreadThreadControl_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_ThreadThreadControl_Exception() throw() {};
};


class TC_ThreadControl
{
public:

    TC_ThreadControl();

    explicit TC_ThreadControl(pthread_t);

    pthread_t id() const;

    void join();

    void detach();

    static void sleep(long millsecond);

    static void yield();

private:

    pthread_t _thread;


};

class TC_Runable
{
public:
    virtual ~TC_Runable(){};
    virtual void run() = 0;
};

class TC_Thread : public TC_Runable
{
public:

    TC_Thread();

    virtual ~TC_Thread(){};

    TC_ThreadControl start();

    TC_ThreadControl getThreadControl() const;

    bool isAlive() const;

    pthread_t id() { return _tid; }

protected:

    static void threadEntry(TC_Thread *pThread);

    virtual void run() = 0;

    bool            _running;

    pthread_t        _tid;

    TC_ThreadLock   _lock;

};


}

#endif
