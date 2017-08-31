/*
 * StopWatch.h
 *
 *  Created on: 2016年1月25日
 *      Author: terry
 */

#ifndef STOPWATCH_H_
#define STOPWATCH_H_

#include "ClockTime.h"

namespace comn
{

class StopWatch
{
public:
	StopWatch():
		m_clock()
	{
		m_clock = ClockTime::getTime();
	}

	virtual ~StopWatch()
	{

	}

	void reset()
	{
		m_clock = ClockTime::getTime();
	}

	uint64_t elapse() const
	{
		return ClockTime::getTime() - m_clock;
	}

	uint64_t getStartClock() const
	{
		return m_clock;
	}

protected:
	uint64_t	m_clock;

};


} /* namespace comn */

#endif /* STOPWATCH_H_ */
