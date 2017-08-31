/*    file: TMutex.h
 *    desc:
 *   
 * created: 2009-09-02 09:44:15
 *  author: terry
 * version: 1.0
 * company: 
 */

#if !defined TMUTEX_TERRY_
#define TMUTEX_TERRY_

////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include <windows.h>
#else //
#include <pthread.h>
#endif //WIN32
/////////////////////////////////////////////////////////////////////////////

namespace comn
{

#ifdef WIN32

class Mutex
{
public:
    typedef HANDLE MutexType;
    typedef HANDLE MutexHandle;

    Mutex()
    {
        m_object = ::CreateMutex( NULL, false, NULL );
    }

    ~Mutex()
    {
        ::CloseHandle( m_object );
    }

    void lock()
    {
        ::WaitForSingleObject( m_object, INFINITE );
    }

    void unlock()
    {
        ::ReleaseMutex( m_object );
    }

    bool trylock()
    {
        DWORD ret = ::WaitForSingleObject( m_object, 0 );
        return ( WAIT_OBJECT_0 == ret );
    }

private:
    MutexType m_object;

    /// disable copy and assign
    Mutex( const Mutex& mtx );
    Mutex& operator = ( const Mutex& mtx );

};

#else

/// cross platform mutex
class Mutex
{
public:
    typedef pthread_mutex_t MutexType;
    typedef pthread_mutex_t* MutexHandle;

    explicit Mutex(bool recursive = true)
    {
        create(recursive);
    }

    ~Mutex()
    {
        destroy();
    }

    void lock()
    {
        ::pthread_mutex_lock(&m_object);
    }

    bool trylock()
    {
        return (0 == ::pthread_mutex_trylock(&m_object));
    }

    void unlock()
    {
        ::pthread_mutex_unlock(&m_object);
    }

    MutexHandle getHandle()
    {
        return &m_object;
    }

private:
    /// disable copy and assign
    Mutex(const Mutex& mtx);
    Mutex& operator =(const Mutex& mtx);

protected:
    pthread_mutexattr_t m_attr;
    MutexType m_object;

    int create(bool recursive)
    {
        int ret = ::pthread_mutexattr_init(&m_attr);
        if (0 == ret)
        {
            if (recursive)
            {
                ::pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE);
            }
            ret = ::pthread_mutex_init(&m_object, &m_attr);
        }
        return ret;
    }

    void destroy()
    {
        ::pthread_mutex_destroy(&m_object);
        ::pthread_mutexattr_destroy(&m_attr);
    }

};

#endif //WIN32

/// auto mutex
class AutoMutex
{
public:
    explicit AutoMutex(Mutex& mtx) :
        m_locked(false), m_mutex(mtx)
    {
        m_mutex.lock();
        m_locked = true;
    }

    ~AutoMutex()
    {
        unlock();
    }

    void unlock()
    {
        if (m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    bool m_locked;
    Mutex& m_mutex;

    AutoMutex(const AutoMutex& mtx);
    AutoMutex& operator =(const AutoMutex& mtx);

};

} // end of namespace
////////////////////////////////////////////////////////////////////////////
#endif //TMUTEX_TERRY_
