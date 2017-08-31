/*
 * DateTime.cpp
 *
 *  Created on: 2011-9-30
 *      Author: terry
 */

#include "DateTime.h"

#include <ctime>

#include <stdio.h>
#include <string.h>


#ifdef WIN32
    #include <Winsock2.h>
#else
    #include <sys/time.h>
#endif //



namespace comn
{


#ifdef WIN32
/*
* Number of micro-seconds between the beginning of the Windows epoch
* (Jan. 1, 1601) and the Unix epoch (Jan. 1, 1970).
*
* This assumes all Win32 compilers have 64-bit support.
*/
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS) || defined(__WATCOMC__)
  #define DELTA_EPOCH_IN_USEC  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_USEC  11644473600000000ULL
#endif
typedef unsigned __int64 u_int64_t;

static u_int64_t filetime_to_unix_epoch (const FILETIME *ft)
{
    u_int64_t res = (u_int64_t) ft->dwHighDateTime << 32;

    res |= ft->dwLowDateTime;
    res /= 10;                   /* from 100 nano-sec periods to usec */
    res -= DELTA_EPOCH_IN_USEC;  /* from Win epoch to Unix epoch */
    return (res);
}

int DateTime::gettimeofday (struct timeval *tv, void *tz)
{
    FILETIME  ft;
    u_int64_t tim;

    if (!tv) {
        //errno = EINVAL;
        return (-1);
    }
    GetSystemTimeAsFileTime (&ft);
    tim = filetime_to_unix_epoch (&ft);
    tv->tv_sec  = (long) (tim / 1000000L);
    tv->tv_usec = (long) (tim % 1000000L);
    return (0);
}
#else

int DateTime::gettimeofday (struct timeval *tv, void *tz)
{
    return ::gettimeofday(tv, (struct timezone*)tz);
}
#endif //win32






DateTime DateTime::now()
{
	DateTime dt;
	dt.getTimeNow();
    return dt;
}

int64_t DateTime::getTime()
{
    struct timeval tp;
    DateTime::gettimeofday(&tp, NULL);
    int64_t ms = tp.tv_sec;
    ms += tp.tv_usec/1000;
    return ms;
}


DateTime::DateTime() :
    m_second(),
    m_millisecond()
{
}

DateTime::~DateTime()
{
}

DateTime::DateTime(time_t t):
	m_second(),
	m_millisecond()
{
    m_second = t;
    m_millisecond = 0;
}

DateTime::DateTime(time_t second, time_t ms):
    m_second(second),
    m_millisecond(ms)
{
}

DateTime::DateTime(struct tm& t, time_t ms):
	m_second(),
	m_millisecond()
{
    m_second = mktime(&t);
    m_millisecond = ms;
}

DateTime::DateTime(int nYear, int nMonth, int nDay,
				   int nHour, int nMin, int nSec, int ms):
	m_second(),
	m_millisecond(ms)		 
{
	struct tm t;
	memset(&t, 0, sizeof(t));
	t.tm_year = nYear - 1900;
	t.tm_mon = nMonth - 1;
	t.tm_mday = nDay;
	t.tm_hour = nHour;
	t.tm_min = nMin;
	t.tm_sec = nSec;

	m_second = mktime(&t);
}

time_t DateTime::totalSecond() const
{
    return m_second;
}

time_t DateTime::totalMillisecond() const
{
    return m_second * 1000 + m_millisecond;
}

std::string DateTime::toPreciseString() const
{
	char buffer[512] = {0};
	size_t length = 512;
	struct tm tmNow = getTm();

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
	length = sprintf_s(buffer, length, "%04d-%02d-%02d %02d:%02d:%02d.%d",
		(tmNow.tm_year + 1900), (tmNow.tm_mon + 1), tmNow.tm_mday,
		tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec, (int)m_millisecond );
#else
	length = sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d.%d",
		(tmNow.tm_year + 1900), (tmNow.tm_mon + 1), tmNow.tm_mday,
		tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec, (int)m_millisecond );
#endif//
	buffer[length] = 0;
	return buffer;
}

std::string DateTime::toString() const
{
	char buffer[512] = {0};
	size_t length = 512;
	length = format(buffer, length, "%Y-%m-%d %H:%M:%S");
	buffer[length] = 0;
    return buffer;
}

