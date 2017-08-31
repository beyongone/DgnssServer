/*
 * ClockTime.h
 *
 *  Created on: 2015年5月29日
 *      Author: chuanjiang.zh@qq.com
 */

#ifndef CLOCKTIME_H_
#define CLOCKTIME_H_

#include "BasicType.h"

namespace comn
{

class ClockTime
{
public:
    ClockTime();
    virtual ~ClockTime();

    /**
     * 获取系统启动时间，单位为毫秒
     * @return
     */
    static uint64_t getTime();

};




} /* namespace comn */

#endif /* CLOCKTIME_H_ */
