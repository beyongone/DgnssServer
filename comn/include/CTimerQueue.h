/*
 * CTimerQueue.h
 *
 *  Created on: 2015年12月3日
 *      Author: terry
 */

#ifndef INCLUDE_CTIMERQUEUE_H_
#define INCLUDE_CTIMERQUEUE_H_

#include "TimerQueue.h"
#include "TThread.h"
#include "TCriticalSection.h"
#include "TEvent.h"
#include <list>


namespace comn
{

struct TimerTask
{
	TimerID	m_id;
	TimerHandler*	m_handler;
    uint32_t  m_interval;
    bool      m_isRepeat;
    int64_t   m_expire;

    TimerTask():
    	m_id(),
		m_handler(),
        m_interval(0),
        m_isRepeat(false),
        m_expire(0)
    {
    }

    TimerTask(const TimerID& id, TimerHandler* handler, uint32_t interval, bool repeat):
    	m_id(id),
		m_handler(handler),
        m_interval(interval),
        m_isRepeat(repeat),
        m_expire(interval)
    {
    }

    int64_t updateExpire()
    {
        m_expire += m_interval;
        return m_expire;
    }

    int64_t updateExpire(int64_t timeBase)
    {
        m_expire = m_interval + timeBase;
        return m_expire;
    }

    bool operator < (const TimerTask& task) const
    {
        return (m_expire < task.m_expire);
    }

};


class CTimerQueue: public TimerQueue, public comn::Thread
{
public:
    static const uint32_t DEVIATION = 10;   /// 计时器误差, 毫秒

public:
	CTimerQueue();
	virtual ~CTimerQueue();

	virtual bool create();

	/**
	 * destroy queue
	 */
	virtual void destroy();


	virtual bool isOpen();

	virtual bool addTimer(TimerID id, TimerHandler* handler, uint32_t ms);

	virtual bool addElapse(TimerID id, TimerHandler* handler, uint32_t ms);

	virtual bool cancel(TimerID id);

	virtual void clear();


protected:
	virtual int run();
	virtual void doStop();


	typedef std::list< TimerTask >  TimerTaskList;
	typedef TimerTaskList::iterator iterator;

	iterator find(const TimerID& id);
	bool remove(const TimerID& id);
	iterator findWithExpire(int64_t expire);

	int64_t getCurTime();
    bool add(TimerTask& task);
	void insertTask(TimerTask& task);
    bool runTask(TimerTask& task);

    bool timedwait(int64_t ms);
    int64_t checkExpire();

    bool checkoutFront(TimerTask& task, int64_t& timeout);

protected:

	TimerTaskList     m_taskList;
    CriticalSection   m_cs;
    Event       m_event;


};


} /* namespace comn */

#endif /* INCLUDE_CTIMERQUEUE_H_ */
