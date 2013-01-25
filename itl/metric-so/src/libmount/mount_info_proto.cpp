/**
 * =====================================================================================
 *       @file  mount_info_proto.cpp
 *      @brief  get mount info
 *
 *     Created  2011-09-19 18:22:07
 *    Revision  1.0.0.0
 *    Compiler  g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  ping, ping@taomee.com
 * =====================================================================================
 */

#include <string.h>
#include <stdlib.h>

extern "C"
{
#include <libtaomee/log.h>
#include <libtaomee/utils.h>
}

#include "mount_info_proto.h"
#include "mount_info_metrics.h"


metric_info_t g_mount_metrics_info[] =
{
#define METRIC(name, value_type, unit, fmt, slope) \
    {#name, value_type, unit, fmt, slope},

#include "metric_define.h"
#undef METRIC

};


int handle_init(void * p)
{

    return 0;
}

int metric_handler(int index, const char *arg, c_value * v)
{

    switch (index)
    {
        case disk_total_by_mount:
        case disk_free_by_mount:
        case inodes_total_by_mount:
        case inodes_free_by_mount:
            if (0 != get_mount_info(arg, index, v))
            {
                return -1;
            }
            break;
        case mount_device_by_mount:
        case mount_dir_by_mount:
        case mount_type_by_mount:
            {
                static char buf[1024] = {0};
                if (0 != get_mount_basic_info(arg, index, buf))
                {
                    return -1;
                }
                *v = buf;
            }
            break;
        case all_mount_device_name:
            if (0 != get_all_mounts(v))
            {
                return -1;
            }
            break;
        case number_of_mount:
            break;
        case number_of_partition:
            break;
        case all_partitions_mounted:
            return all_partitions_mounted_func(v);

        default:
            ERROR_LOG("index : %d cannot be handle in common_proto", index);
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
    if (index < 0 || index >= (int)array_elem_num(g_mount_metrics_info))
    {
        return NULL;
    }
    return (g_mount_metrics_info + index);
}

