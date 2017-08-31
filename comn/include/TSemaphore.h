/*
 * TSemaphore.h
 *
 *  Created on: 2009-11-11
 *      Author: terry
 */

#ifndef TSEMAPHORE_H_
#define TSEMAPHORE_H_
/////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#else
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>
#endif //WIN32

namespace comn
{

#ifndef MAX_SEM_VALUE
#define MAX_SEM_VALUE 1000000
#endif //MAX_SEM_VALUE
#ifdef WIN32
class Semaphore
{
public:
    Semaphore( int resource = 0 )
    {
        m_handle = ::CreateSemaphore( NULL, (LONG)resource, MAX_SEM_VALUE, NULL );
    }

    ~Semaphore()
    {
        ::CloseHandle( m_handle );
    }

    void post()
    {
        ::ReleaseSemaphore( m_handle, 1, NULL );
    }

    /**
     * wait for semaphore
     * @param timeout period in milliseconds, -1 means infinite
     * @return false if timed out
     */
    bool wait( int timeout )
    {
        DWORD dwMilliseconds = timeout;
        if ( timeout < 0 )
        {
            dwMilliseconds = INFINITE;
        }
        return ( WAIT_OBJECT_0 == ::WaitForSingleObject( m_handle, dwMilliseconds) );
    }

private:
    HANDLE m_handle;

};

#else

class Semaphore
{
public:
    Semaphore(int resource)
    {
        sem_init(&m_sem, 0, resource);
    }

    ~Semaphore()
    {
        sem_destroy(&m_sem);
    }

    void post()
    {
        sem_post(&m_sem);
    }

    bool wait(int timeout)
    {
        if (timeout < 0)
        {
            return (0 == sem_wait(&m_sem));
        }
        else if (timeout == 0)
        {
            return (0 == sem_trywait(&m_sem));
        }
        else
        {
            struct timespec ts;
            getTimeout(&ts, timeout);
            return (0 == sem_timedwait(&m_sem, &ts));
        }
    }

    bool tryWait()
    {
        return (0 == sem_trywait(&m_sem));
    }

    int getValue()
    {
        int val = 0;
        sem_getvalue(&m_sem, &val);
        return val;
    }

    static void getTimeout(struct timespec *spec, int timer)
    {
        struct timeval current;
        gettimeofday(&current, NULL);
        TIMEVAL_TO_TIMESPEC( &current, spec );
        spec->tv_sec = current.tv_sec + ((timer + current.tv_usec / 1000)
                / 1000);
        spec->tv_nsec = ((current.tv_usec / 1000 + timer) % 1000) * 1000000;
    }
private:
    sem_t m_sem;

};

class SimulatedSemaphore
{
public:
    SimulatedSemaphore(int resource = 0)
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutex_init(&m_mutex, &attr);
        pthread_mutexattr_destroy(&attr);
        pthread_cond_init(&m_cond, NULL);

        m_count = resource;
        m_waiters = 0;
    }

    ~SimulatedSemaphore()
    {
        pthread_mutex_unlock(&m_mutex);
        pthread_cond_destroy(&m_cond);
        pthread_mutex_destroy(&m_mutex);
    }

    void post()
    {
        pthread_mutex_lock(&m_mutex);
        if (m_waiters > 0)
        {
            pthread_cond_signal(&m_cond);
        }
        ++m_count;
        pthread_mutex_unlock(&m_mutex);
    }

    bool wait(int timeout)
    {
        struct timespec ts;
        bool flag = true;
        int rc;
        pthread_mutex_lock(&m_mutex);
        ++m_waiters;

        if (m_count)
            goto exiting;

        if (!timeout)
        {
            while (m_count == 0)
                pthread_cond_wait(&m_cond, &m_mutex);
            goto exiting;
        }

        Semaphore::getTimeout(&ts, timeout);
        rc = pthread_cond_timedwait(&m_cond, &m_mutex, &ts);
        if (rc == ETIMEDOUT || m_count == 0)
            flag = false;

        exiting: --m_waiters;
        if (m_count)
            --m_count;
        pthread_mutex_unlock(&m_mutex);
        return flag;
    }

    int getValue()
    {
        pthread_mutex_lock(&m_mutex);
        int value = m_count;
        pthread_mutex_unlock(&m_mutex);
        return value;
    }

private:
    int m_count;
    int m_waiters;
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;

};

#endif //WIN32
} // end of namespace

/////////////////////////////////////////////////////////////////////////////
#endif /* TSEMAPHORE_H_ */
