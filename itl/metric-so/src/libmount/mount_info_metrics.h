/** 
 * ========================================================================
 * @file mount_info_metrics.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-11
 * Modify $Date: 2012-10-12 11:57:16 +0800 (五, 12 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_MOUNT_INFO_METRICS_H_2012_10_11
#define H_MOUNT_INFO_METRICS_H_2012_10_11


#include "so_proto.h"



enum 
{
#define METRIC(name, type, unit, fmt, slope) name,

#include "metric_define.h"

#undef METRIC
};

int get_mount_info(const char * args, int kind, c_value * val);
int get_mount_basic_info(const char * args, int kind, char * buf);

bool remote_mounts(const char * device, const char * type);
int get_all_mounts(c_value * val);

int all_partitions_mounted_func(c_value * v);

#endif
