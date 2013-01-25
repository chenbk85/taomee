/** 
 * ========================================================================
 * @file switch_proto.cpp
 * @brief 
 * @author tonyliu
 * @version 1.0.0
 * @date 2012-08-20
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */
#include <string.h>
#include <stdlib.h>

extern "C"
{
#include <libtaomee/log.h>
#include <libtaomee/utils.h>
}

#include "metrics.h"
#include "switch_proto.h"


metric_info_t g_switch_metrics_info[] =
{
    {"switch_cpu_used", OA_VALUE_INT, "%", "%d", SLOPE_BOTH, ""},
    {"switch_mem_free", OA_VALUE_INT, "byte", "%d", SLOPE_BOTH, ""},
    {"switch_interface_info", OA_VALUE_STRING, "", "%s", SLOPE_BOTH, "switch interface number, name and status"},
    {"switch_interface_in", OA_VALUE_UNSIGNED_INT, "byte/s", "%d", SLOPE_POSITIVE, ""},
    {"switch_interface_out", OA_VALUE_UNSIGNED_INT, "byte/s", "%d", SLOPE_POSITIVE, ""},
};



int handle_init(void * p_handler)
{
    if (metric_init() != 0) 
    {
        ERROR_LOG("ERROR: metric_init() in switch_proto");
        return -1;
    }

    DEBUG_LOG("switch proto inited");

    return 0;
}

int metric_handler(int index, const char *arg, value_t & val)
{
    val.value_ull = 0;
    val.value_str[0] = 0;

    ///index为指标在metric_info里的位置
    switch (index) 
    {
        case 0:
            return switch_cpu_used_func(arg, val);
        case 1:
            return switch_mem_free_func(arg, val);
        case 2:
            return switch_interface_info_func(arg, val);
        case 3:
            return switch_interface_in_func(arg, val);
        case 4:
            return switch_interface_out_func(arg, val);
        default:
            ERROR_LOG("index[%d] cannot be handled in switch_proto", index);
            return -1;
    }

    return 0;
}

int handle_fini()
{
    return 0;
}

const metric_info_t * get_metric_info(int index)
{
    if (index < 0 || index >= (int)array_elem_num(g_switch_metrics_info))
    {
        return NULL;
    }

    return (g_switch_metrics_info + index);
}

