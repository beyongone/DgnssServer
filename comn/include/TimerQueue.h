/*
 * TimerQueue.h
 *
 *  Created on: 2015年1月14日
 *      Author: zhengchuanjiang
 */

#ifndef CTIMERQUEUE_H_
#define CTIMERQUEUE_H_

#include "BasicType.h"
#include <string>
#include <time.h>


namespace comn
{

typedef std::string TimerID;


class TimerHandler
{
public:
	virtual ~TimerHandler() {}

	/**
	 * 计时器处理
	 * @param id	计时器ID
	 * @param interval	计时器间隔
	 * @return	是否继续定时, false 表示结束
	 */
	virtual bool onTimer(const TimerID& id, uint32_t& interval) =0;

};


class TimerQueue
{
public:
	virtual ~TimerQueue() {}

	/**
	 * create queue
	 * return true if success
	 */
	virtual bool create() =0;

	/**
	 * destroy queue
	 */
	virtual void destroy() =0;


	virtual bool isOpen() =0;

	virtual bool addTimer(TimerID id, TimerHandler* handler, uint32_t ms) =0;

	virtual bool addElapse(TimerID id, TimerHandler* handler, uint32_t ms) =0;

	virtual bool cancel(TimerID id) =0;

	/**
	 * cancel all timer
	 */
	virtual void clear() =0;


};





} /* namespace comn */

#endif /* CTIMERQUEUE_H_ */
