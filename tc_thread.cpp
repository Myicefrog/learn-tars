#include "tc_thread.h"
#include <iostream>
#include <cerrno>

namespace tars
{

TC_ThreadControl::TC_ThreadControl(pthread_t thread) : _thread(thread)
{
}

TC_ThreadControl::TC_ThreadControl() : _thread(pthread_self())
{
}

void TC_ThreadControl::join()
{
    if(pthread_self() == _thread)
    {
        throw TC_ThreadThreadControl_Exception("[TC_ThreadControl::join] can't be called in the same thread");
    }

    void* ignore = 0;
    int rc = pthread_join(_thread, &ignore);
    if(rc != 0)
    {
        throw TC_ThreadThreadControl_Exception("[TC_ThreadControl::join] pthread_join error ", rc);
    }
}

void TC_ThreadControl::detach()
{
    if(pthread_self() == _thread)
    {
        throw TC_ThreadThreadControl_Exception("[TC_ThreadControl::join] can't be called in the same thread");
    }

    int rc = pthread_detach(_thread);
    if(rc != 0)
    {
        throw TC_ThreadThreadControl_Exception("[TC_ThreadControl::join] pthread_join error", rc);
    }
}

pthread_t TC_ThreadControl::id() const
{
    return _thread;
}

void TC_ThreadControl::sleep(long millsecond)
{
    struct timespec ts;
    ts.tv_sec = millsecond / 1000;
    ts.tv_nsec = (millsecond % 1000)*1000000;
    nanosleep(&ts, 0);
}

void TC_ThreadControl::yield()
{
    sched_yield();
}

TC_Thread::TC_Thread() : _running(false),_tid(-1)
{
}

void TC_Thread::threadEntry(TC_Thread *pThread)
{
    pThread->_running = true;

    {
        TC_ThreadLock::Lock sync(pThread->_lock);
        cout<<"pThread->_lock.notifyAll()"<<endl;
        pThread->_lock.notifyAll();
    }

    try
    {
        pThread->run();
    }
    catch(...)
    {
        pThread->_running = false;
        throw;
    }
    pThread->_running = false;
}

TC_ThreadControl TC_Thread::start()
{
    TC_ThreadLock::Lock sync(_lock);

    if(_running)
    {
        throw TC_ThreadThreadControl_Exception("[TC_Thread::start] thread has start");
    }

    int ret = pthread_create(&_tid,
                   0,
                   (void *(*)(void *))&threadEntry,
                   (void *)this);

    if(ret != 0)
    {
        throw TC_ThreadThreadControl_Exception("[TC_Thread::start] thread start error", ret);
    }

    _lock.wait();

    return TC_ThreadControl(_tid);
}

TC_ThreadControl TC_Thread::getThreadControl() const
{
    return TC_ThreadControl(_tid);
}

bool TC_Thread::isAlive() const
{
    return _running;
}



}
