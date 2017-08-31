/*
 * DateTime.h
 *
 *  Created on: 2011-9-30
 *      Author: terry
 */

#ifndef DATETIME_H_
#define DATETIME_H_

#include "BasicType.h"

#include <string>
#include <time.h>

#ifndef WIN32
#include <sys/time.h>
#endif //

namespace comn
{


class  DateTime
{
public:
    static int gettimeofday (struct timeval *tv, void *tz);

    static DateTime now();

    static int64_t getTime();

	enum Seconds
	{
		kMinute = 60,
		kHour = 3600,
		kDay = 3600 * 24,
		kWeek = 3600 * 24 * 7
	};

public:
    DateTime();
    ~DateTime();

    explicit DateTime(time_t t);
    DateTime(time_t second, time_t ms);
    DateTime(struct tm& t, time_t ms);
	DateTime(int nYear, int nMonth, int nDay,
		int nHour, int nMin, int nSec, int ms);


    time_t totalSecond() const;
    time_t totalMillisecond() const;

    struct tm getTm() const;
    void getTm(struct tm& tmNow) const;

	std::string toPreciseString() const;

    std::string toString() const;

    size_t format(char* buffer, size_t length, const char* fmt) const;
	std::string format(const char* fmt) const;

    void reset();

    /**
     * get elapse in milliseconds
     */
    time_t getElapse() const;

    /**
     * get elapse in milliseconds
     */
    time_t getElapse( const DateTime& tmPrev ) const;

    int getYear() const;
    int getMonth() const;
    int getDay() const;
    int getWeekday() const;
    int getHour() const;
    int getMinute() const;
    int getSecond() const;
	int getMillisecond() const;

	/// 算术操作符以秒为单位
	const DateTime& operator + (time_t seconds);

	const DateTime& operator += (time_t seconds);

	const DateTime& operator - (time_t seconds);

	const DateTime& operator -= (time_t seconds);

	time_t operator - (const DateTime& dt) const;


	bool operator < (const DateTime& dt) const;

	bool operator == (const DateTime& dt) const;

	
	/**
	 * 减去时间，返回总共毫秒数
	 * @param dt 
	 * @return time_t 
	 */
	time_t subtract(const DateTime& dt) const;

	const DateTime& subtract(time_t ms);

	const DateTime& add(time_t ms);

	
	bool isSameDate(const DateTime& dt) const;

protected:
    void getTimeNow();

    
protected:
    time_t  m_second;
    time_t  m_millisecond;

};

}


#endif /* DATETIME_H_ */
