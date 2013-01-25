/** 
 * ========================================================================
 * @file rrd.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-04
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include <libtaomee++/utils/md5.h>
#include "proto.h"
#include "itl_common.h"
#include "rrd.h"


#define g_rrd_handler SINGLETON(c_rrd_handler)

#define g_md5_obj SINGLETON(utils::MD5)


extern int g_is_switch_rrd;
extern const char *g_p_host_name;
extern const char *g_p_metric_name;

int rrd_p_get_version(DEFAULT_ARG)
{
    //rrd_p_get_version_in * p_in = P_IN;
    //rrd_p_get_version_out * p_out = P_OUT;

    return 0;
}

int rrd_p_recv_data(DEFAULT_ARG)
{
    rrd_p_recv_data_in * p_in = P_IN;


    lower_case(p_in->rrd_data.host);
    const char *host = p_in->rrd_data.host;
    const char *metric = p_in->rrd_data.metric;
    const char * arg = p_in->rrd_data.metric_arg;
    const char * val_str = p_in->rrd_data.value;
    uint32_t step = p_in->rrd_data.collect_interval;

    bool is_do_log = false;
    if (NULL != g_p_host_name)
    {
        if (0 == strcasecmp(host, g_p_host_name))
        {
            if (NULL == g_p_metric_name || 0 == strcasecmp(metric, g_p_metric_name))
            {
                is_do_log = true;
            }
        }
    }
    else
    {
        if (NULL != g_p_metric_name && 0 == strcasecmp(metric, g_p_metric_name))
        {
            is_do_log = true;
        }
    }

    if (is_do_log)
    {
        INFO_LOG("Host[%s] metric[%s] arg[%s] value[%s] step[%u]", host, metric, arg, val_str, step);
    }

    static char filename[MAX_METRIC_NAME_LEN + 33] = {0};

    if (arg[0])
    {
        if (g_is_switch_rrd)
        {
            snprintf(filename, sizeof(filename), "%s_%s", metric, arg);
        }
        else
        {
            g_md5_obj.reset();
            g_md5_obj.update(arg, strlen(arg));
            snprintf(filename, sizeof(filename), "%s_%s", metric, g_md5_obj.toString().c_str());
        }
    }
    else
    {
        snprintf(filename, sizeof(filename), "%s", metric);
    }

    return  g_rrd_handler.write_data_to_rrd(host, filename, val_str, NULL, step);
}


int init_rrd_handler(const char * rrd_dir)
{
    if (0 != g_rrd_handler.init(rrd_dir)) {
        ERROR_LOG("Init ERROR: g_p_rrd_handler init fialed.");
        return -1;
    }
    return 0;
}


int fini_rrd_handler()
{
    g_rrd_handler.uninit();
    return 0;
}
