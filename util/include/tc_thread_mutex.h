#ifndef __TC_THREAD_MUTEX_H
#define __TC_THREAD_MUTEX_H

#include <iostream>
#include <pthread.h>

using namespace std;

namespace tars
{

class TC_ThreadCond;

class TC_ThreadMutex
{
public:

    TC_ThreadMutex();
    virtual ~TC_ThreadMutex();

    void lock() const;

    bool tryLock() const;

    void unlock() const;

    bool willUnlock() const { return true;}

protected:

    // noncopyable
    TC_ThreadMutex(const TC_ThreadMutex&);
    void operator=(const TC_ThreadMutex&);

    int count() const;

    void count(int c) const;

    friend class TC_ThreadCond;

    mutable pthread_mutex_t _mutex;
};


/*
class TC_ThreadRecMutex
{
public:

    TC_ThreadRecMutex();

    virtual ~TC_ThreadRecMutex();

    int lock() const;

    int unlock() const;

    bool tryLock() const;

    bool willUnlock() const;
protected:

    friend class TC_ThreadCond;

    int count() const;

    void count(int c) const;

private:
    mutable pthread_mutex_t _mutex;
    mutable int _count;
};
*/
}

#endif
