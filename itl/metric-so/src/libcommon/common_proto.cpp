/**
 * =====================================================================================
 *       @file  common_proto.cpp
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/24/2010 04:35:38 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <string.h>
#include <stdlib.h>

extern "C"
{
#include <libtaomee/log.h>
#include <libtaomee/utils.h>
}
#include "common_proto.h"
#include "metrics.h"


metric_info_t g_common_metrics_info[] =
{
#define METRIC(name, value_type, unit, fmt, slope) \
    {#name, value_type, unit, fmt, slope},

#include "metric_define.h"
#undef METRIC
};

enum
{
#define METRIC(name, type, unit, fmt, slope) name,

#include "metric_define.h"

#undef METRIC

};

int handle_init(void * p)
{

    if (metric_init() != 0) 
    {
        ERROR_LOG("ERROR: metric_init() in common_proto");
        return -1;
    }

    DEBUG_LOG("common proto inited");

    return 0;
}

int metric_handler(int index, const char *arg, c_value * val)
{

    val->reset();

    ///index为指标在metric_info里的位置
    switch (index) 
    {
#define METRIC(name, type, unit, fmt, slope) \
        case name: return name ## _func(val);
#include "metric_define.h"
#undef METRIC
        // case 0:
            // return bytes_out_func(val);
        // case 1:
            // return eth0_send_func(val);
        // case 2:
            // return eth1_send_func(val);
        // case 3:
            // return bytes_in_func(val);
        // case 4:
            // return eth0_recv_func(val);
        // case 5:
            // return eth1_recv_func(val);
        // case 6:
            // return pkts_in_func(val);
        // case 7:
            // return pkts_out_func(val);
        // case 8:
            // return mem_total_func(val);
        // case 9:
            // return mem_free_func(val);
        // case 10:
            // return mem_buffers_func(val);
        // case 11:
            // return mem_cached_func(val);
        // case 12:
            // return swap_free_func(val);
        // case 13:
            // return swap_total_func(val);
        // case 14:
            // return disk_total_func(val);
        // case 15:
            // return disk_space_usage(val);
        // case 16:
            // return disk_free_func(val);
        // case 17:
            // return part_max_used_func(val);
        // case 18:
            // return boottime_func(val);
        // case 19:
            // return sys_clock_func(val);
        // case 20:
            // return machine_type_func(val);
        // case 21:
            // return os_name_func(val);
        // case 22:
            // return os_release_func(val);
        // case 23:
            // return proc_run_func(val);
        // case 24:
            // return proc_total_func(val);
        // case 25:
            // return load_one_func(val);
        // case 26:
            // return load_five_func(val);
        // case 27:
            // return load_fifteen_func(val);
        // case 28:
            // return cpu_num_func(val);
        // case 29:
            // return cpu_speed_func(val);
        // case 30:
            // return cpu_user_func(val);
        // case 31:
            // return cpu_nice_func(val);
        // case 32:
            // return cpu_system_func(val);
        // case 33:
            // return cpu_idle_func(val);
        // case 34:
            // return cpu_aidle_func(val);
        // case 35:
            // return cpu_wio_func(val);
        // case 36:
            // return cpu_intr_func(val);
        // case 37:
            // return cpu_sintr_func(val);
        // case 38:
            // return inside_ip_func(val);
        // case 39:
            // return outside_ip_func(val);
        // case 40:
            // return cpu_info_func(val);
        // case 41:
            // return os_version_func(val);
        // case 42:
            // return system_time_offset_func(val);
        // case 43:
            // return eth0_speed_func(val);
        // case 44:
            // return eth1_speed_func(val);
        default:
            ERROR_LOG("index:%d cannot be handle in common_proto", index);
            return -1;

    }

}


int handle_fini()
{
    return 0;
}


const metric_info_t * get_metric_info(int index)
{
    if (index < 0 || index >= (int)array_elem_num(g_common_metrics_info))
    {
        return NULL;
    }
    return (g_common_metrics_info + index);

}

