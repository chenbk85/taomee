/**
 * =====================================================================================
 *       @file  define.h
 *      @brief  
 *   @internal
 *     Created  2012-01-13 16:13:42
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2012, TaoMee.Inc, ShangHai.
 *
 *     @author  tonyliu(LCT), tonyliu@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef H_DEFINE_20120113_H
#define H_DEFINE_20120113_H
#include <limits.h>

#define MYSQL_FLUSH_TIMES 20 /**<数据库没隔N次监控刷新一次*/
#define SERVER_CHECK_SPAN 180 /**<服务每次监控时间跨度*/
#define VERSION "1.0.0"
#define INSIDE_HEAD_PATH "/opt/taomee/oa/oa-head-"
#define INSIDE_ALARM_PATH "/opt/taomee/oaadmin/alarm-server"
#define OUTSIDE_HEAD_PATH "/opt/taomee/oaadmin/oa-head-"
#define OUTSIDE_ALARM_PATH "/opt/taomee/oaadmin/deamon/alarm-server"
#define MAX_STR_LEN 256
#define MAX_BUF_LEN 1024

//配置项的结构
typedef struct {
    unsigned int   log_count;              
    unsigned int   log_lvl;
    unsigned int   log_size;
    char           log_prefix[MAX_STR_LEN];
    char           log_dir[PATH_MAX];
} log_conf_t;

#endif
