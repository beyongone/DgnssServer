/*
 * ClockTime.cpp
 *
 *  Created on: 2015年5月29日
 *      Author: chuanjiang.zh@qq.com
 */

#include "ClockTime.h"
#include <time.h>

namespace comn
{

ClockTime::ClockTime()
{
}

ClockTime::~ClockTime()
{
}

uint64_t ClockTime::getTime()
{
#ifdef WIN32
    return GetTickCount();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    int64_t count = ts.tv_sec;
    count *= 1000;
    count += ts.tv_nsec / 1000000;
    return count;
#endif //WIN32
}




} /* namespace comn */
