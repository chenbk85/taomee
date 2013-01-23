/**
 * =====================================================================================
 *       @file  mc_config.h
 *      @brief  读取、检验配置文件
 *
 *   @internal
 *     Created  2008年12月19日 12时00分55秒 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2008, TaoMee.Inc, ShangHai.
 *
 *     @author  aceway (半介书生), aceway@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <stdint.h>
#include "collector_server.h"
extern "C" 
{
#include <config.h>
}

#ifndef _COLLECTOR_CONFIG_H_
#define _COLLECTOR_CONFIG_H_

#pragma pack(push)
#pragma pack(1)

typedef struct s_config_variable
{
//log info config 
    char log_dir[MAX_CONFIG_STR_LEN];
    char log_prefix[MAX_CONFIG_STR_LEN];
    uint16_t log_level;
    uint16_t log_count;
    uint32_t log_size;

//magic card database config 
    char db_name[MAX_CONFIG_STR_LEN];
    char db_host[MAX_CONFIG_STR_LEN];
    char db_user[MAX_CONFIG_STR_LEN];
    char db_pass[MAX_CONFIG_STR_LEN];
    char table_prefix[MAX_CONFIG_STR_LEN];
    uint16_t    db_port;
    uint16_t    table_count;
}s_config_variable;

#pragma pack(pop)

int32_t config_variable_init();
#endif// __CONFIG_CHECKER_H__
