/**
 * =====================================================================================
 *       @file  mysql_proto.cpp
 *      @brief  获取数据库监控数据
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/04/2011 01:30:55 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  mason@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

extern "C"
{
#include <libtaomee/log.h>
#include <libtaomee/inet/inet_utils.h>
}
#include "mysql_metric.h"
#include "mysql_proto.h"
#include "metric.h"
#include "instance.h"



int handle_init(void * param)
{
    std::vector<mysql_so_param_t> * p_vec = reinterpret_cast< typeof(p_vec) >(param);


    if(0 != init_instances(p_vec)) 
    {
        ERROR_LOG("ERROR: init instances failed!");
        return -1;
    }

    return 0;
}

int metric_handler(int index, const char * arg, c_value * val)
{
    //index 应该在这个范围内[0, METRIC_NUM - 1]
    if (index < 0 || index >= (int)METRIC_NUM || NULL == arg)
    {
        return -1;
    }

    int port = atoi(arg);
    if (!is_legal_port(port))
    {
        return -1;
    }

    int ret = get_value_by_metric_name(port, g_metrics_info[index].name, val);

    return ret;
}


int handle_fini()
{
    fini_instances();
    return 0;
}

const metric_info_t * get_metric_info(int index)
{
    if (index < 0 || index >= (int)METRIC_NUM)
    {
        return NULL;
    }
    return (g_metrics_info + index);
}
