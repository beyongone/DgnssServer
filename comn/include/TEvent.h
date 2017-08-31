/*
 * TEvent.h
 *
 *  Created on: 2009-11-11
 *      Author: terry
 */

#ifndef TEVENT_H_
#define TEVENT_H_
/////////////////////////////////////////////////////////////////////////////
#include <assert.h>

#ifdef WIN32
#else
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <semaphore.h>
#endif //WIN32


#ifndef TIMEVAL_TO_TIMESPEC
#define TIMEVAL_TO_TIMESPEC(tv, ts) {                   \
        (ts)->tv_sec = (tv)->tv_sec;                    \
        (ts)->tv_nsec = (tv)->tv_usec * 1000;               \
}
#endif //TIMEVAL_TO_TIMESPEC


namespace comn
{

#ifdef WIN32
class Event
{
public:
    typedef HANDLE EventType;
    typedef HANDLE EventHandle;

    Event()
    {
        open();
    }

    ~Event()
    {
        close();
    }

    bool post()
    {
        return ( 0 != ::SetEvent( m_object ) );
    }

    // return true if event is set
    bool wait()
    {
        return timedwait( INFINITE );
    }

    // return true if event is set
    bool timedwait( int millisec )
    {
        bool ok = false;
        DWORD ret = ::WaitForSingleObject( m_object, millisec );
        if ( WAIT_OBJECT_0 == ret )
        {
            ::ResetEvent( m_object );
            ok = true;
        }
        return ok;
    }

    // return true if event is set
    bool trywait()
    {
        return timedwait( 0 );
    }

    EventHandle getHandle()
    {
        return m_object;
    }

    void reset()
    {
        ::ResetEvent( m_object );
    }

protected:
    bool open()
    {
        m_object = CreateEvent( NULL, TRUE, FALSE, NULL );
        return ( NULL != m_object );
    }

    void close()
    {
        ::CloseHandle( m_object );
        m_object = NULL;
    }

    EventType m_object;
};

#else
/////////////////////////////////////////////////////////////////////////////


class ConditionEvent
{
public:
    ConditionEvent()
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutex_init(&m_mutex, &attr);
        pthread_mutexattr_destroy(&attr);
        pthread_cond_init(&m_cond, NULL);
        m_signaled = false;
    }

    ~ConditionEvent()
    {
        pthread_cond_destroy(&m_cond);
        pthread_mutex_destroy(&m_mutex);
    }

    bool post()
    {
        pthread_mutex_lock(&m_mutex);
        m_signaled = true;
        pthread_cond_broadcast(&m_cond);
        pthread_mutex_unlock(&m_mutex);
        return true;
    }

    bool wait()
    {
        return timedwait(-1);
    }

    bool trywait()
    {
        return timedwait(0);
        /// another implement
        //		pthread_mutex_lock(&m_mutex);
        //		bool ok = m_signaled;
        //
        //		if ( m_signaled )
        //		{
        //			m_signaled = false;
        //		}
        //		pthread_mutex_unlock(&m_mutex);
        //		return ok;
    }

    bool timedwait(int millisec)
    {
        bool ok = false;

        pthread_mutex_lock(&m_mutex);

        struct timespec spec;
        while (!m_signaled)
        {
            if (millisec >= 0)
            {
                getTimeout(&spec, millisec);
                int rc = pthread_cond_timedwait(&m_cond, &m_mutex, &spec);
                if (rc == ETIMEDOUT)
                {
                    break;
                }
            }
            else
            {
                pthread_cond_wait(&m_cond, &m_mutex);
            }
        }

        ok = m_signaled;

        if (m_signaled)
        {
            m_signaled = false;
        }

        pthread_mutex_unlock(&m_mutex);

        return ok;
    }

    void reset()
    {
        m_signaled = false;
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
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
    bool m_signaled;
};

////////////////////////////////////////////////////////////////////////////
class SemaphoreEvent
{
public:
    SemaphoreEvent()
    {
        sem_init(&m_sem, 0, 0);
    }

    ~SemaphoreEvent()
    {
        sem_destroy(&m_sem);
    }

    bool post()
    {
        if (getValue() <= 0)
        {
            return (0 == sem_post(&m_sem));
        }
        return true;
    }

    bool wait()
    {
        return (0 == sem_wait(&m_sem));
    }

    bool trywait()
    {
        return (0 == sem_trywait(&m_sem));
    }

    bool timedwait(int timeout)
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
            ConditionEvent::getTimeout(&ts, timeout);
            return (0 == sem_timedwait(&m_sem, &ts));
        }
    }

    /// reset to nonsignal
    void reset()
    {
        int val = 0;
        sem_getvalue(&m_sem, &val);
        while (val > 0)
        {
            if (0 != sem_post(&m_sem))
            {
                break;
            }
            sem_getvalue(&m_sem, &val);
        }
    }

    int getValue()
    {
        int val = 0;
        sem_getvalue(&m_sem, &val);
        return val;
    }

private:
    sem_t m_sem;
};

////////////////////////////////////////////////////////////////////////////

typedef ConditionEvent Event;
//typedef SemaphoreEvent	Event;
////////////////////////////////////////////////////////////////////////////

#endif //WIN32

} // end of namespace

////////////////////////////////////////////////////////////////////////////
#endif /* TEVENT_H_ */
