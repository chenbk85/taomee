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

int check_host_alive(const char * host, unsigned int timeout);

#endif
