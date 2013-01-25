/**
 * =====================================================================================
 *       @file  c_rrd_handler.cpp
 *      @brief  
 *
 *      封装的rrd操作函数类实现
 *
 *   @internal
 *     Created  2010-10-18 11:13:42
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  mason, mason@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <string.h>
#include <unistd.h>
#include </usr/include/rrd.h>
#include <time.h>

#include "c_rrd_handler.h"

extern int g_is_switch_rrd;


//=============================宏定义=============================
#define GEN_SERVER_RRA(argc, argv) \
    do {\
        argv[argc++] = "RRA:AVERAGE:0.5:1:10080";\
        argv[argc++] = "RRA:AVERAGE:0.5:1:25920";\
        argv[argc++] = "RRA:AVERAGE:0.5:6:9000";\
        argv[argc++] = "RRA:AVERAGE:0.5:24:2250";\
        argv[argc++] = "RRA:AVERAGE:0.5:288:797";\
        \
        argv[argc++] = "RRA:MAX:0.5:1:10080";\
        argv[argc++] = "RRA:MAX:0.5:1:25920";\
        argv[argc++] = "RRA:MAX:0.5:6:9000";\
        argv[argc++] = "RRA:MAX:0.5:24:2250";\
        argv[argc++] = "RRA:MAX:0.5:288:797";\
    } while (false)

#define GEN_SWITCH_RRA(argc, argv) \
    do {\
        argv[argc++] = "RRA:AVERAGE:0.5:1:10080";\
        argv[argc++] = "RRA:AVERAGE:0.5:1:25920";\
        argv[argc++] = "RRA:AVERAGE:0.5:6:9000";\
        argv[argc++] = "RRA:AVERAGE:0.5:24:2250";\
        argv[argc++] = "RRA:AVERAGE:0.5:288:797";\
        \
        argv[argc++] = "RRA:MAX:0.5:1:10080";\
        argv[argc++] = "RRA:MAX:0.5:1:25920";\
        argv[argc++] = "RRA:MAX:0.5:6:9000";\
        argv[argc++] = "RRA:MAX:0.5:24:2250";\
        argv[argc++] = "RRA:MAX:0.5:288:797";\
        \
        argv[argc++] = "RRA:LAST:0.5:1:10080";\
        argv[argc++] = "RRA:LAST:0.5:1:25920";\
        argv[argc++] = "RRA:LAST:0.5:6:9000";\
        argv[argc++] = "RRA:LAST:0.5:24:2250";\
        argv[argc++] = "RRA:LAST:0.5:288:797";\
    } while (false)
//================================================================


/** 
 * @brief  构造函数
 * @param   
 * @return  
 */
c_rrd_handler::c_rrd_handler()
{
}


/** 
 * @brief  析构函数
 * @param   
 * @return  
 */
c_rrd_handler::~c_rrd_handler()
{
}

/** 
 * @brief  初始化函数
 * @param   p_config config对象指针
 * @return  0 成功， -1 失败
 */
int c_rrd_handler::init(const char *p_rrd_dir)
{
    if(p_rrd_dir == NULL) {
        ERROR_LOG("Parameter cannot be NULL.");
        return -1;
    }

    m_rrd_base_dir = p_rrd_dir;

    if (0 != rrd_mkdir(m_rrd_base_dir)){
        return -1; 
    }

    m_inited = true;

    return 0;
}

/** 
 * @brief  反初始化函数
 * @return  0 成功， -1 失败
 */
int c_rrd_handler::uninit()
{
    m_inited = false;
    return 0;
}

