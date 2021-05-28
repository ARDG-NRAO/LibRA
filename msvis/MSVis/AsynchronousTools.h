/*
 * AsynchronousTools.h
 *
 *  Created on: Nov 1, 2010
 *      Author: jjacobs
 */

#ifndef ASYNCHRONOUSTOOLS_H_
#define ASYNCHRONOUSTOOLS_H_

#include <casa/aips.h>
#include <casa/aipstype.h>
#include <casa/BasicSL/String.h>
#include <mutex>
#include "UtilJ.h"

#include <map>
#include <queue>

using std::map;
using std::mutex;
using std::queue;

namespace casa {

namespace async {

class MutexImpl;

class Mutex {

    friend class Condition;
    friend class UniqueLock;

public:

    Mutex ();
    virtual ~Mutex ();

    ///casacore::Bool isLockedByThisThread () const; // for debug purposes only !!!

    void lock ();
    //casacore::Bool lock (casacore::Int milliseconds);
    void unlock ();
    bool trylock ();
    
    // jagonzal: Useful when mandatory is locking
    void acquirelock();

protected:

    std::mutex & getMutex ();

private:

    bool        isLocked_p;
    MutexImpl * impl_p;

    Mutex (const Mutex & other); // illegal operation: do not define
    Mutex operator= (const Mutex & other); // illegal operation: do not define

};

class LockGuard {

    friend class LockGuardInverse;

public:

    LockGuard (Mutex & mutex);
    LockGuard (Mutex * mutex);

    virtual ~LockGuard ();

private:

    Mutex * mutex_p;

};

class LockGuardInverse {

public:

    LockGuardInverse (Mutex & mutex);
    LockGuardInverse (Mutex * mutex);
    LockGuardInverse (LockGuard & lg);

    virtual ~LockGuardInverse ();

private:

    Mutex * mutex_p;

};


class MutexLocker {

public:

    MutexLocker (Mutex & mutex);
    MutexLocker (Mutex * mutex);

    virtual ~MutexLocker ();

private:

    Mutex * mutex_p;

    MutexLocker (const MutexLocker & other); // do not define
    MutexLocker & operator= (const MutexLocker & other); // do not define

};

class ConditionImpl;
class UniqueLock;

class Condition {

public:

    Condition ();
    virtual ~Condition ();

    void broadcast () __attribute__ ((deprecated)) /*"Use notify_all */;
    void signal () __attribute__ ((deprecated)) /* Use notify_one */;

    void notify_all ();
    void notify_one ();
    void wait (UniqueLock & uniqueLock);
    // casacore::Bool wait (Mutex & mutex, int milliseconds);

private:

    ConditionImpl * impl_p;
};

class SemaphoreImpl;

class Semaphore {

public:

    Semaphore (int initialValue = 0);
    ~Semaphore ();

    casacore::Int getValue ();
    void post ();
    casacore::Bool trywait ();
    void wait ();
    casacore::Bool wait (int milliseconds);

private:

    SemaphoreImpl * impl_p;
    casacore::String name_p;

    Semaphore (const Semaphore & other); // illegal operation: do not define
    Semaphore operator= (const Semaphore & other); // illegal operation: do not define

};

class Thread {

public:

    typedef void * (* ThreadFunction) (void *);

    Thread ();
    virtual ~Thread ();

    pthread_t getId () const;
    pid_t gettid () const; // linux only
    bool isTerminationRequested () const;
    void * join ();
    void startThread ();
    virtual void terminate ();

protected:

    bool isStarted () const;
    virtual void * run () = 0;

    static void * threadFunction (void *);

private:

    pthread_t * id_p;
    bool started_p;
    volatile bool terminationRequested_p;

};

class UniqueLock {

    friend class Condition;

public:

    UniqueLock (Mutex & mutex);

    void lock ();
    void unlock ();

private:

    std::unique_lock<std::mutex> uniqueLock_p;
};

class Logger {

public:

    // make noncopyable...
    Logger( const Logger& ) = delete;
    Logger& operator=( const Logger& ) = delete;

    void log (const char * format, ...);
    void registerName (const casacore::String & threadName);
    void start (const char * logFilename);

    static Logger * get ();

protected:

    class LoggerThread : public Thread {
    public:

        LoggerThread ();
        ~LoggerThread ();

        void log (const string & text);
        void setLogFilename (const casacore::String & filename);
        void terminate ();

    protected:

        void * run ();

    private:

        bool      deleteStream_p;
        casacore::String    logFilename_p;
        Condition loggerChanged_p;
        std::ostream * logStream_p;
        Mutex mutex_p;
        queue<std::string> outputQueue_p;
    };


//    void log (char * format, ...);
//    void setLogFilename (char * logFilename);
//    void terminate ();

private:

    typedef map <pthread_t, casacore::String>  ThreadNames;

    LoggerThread * loggerThread_p;
    bool loggingStarted_p;
    Mutex * nameMutex_p;
    ThreadNames threadNames_p;

    static Logger * singleton_p;

    Logger (); // singleton
    ~Logger ();

    static void initialize ();
};


}

}


#endif /* ASYNCHRONOUSTOOLS_H_ */
