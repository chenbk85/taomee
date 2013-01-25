/**
 * =====================================================================================
 *       @file  check_hostalve.h
 *      @brief  
 *
 *  用ping命令检查一个主机是否down掉
 *
 *   @internal
 *     Created  01/12/2011 05:40:44 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  mason, mason@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef H_CHECK_HOSTALIVE
#define H_CHECK_HOSTALIVE

enum 
{
    STATE_OK = 0,
    STATE_WARNING,
    STATE_CRITICAL
};

const char* const  PING_COMMAND = "/bin/ping -n -U -w%u -c5 %s"; //发5个包
const unsigned int MAX_INPUT_BUFFER = 4096;

const float DEFAULT_WTT = 3000.0;    //缺省警戒间歇时间WARNING TRIP TIME 毫秒
const float DEFAULT_CTT = 5000.0;    //缺省严重警戒间歇时间CRITICAL TRIP TIME 毫秒
const int   DEFAULT_WPL = 80;        //缺省警戒丢包率 百分比
const int   DEFAULT_CPL = 100;       //缺省严重警戒丢包率 百分比

int check_hostalive(const char *host, unsigned int timeout);
int error_scan(char *buf, const char *host_name);
#endif
