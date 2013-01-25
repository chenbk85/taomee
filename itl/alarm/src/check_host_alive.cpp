/**
 * =====================================================================================
 *       @file  check_hostalive.cpp
 *      @brief  
 *
 *  使用ping命令检查一个主机是否down了
 *
 *   @internal
 *     Created  01/12/2011 05:40:03 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  mason, mason@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <strings.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <libtaomee/log.h>

#include "check_host_alive.h"


const char* const PING_COMMAND = "/bin/ping -n -U -w%u -c5 %s"; //发5个包
const unsigned int MAX_INPUT_BUFFER = 1024;

const float DEFAULT_WTT = 3000.0;    //缺省警戒间歇时间WARNING ROUND TRIP TIME 毫秒
const float DEFAULT_CTT = 5000.0;    //缺省严重警戒间歇时间CRITICAL ROUND TRIP TIME 毫秒
const int   DEFAULT_WPL = 80;        //缺省警戒丢包率 百分比
const int   DEFAULT_CPL = 100;       //缺省严重警戒丢包率 百分比


int error_scan(char * buf, const char * host_name);

/**
 * @brief   使用ping命令探测主机
 * @param   host       host
 * @param   timeout    超时值
 * @return  STATE_OK | STATE_WARNING | STATE_CRITICAL
 */
int check_host_alive(const char * host, unsigned int timeout)
{
    if (host == NULL) 
    {
        return -1;
    }

    char  buf[MAX_INPUT_BUFFER] = {0};
    char  cmd[256] = {0};
    float rta = -1.0; //round-trip average time 单位是ms
    int   pl = 101;//丢包率

    snprintf(cmd, sizeof(cmd), PING_COMMAND, timeout == 0 ? 5 : timeout, host);
    DEBUG_LOG("ping cmd=[%s]", cmd);

    FILE * fp = popen(cmd, "r");
    if (NULL == fp)
    {
        ERROR_LOG("popen failed, check host: %s", host);
        return -1;
    }

    while (fgets(buf, sizeof(buf), fp))
    {
        //把结尾的换行符去掉，打在日志里挺乱的
        char * endline  = index(buf, '\n');
        if (NULL != endline)
        {
            *endline = 0;
        }

        if (error_scan(buf, host) == STATE_CRITICAL) 
        {
            DEBUG_LOG("catch error[%s]", buf);
            pclose(fp);
            return STATE_CRITICAL;
        }

        //获取丢包率
        if (sscanf(buf, "%*d packets transmitted, %*d packets received, +%*d errors, %d%% packet loss", &pl) > 0 ||
                sscanf(buf, "%*d packets transmitted, %*d packets received, +%*d duplicates, %d%% packet loss", &pl) > 0 ||          
                sscanf(buf, "%*d packets transmitted, %*d received, +%*d duplicates, %d%% packet loss", &pl) > 0 ||
                sscanf(buf, "%*d packets transmitted, %*d packets received, %d%% packet loss", &pl) > 0 ||
                sscanf(buf, "%*d packets transmitted, %*d packets received, %d%% loss, time", &pl) > 0 ||
                sscanf(buf, "%*d packets transmitted, %*d received, %d%% loss, time", &pl) > 0 ||
                sscanf(buf, "%*d packets transmitted, %*d received, %d%% packet loss, time", &pl) > 0 ||
                sscanf(buf, "%*d packets transmitted, %*d received, +%*d errors, %d%% packet loss", &pl) > 0 ||
                sscanf(buf, "%*d packets transmitted %*d received, +%*d errors, %d%% packet loss", &pl) > 0) 
        { 
            DEBUG_LOG("stdout: %s, packets loss rate: %d%%", buf, pl);
            continue;
        } 
        else if (sscanf(buf, "round-trip min/avg/max = %*f/%f/%*f", &rta) == 1 ||
                sscanf(buf, "round-trip min/avg/max/mdev = %*f/%f/%*f/%*f", &rta) == 1 ||
                sscanf(buf, "round-trip min/avg/max/sdev = %*f/%f/%*f/%*f", &rta) == 1 ||
                sscanf(buf, "round-trip min/avg/max/stddev = %*f/%f/%*f/%*f", &rta) == 1 ||
                sscanf(buf, "round-trip min/avg/max/std-dev = %*f/%f/%*f/%*f", &rta) == 1 ||
                sscanf(buf, "round-trip (ms) min/avg/max = %*f/%f/%*f", &rta) == 1 ||
                sscanf(buf, "round-trip (ms) min/avg/max/stddev = %*f/%f/%*f/%*f", &rta) == 1 ||
                sscanf(buf, "rtt min/avg/max/mdev = %*f/%f/%*f/%*f ms", &rta) == 1) 
        {
            DEBUG_LOG("stdout: %s, rta: %.2f", buf, rta);
            continue;
        }
        else
        {
            // do nothing
        }
    }

    pclose(fp);

    if (pl == 100) //丢包率百分百
    {
        rta = DEFAULT_CTT;
    }

    if (pl >= DEFAULT_CPL || rta >= DEFAULT_CTT)
    {
        return STATE_CRITICAL; 
    }
    else if (pl >= DEFAULT_WPL || rta >= DEFAULT_WTT)
    {
        return STATE_WARNING; 
    }

    // 忽略返回结果

    return STATE_OK;
}

int error_scan(char * buf, const char * host_name)
{
    if(strcasestr(buf, "Network is unreachable") || strcasestr(buf, "Destination Net Unreachable")) 
    {   
        ERROR_LOG("CRITICAL - Network Unreachable(%s)", host_name);
        return STATE_CRITICAL;
    }
    else if(strcasestr(buf, "Destination Host Unreachable"))
    {
        ERROR_LOG("CRITICAL - Host Unreachable(%s)", host_name);
        return STATE_CRITICAL;
    }
    else if(strcasestr(buf, "Destination Port Unreachable")) 
    {
        ERROR_LOG("CRITICAL - Bogus ICMP: Port Unreachable(%s)", host_name);
        return STATE_CRITICAL;
    } 
    else if(strcasestr(buf, "Destination Protocol Unreachable"))
    {
        ERROR_LOG("CRITICAL - Bogus ICMP: Protocol Unreachable(%s)", host_name);
        return STATE_CRITICAL;
    }
    else if(strcasestr(buf, "Destination Net Prohibited"))
    {
        ERROR_LOG("CRITICAL - Network Prohibited(%s)", host_name);
        return STATE_CRITICAL;
    } 
    else if(strcasestr(buf, "Destination Host Prohibited")) 
    {
        ERROR_LOG("CRITICAL - Host Prohibited(%s)", host_name);
        return STATE_CRITICAL;
    }
    else if(strcasestr(buf, "Packet filtered"))
    {
        ERROR_LOG("CRITICAL - Packet Filtered(%s)", host_name);
        return STATE_CRITICAL;
    } 
    else if(strcasestr(buf, "unknown host"))
    {
        ERROR_LOG("CRITICAL - Host not found(%s)", host_name);
        return STATE_CRITICAL;
    }
    else if(strcasestr(buf, "Time to live exceeded")) 
    {
        ERROR_LOG("CRITICAL - Time to live exceeded(%s)", host_name);
        return STATE_CRITICAL;
    }
    else
    {
        return STATE_OK;
    }
}