size_t DateTime::format(char* buffer, size_t length, const char* fmt) const
{
    struct tm tmNow;
    getTm(tmNow);

    return strftime(buffer, length, fmt, &tmNow);
}

std::string DateTime::format(const char* fmt) const
{
	char buffer[1024] = {0};
	size_t length = 1024;
	length = format(buffer, length, fmt);
	buffer[length] = 0;
	return buffer;
}


void DateTime::reset()
{
    getTimeNow();
}

time_t DateTime::getElapse() const
{
    return now().getElapse(*this);
}

time_t DateTime::getElapse( const DateTime& tmPrev ) const
{
    return subtract(tmPrev);
}

void DateTime::getTimeNow()
{
    struct timeval tp;
    DateTime::gettimeofday( &tp, NULL );
    m_second = tp.tv_sec;
    m_millisecond = tp.tv_usec/1000;
}

void DateTime::getTm(struct tm& tmNow) const
{
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
    localtime_s(&tmNow, &m_second);
#else
    tmNow = *localtime(&m_second);
#endif //
}

tm DateTime::getTm() const
{
    struct tm tmNow;
    getTm(tmNow);
    return tmNow;
}

int DateTime::getYear() const
{
    struct tm tmNow;
    getTm(tmNow);
    return tmNow.tm_year + 1900;
}

int DateTime::getMonth() const
{
    struct tm tmNow;
    getTm(tmNow);
    return tmNow.tm_mon + 1;
}

int DateTime::getDay() const
{
    struct tm tmNow;
    getTm(tmNow);
    return tmNow.tm_mday;
}

int DateTime::getWeekday() const
{
    struct tm tmNow;
    getTm(tmNow);
    return tmNow.tm_wday;
}

int DateTime::getHour() const
{
    struct tm tmNow;
    getTm(tmNow);
    return tmNow.tm_hour;
}

int DateTime::getMinute() const
{
    struct tm tmNow;
    getTm(tmNow);
    return tmNow.tm_min;
}

int DateTime::getSecond() const
{
    struct tm tmNow;
    getTm(tmNow);
    return tmNow.tm_sec;
}

int DateTime::getMillisecond() const
{
	return (int)m_millisecond;
}

const DateTime& DateTime::operator + (time_t seconds)
{
	m_second += seconds;
	return *this;
}

const DateTime& DateTime::operator += (time_t seconds)
{
	m_second += seconds;
	return *this;
}

const DateTime& DateTime::operator - (time_t seconds)
{
	m_second -= seconds;
	return *this;
}

const DateTime& DateTime::operator -= (time_t seconds)
{
	m_second -= seconds;
	return *this;
}

time_t DateTime::operator - (const DateTime& dt) const
{
	return m_second - dt.m_second;
}



bool DateTime::operator < (const DateTime& dt) const
{
	if (m_second == dt.m_second)
	{
		return (m_millisecond < dt.m_millisecond);
	}
	return (m_second < dt.m_second);
}

bool DateTime::operator == (const DateTime& dt) const
{
	return (m_second == dt.m_second) && (m_millisecond == dt.m_millisecond);
}

time_t DateTime::subtract(const DateTime& dt) const
{
	time_t seconds = (m_second - dt.m_second);
	time_t ms = m_millisecond - dt.m_millisecond;
	return seconds * 1000 + ms;
}

const DateTime& DateTime::subtract(time_t ms)
{
	time_t totalMS = m_second * 1000 + m_millisecond - ms;
	m_second = totalMS / 1000;
	m_millisecond = totalMS % 1000;
	return (*this);
}

const DateTime& DateTime::add(time_t ms)
{
	time_t totalMS = m_second * 1000 + m_millisecond + ms;
	m_second = totalMS / 1000;
	m_millisecond = totalMS % 1000;
	return (*this);
}


bool DateTime::isSameDate(const DateTime& dt) const
{
	struct tm tmSelf = getTm();
	struct tm tmObj = dt.getTm();
	return (tmSelf.tm_year == tmObj.tm_year) &&
		(tmSelf.tm_mon == tmObj.tm_mon) &&
		(tmSelf.tm_mday == tmObj.tm_mday);
}



}
