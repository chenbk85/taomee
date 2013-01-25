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

#include "../defines.h"
#include "oa_popen.h"
#include "check_hostalive.h"
#include "log.h"
#include "utils.h"

/**
 * @brief   使用ping命令探测主机
 * @param   host       host
 * @param   timeout    超时值
 * @return  STATE_OK | STATE_WARNING | STATE_CRITICAL
 */
int check_hostalive(const char *host, unsigned int timeout)
{
    if(host == NULL)
        return -1;

    char  buf[MAX_INPUT_BUFFER] = {0};
    char  cmd[1024] = {0};
    float rta = -1.0; //round-trip average
    int   pl = 101;

    snprintf(cmd, sizeof(cmd) - 1, PING_COMMAND, timeout == 0 ? DEFAULT_TIME_OUT : timeout, host);
    DEBUG_LOG("ping cmd=[%s]", cmd);

    FILE  *child_stdout = NULL, *child_stderr = NULL;
    if((child_stdout = oa_popen(cmd)) == NULL)
    {
        ERROR_LOG("Popen failed.");
        return -1;          
    }
    child_stderr = fdopen(child_stderr_array[fileno(child_stdout)], "r");

    while(fgets(buf, MAX_INPUT_BUFFER - 1, child_stdout))
    {
        if(error_scan(buf, host) == STATE_CRITICAL) 
        {
            DEBUG_LOG("Cache error[%s]", buf);
            oa_pclose(child_stdout);
            fclose(child_stderr);
            return STATE_CRITICAL;
        }

        //获取丢包率
        if(sscanf(buf, "%*d packets transmitted, %*d packets received, +%*d errors, %d%% packet loss", &pl) > 0 ||
                sscanf(buf, "%*d packets transmitted, %*d packets received, +%*d duplicates, %d%% packet loss", &pl) > 0 ||          
                sscanf(buf, "%*d packets transmitted, %*d received, +%*d duplicates, %d%% packet loss", &pl) > 0 ||
                sscanf(buf, "%*d packets transmitted, %*d packets received, %d%% packet loss", &pl) > 0 ||
                sscanf(buf, "%*d packets transmitted, %*d packets received, %d%% loss, time", &pl) > 0 ||
                sscanf(buf, "%*d packets transmitted, %*d received, %d%% loss, time", &pl) > 0 ||
                sscanf(buf, "%*d packets transmitted, %*d received, %d%% packet loss, time", &pl) > 0 ||
                sscanf(buf, "%*d packets transmitted, %*d received, +%*d errors, %d%% packet loss", &pl) > 0 ||
                sscanf(buf, "%*d packets transmitted %*d received, +%*d errors, %d%% packet loss", &pl) > 0) 
        { 
            DEBUG_LOG("buf:%s pl:%d/%s", buf, pl, "\%");
            continue;
        } 
        else if(sscanf(buf, "round-trip min/avg/max = %*f/%f/%*f", &rta) == 1 ||
                sscanf(buf, "round-trip min/avg/max/mdev = %*f/%f/%*f/%*f", &rta) == 1 ||
                sscanf(buf, "round-trip min/avg/max/sdev = %*f/%f/%*f/%*f", &rta) == 1 ||
                sscanf(buf, "round-trip min/avg/max/stddev = %*f/%f/%*f/%*f", &rta) == 1 ||
                sscanf(buf, "round-trip min/avg/max/std-dev = %*f/%f/%*f/%*f", &rta) == 1 ||
                sscanf(buf, "round-trip (ms) min/avg/max = %*f/%f/%*f", &rta) == 1 ||
                sscanf(buf, "round-trip (ms) min/avg/max/stddev = %*f/%f/%*f/%*f", &rta) == 1 ||
                sscanf(buf, "rtt min/avg/max/mdev = %*f/%f/%*f/%*f ms", &rta) == 1) 
        {
            DEBUG_LOG("buf:%s rta:%f", buf, rta);
            continue;
        }
        else
        {
            // do nothing
        }
    }

    if(pl == 100) //丢包率百分百
    {
        rta = DEFAULT_CTT;
    }

    if(child_stderr != NULL) 
    {
        while(fgets(buf, MAX_INPUT_BUFFER - 1, child_stderr))
        {
            if(!strcasestr(buf, "WARNING - no SO_TIMESTAMP support, falling back to SIOCGSTAMP"))
            { 
                if(error_scan(buf, host) == STATE_CRITICAL)
                {
                    oa_pclose(child_stdout);
                    fclose(child_stderr);
                    DEBUG_LOG("Cach error[%s]", buf);
                    return STATE_CRITICAL;
                }
            }
        } 
        fclose(child_stderr);
    }

    int return_stauts = oa_pclose(child_stdout);
    if(return_stauts != 0)//超时了
    {
        ERROR_LOG("Pclose failed.");
        return STATE_CRITICAL; 
    }

    if(pl >= DEFAULT_CPL || rta >= DEFAULT_CTT)
    {
        return STATE_CRITICAL; 
    }
    else if(pl >= DEFAULT_WPL || rta >= DEFAULT_WTT)
    {
        return STATE_WARNING; 
    }
    return STATE_OK;
}

int error_scan(char *buf, const char *host_name) 
{
    if(strcasestr(buf, "Network is unreachable") || strcasestr(buf, "Destination Net Unreachable")) 
    {   
        ERROR_LOG("CRITICAL - Network Unreachable (%s)", host_name);
        return STATE_CRITICAL;
    }
    else if(strcasestr(buf, "Destination Host Unreachable"))
    {
        ERROR_LOG("CRITICAL - Host Unreachable (%s)", host_name);
        return STATE_CRITICAL;
    }
    else if(strcasestr(buf, "Destination Port Unreachable")) 
    {
        ERROR_LOG("CRITICAL - Bogus ICMP: Port Unreachable (%s)", host_name);
        return STATE_CRITICAL;
    } 
    else if(strcasestr(buf, "Destination Protocol Unreachable"))
    {
        ERROR_LOG("CRITICAL - Bogus ICMP: Protocol Unreachable (%s)", host_name);
        return STATE_CRITICAL;
    }
    else if(strcasestr(buf, "Destination Net Prohibited"))
    {
        ERROR_LOG("CRITICAL - Network Prohibited (%s)", host_name);
        return STATE_CRITICAL;
    } 
    else if(strcasestr(buf, "Destination Host Prohibited")) 
    {
        ERROR_LOG("CRITICAL - Host Prohibited (%s)", host_name);
        return STATE_CRITICAL;
    }
    else if(strcasestr(buf, "Packet filtered"))
    {
        ERROR_LOG("CRITICAL - Packet Filtered (%s)", host_name);
        return STATE_CRITICAL;
    } 
    else if(strcasestr(buf, "unknown host"))
    {
        ERROR_LOG("CRITICAL - Host not found (%s)", host_name);
        return STATE_CRITICAL;
    }
    else if(strcasestr(buf, "Time to live exceeded")) 
    {
        ERROR_LOG("CRITICAL - Time to live exceeded (%s)", host_name);
        return STATE_CRITICAL;
    }
    else
    {
        return STATE_OK;
    }
}
