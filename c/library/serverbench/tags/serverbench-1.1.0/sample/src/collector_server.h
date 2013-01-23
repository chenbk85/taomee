/**
 * =====================================================================================
 *       @file  collector_server.h
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/10/2009 02:52:21 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
 *
 *     @author  aceway (半介书生), aceway@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <benchapi.h>

#ifndef _COLLECTOR_SERVER_H_
#define _COLLECTOR_SERVER_H_

//#define __DEBUG__

const char RED_TIP[] = "\e[1m\e[31m";
const char GRN_TIP[] = "\e[1m\e[32m";
const char END_CLR[] = "\e[m";

const uint16_t MAX_CONFIG_STR_LEN       = 256;
const uint32_t MAX_SNDBUF_LEN           = 1024*8;
const uint32_t MAX_STR_LEN              = 1024;

#pragma pack(push)
#pragma pack(1)

struct s_pkg_header
{
    uint16_t    pkg_len;
    uint8_t     cmd_id;
};

enum e_protocol_cmd
{
    e_login_ip  = 0x10, //
};

struct s_login_info
{
   uint32_t     mimi_number; 
   uint8_t      login_item; 
   uint32_t     login_ip; 
   uint32_t     login_time; 
};

#pragma pack(pop)

int32_t process_init();
int32_t process_finish(int param);
int32_t process_input(const char *buffer, const int32_t length, const skinfo_t *sk);

int32_t process_info(char *recvbuf, const int32_t rcvlen, char **sendbuf, int32_t *sndlen, const skinfo_t *sk);
int32_t process_login_info(const char *p_info_body, const uint16_t info_len);
#endif
