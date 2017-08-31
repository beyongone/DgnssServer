/*    file: main.cpp
 *    desc:
 * 
 * created: 2015-12-03
 *  author: chuanjiang.zh@qq.com
 * company: 
 */


#include <stdio.h>
#include <iostream>

#include "CTimerQueue.h"
#include "CLog.h"
#include "TStringCast.h"

using namespace comn;


class Demo : public TimerHandler
{
public:
    explicit Demo(TimerQueue& que)
    {

    }
    
    virtual bool onTimer(const TimerID& id, uint32_t& interval)
    {
        printf("%s timer: %s, interval:%d\n",
            CLog::getShortTime(),
            id.c_str(), interval);
        
        if (interval == 0)
        {
            return false;
        }

        if (interval > 5000)
        {
            return false;
        }

        if (interval == 3000)
        {
            interval += 10;
        }

        return true;
    }

};

int main(int argc, char** argv)
{
    CTimerQueue que;
    Demo demo(que);

    que.create();

    printf("%s\n", CLog::getShortTime());

    for (int i = 0; i < 5; i ++)
    {
        TimerID id = comn::StringCast::toString(i);
        que.addTimer(id, &demo, i * 1000);
    }

    std::cin.get();

    que.destroy();

	return 0;
}