/** 
* @brief   写数据到RRD数据库中
* @param   host    host名字
* @param   metric  metric的名字
* @param   value   数值
* @param   num     个数
* @param   step    抽取间隔
* @return  成功返回0,失败返回-1
*/
int c_rrd_handler::write_data_to_rrd(
        const char *host,
        const char *metric,
        const char *value, 
        const char *num,
        unsigned int step)
{
    if (!m_inited)
    {
        ERROR_LOG("c_rrd_handler has not been inited.");
        return -1;
    }

    if (NULL == host || NULL == metric || NULL == value) {
        ERROR_LOG("Parameter ERROR: host[%s], metric[%s], value[%s]",
                NULL == host ? "NULL" : host,
                NULL == metric ? "NULL" : metric,
                NULL == value ? "NULL" : value);
        return -1;
    }

    char rrd_file_path[PATH_MAX] = {'\0'};
    int len = 0;
    int remain_len = PATH_MAX - 1;
    char *write_pos = rrd_file_path;

    //先把根目录拷进来
    len = snprintf(write_pos, remain_len, "%s", m_rrd_base_dir);
    write_pos += len;
    remain_len -= len;
    // char * relative_name = write_pos - 1;

    //len = snprintf(write_pos, remain_len, "/clusters-data");
    //write_pos += len;
    //remain_len -= len;
    //rrd_mkdir(rrd_file_path);

    len = snprintf(write_pos, remain_len, "/%s", host);
    write_pos += len;
    remain_len -= len;
    rrd_mkdir(rrd_file_path);

    snprintf(write_pos, remain_len, "/%s.rrd", metric);

    if(0 != access(rrd_file_path, F_OK))
    {
        bool summary = (num != NULL) ? true : false;
        if(0 != RRD_create(rrd_file_path, summary, step))
        {
            return -1;
        }
    }

    // *relative_name = '.';

    // return RRD_update(relative_name, value, num);
    return RRD_update(rrd_file_path, value, num);
}

/** 
 * @brief  创建RRD数据库
 * @param   rrd_file     rrd数据库路径
 * @param   summary 标志位是否summary
 * @param   step    步长
 * @return  成功返回0，失败返回-1
 */
int c_rrd_handler::RRD_create(const char *rrd_file, bool summary, unsigned int step)
{
    /* Warning: RRD_create will overwrite a RRD if it already exists */
    int  argc = 0;
    const char *argv[128] = {NULL};
    const char *data_source_type = "GAUGE";
    int  heartbeat = 30;
    char sstep[16] = {'\0'};
    char sstart[64] = {'\0'};
    char ssum[64] = {'\0'};
    char snum[64] = {'\0'};

    /* Our heartbeat is twice the step interval. */
    heartbeat = 2 * step;


    argv[argc++] = "dummy";
    argv[argc++] = rrd_file;
    argv[argc++] = "--step";
    sprintf(sstep, "%u", step);
    argv[argc++] = sstep;
    argv[argc++] = "--start";
    sprintf(sstart, "%lu", time(NULL) - 1);
    argv[argc++] = sstart;
    sprintf(ssum, "DS:sum:%s:%d:U:U", data_source_type, heartbeat);
    argv[argc++] = ssum;

    if(summary)
    {
        sprintf(snum, "DS:num:%s:%d:U:U", data_source_type, heartbeat);
        argv[argc++] = snum;
    }

    if (g_is_switch_rrd)
    {
        GEN_SWITCH_RRA(argc, argv);
    }
    else
    {
        GEN_SERVER_RRA(argc, argv);
    }

    optind = 0;
    optopt = 0;
    opterr = 0;
    optarg = NULL;
    rrd_clear_error();

    rrd_create(argc, (char**)argv);

    if(rrd_test_error())
    {
        ERROR_LOG("RRD_create error: %s", rrd_get_error());
        return -1;
    }

    DEBUG_LOG("CREATE RRD[%s]", rrd_file);
    return 0;
}

/** 
 * @brief   更新RRD数据库
 * @param   rrd_file  rrd数据库路径
 * @param   value  数值
 * @param   num  个数
 * @return  成功返回0，失败返回-1
 */
int c_rrd_handler::RRD_update(const char *rrd_file, const char *value, const char *num)
{
    int argc = 3;
    const char *argv[4] = {NULL};
    char val[128] = {'\0'};

    if(NULL == rrd_file || NULL == value)
    {
        ERROR_LOG("RRD path OR Value is NULL.");
        return -1;
    }

    // if(access(rrd_file, F_OK | R_OK | W_OK) != 0) {
        // ERROR_LOG("Can not access rrd_file[%s]: [%s]", rrd_file, strerror(errno));
        // return -1;
    // }

    if(num)
    {
        snprintf(val, sizeof(val) - 1, "N:%s:%s", value, num);
    }
    else
    {
        snprintf(val, sizeof(val) - 1, "N:%s", value);
    }

    argv[0] = "dummy";
    argv[1] = rrd_file;
    argv[2] = val; 

    optind = 0;
    optopt = 0;
    opterr = 0;
    optarg = NULL;
    rrd_clear_error();

    rrd_update(argc, (char**)argv);
    if(rrd_test_error())
    {
        ERROR_LOG("ERROR: RRD_update(%s): %s", rrd_file, rrd_get_error());
        return -1;
    } 

    return 0;
}

