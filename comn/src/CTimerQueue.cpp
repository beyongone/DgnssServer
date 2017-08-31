/*
 * CTimerQueue.cpp
 *
 *  Created on: 2015年12月3日
 *      Author: terry
 */

#include "CTimerQueue.h"
#include <list>
#include <algorithm>
#include <functional>
#include <iostream>

#include "ClockTime.h"


namespace comn
{

struct IsThisID : public std::binary_function<TimerTask, TimerID, bool >
{
    bool operator () (const TimerTask& task, const TimerID& id) const
    {
        return (task.m_id == id);
    }
};

struct ExpireComparator: public std::binary_function< TimerTask, TimerTask, bool>
{
    bool operator () (const TimerTask& first, const TimerTask& task) const
    {
        return (first.m_expire < task.m_expire);
    }
};



CTimerQueue::CTimerQueue()
{
}

CTimerQueue::~CTimerQueue()
{
	destroy();
}

bool CTimerQueue::create()
{
	if (isOpen())
	{
		return true;
	}
	return start();
}

void CTimerQueue::destroy()
{
	if (isRunning())
	{
		stop();
	}

    clear();
}

bool CTimerQueue::isOpen()
{
	return isRunning();
}

bool CTimerQueue::addTimer(TimerID id, TimerHandler* handler, uint32_t ms)
{
	if (!handler)
	{
		return false;
	}

	TimerTask task(id, handler, ms, true);
	return add(task);
}

bool CTimerQueue::addElapse(TimerID id, TimerHandler* handler, uint32_t ms)
{
	if (!handler)
	{
		return false;
	}

	TimerTask task(id, handler, ms, false);
	return add(task);
}

bool CTimerQueue::cancel(TimerID id)
{
	return remove(id);
}

void CTimerQueue::clear()
{
	AutoCritSec lock(m_cs);
	m_taskList.clear();
}


bool CTimerQueue::add(TimerTask& task)
{
	bool found = remove(task.m_id);

	task.updateExpire(getCurTime());
	insertTask(task);

	m_event.post();

	return !found;
}

void CTimerQueue::insertTask(TimerTask& task)
{
	comn::AutoCritSec lock(m_cs);
	iterator it = findWithExpire(task.m_expire);
	m_taskList.insert(it, task);
}

bool CTimerQueue::runTask(TimerTask& task)
{
	if (task.m_handler)
	{
		return task.m_handler->onTimer(task.m_id, task.m_interval);
	}
	return false;
}

bool CTimerQueue::checkoutFront(TimerTask& task, int64_t& timeout)
{
    bool found = false;
    timeout = -1;
    comn::AutoCritSec lock(m_cs);
    if (!m_taskList.empty())
    {
        int64_t tmNow = getCurTime();
        task = m_taskList.front();
        if (task.m_expire <= (tmNow + DEVIATION))
        {
            m_taskList.pop_front();
            timeout = 0;
            found = true;
        }
        else
        {
            timeout = (task.m_expire - tmNow);
        }
    }
    return found;
}

int64_t CTimerQueue::checkExpire()
{
    int64_t timeout = -1;
    TimerTask task;
    if (checkoutFront(task, timeout))
    {
        bool done = runTask(task);

        if (done && task.m_isRepeat)
        {
            int64_t expire = task.updateExpire();
            if (expire < getCurTime())
            {
                task.updateExpire(getCurTime());
            }

            insertTask(task);
        }

        timeout = 0;
    }
    return timeout;
}

int CTimerQueue::run()
{
	int64_t timeout = -1;
	while (!m_canExit)
	{
		//Time watch;
		//std::cout << "timeout: " << timeout << std::endl;
		// it is not precise,error range is about [0-20] in win32
        timedwait(timeout);
        if (!m_canExit)
        {
            timeout = checkExpire();
        }
	}
	return 0;
}

void CTimerQueue::doStop()
{
	m_event.post();
}

CTimerQueue::iterator CTimerQueue::find(const TimerID& id)
{
	return std::find_if(m_taskList.begin(), m_taskList.end(),
			std::bind2nd(IsThisID(), id));
}

bool CTimerQueue::remove(const TimerID& id)
{
	comn::AutoCritSec lock(m_cs);

	bool found = false;
	iterator it = find(id);
	if (it != m_taskList.end())
	{
		m_taskList.erase(it);
		found = true;
	}
	return found;
}

CTimerQueue::iterator CTimerQueue::findWithExpire(int64_t expire)
{
	TimerTask first;
	first.m_expire = expire;
	return std::upper_bound(m_taskList.begin(), m_taskList.end(),
			first, ExpireComparator());
}

int64_t CTimerQueue::getCurTime()
{
	return ClockTime::getTime();
}

bool CTimerQueue::timedwait(int64_t ms)
{
    if (ms == 0)
    {
        return true;
    }
    return m_event.timedwait((int)ms);
}



} /* namespace comn */
