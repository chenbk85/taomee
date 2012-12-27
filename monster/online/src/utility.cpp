/*
 * =====================================================================================
 *
 *       Filename:  utility.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2010年08月24日 13时17分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

extern "C"
{
#include <libtaomee/log.h>
}

#include <boost/random.hpp>
#include "utility.h"

static boost::kreutzer1986  rng(time(NULL));
static boost::uniform_real<> dist(0, 1);
static boost::variate_generator<boost::kreutzer1986&, boost::uniform_real<> > uniform_sampler(rng, dist);

int timestamp2day(uint32_t timestamp)
{
    struct tm tm_time;
    time_t tmp_timestamp = (time_t)timestamp;
    if(NULL == localtime_r(&tmp_timestamp, &tm_time))
    {   
        CRIT_LOG("localtime failed, time:%u", tmp_timestamp);
        return -1;
    }   

    return (tm_time.tm_year + 1900 - 2000) * 10000 + (tm_time.tm_mon + 1) * 100 + tm_time.tm_mday;
}

int timestamp_equal_day(uint32_t timestamp1, uint32_t timestamp2)
{
    assert(timestamp1 != 0 && timestamp2 != 0);
    int day1 = timestamp2day(timestamp1); 
    if (day1 < 0)
    {
        return -1; 
    }

    int day2 = timestamp2day(timestamp2); 
    if (day2 < 0)
    {
        return -1; 
    }

    return day1 == day2 ? 1 : 0;
}

int min_max(int v, int min, int max)
{
    if (v > max)
    {
        return max;
    }
    else if (v < min)
    {
        return min;
    }
    else
    {
        return v;
    }
}

/**
 * @brief 四舍五入，-2.5得到-3
 *
 * @param x
 *
 * @return
 */
int float2int(float x)
{
    if (x > 0)
    {   
        return (int)(x + 0.5);
    }   
    else if (x < 0)
    {   
        return (int)(x - 0.5);
    }   
    else
    {   
        return 0;
    }   
}

int uniform_rand(int min, int max)
{
    // 传回样本分布结果
    return (float2int(uniform_sampler() * (max - min) + min));
}

bool string_is_digit(char *p_str)
{
    while (*p_str != 0)
    {
        if (!isdigit(*p_str))
        {
            return false;
        }
        ++p_str;
    }

    return true;
}

uint32_t timestamp_begin_day()
{
    struct tm ts;
    time_t tmp_time = time(NULL);
    if (NULL == localtime_r(&tmp_time, &ts))
    {    
        return 0;
    }    

    return tmp_time - ts.tm_hour * 60 * 60 - ts.tm_min * 60 - ts.tm_sec;
}
