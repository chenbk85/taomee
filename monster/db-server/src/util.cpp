/*
 * =====================================================================================
 *
 *       Filename:  util.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月20日 15时46分45秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <boost/random.hpp>

extern "C"
{
    #include <libtaomee/log.h>
}

#include "util.h"

static boost::kreutzer1986  rng(time(NULL));
static boost::uniform_real<> dist(0, 1);
static boost::variate_generator<boost::kreutzer1986&, boost::uniform_real<> > uniform_sampler(rng, dist);

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

int str2uint(uint32_t *p_uint, const char *p_str)
{
    if (NULL == p_str || NULL == p_uint)
    {
        return -1;
    }

    for (uint32_t i = 0; i < strlen(p_str); i++)
    {
        if (!isdigit(p_str[i]))
        {
            ERROR_LOG("p_str:%s include nondigit", p_str);
            *p_uint = 0;
            return -1;
        }
    }

    std::string str(p_str);

    std::istringstream istr(str);

    istr >> std::dec >> *p_uint;
    if (!istr)
    {
        ERROR_LOG("p_str:%s", p_str);
        *p_uint = 0;
        return -1;
    }

    return 0;
}

int str2uint(uint16_t *p_uint, const char *p_str)
{
    uint32_t temp = 0;
    if (0 != str2uint(&temp, p_str))
    {
        *p_uint = 0;
        return -1;
    }
    *p_uint = (uint16_t)temp;
    return 0;
}

int str2uint(uint8_t *p_uint, const char *p_str)
{
    uint32_t temp = 0;
    if (0 != str2uint(&temp, p_str))
    {
        *p_uint = 0;
        return -1;
    }
    *p_uint = (uint8_t)temp;
    return 0;
}

int intip2str(uint32_t ip, char *ipstr, int ipstr_len)
{
    if(inet_ntop(AF_INET, &ip, ipstr, ipstr_len) == NULL)
    {
        ERROR_LOG("inet_ntop(%u) failed(%s).", ip, strerror(errno));
        return -1;
    }
    return 0;
}
