/** 
 * ========================================================================
 * @file metric_define.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-12
 * Modify $Date: 2012-10-12 11:57:16 +0800 (五, 12 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


// #ifndef H_METRIC_DEFINE_H_2012_10_12
// #define H_METRIC_DEFINE_H_2012_10_12


METRIC(disk_total_by_mount, ITL_VALUE_UINT64, "B", "%llu", SLOPE_CURR)
METRIC(disk_free_by_mount, ITL_VALUE_UINT64, "B", "%llu", SLOPE_CURR)
METRIC(inodes_total_by_mount, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
METRIC(inodes_free_by_mount, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
METRIC(mount_device_by_mount, ITL_VALUE_STRING, "", "%s", SLOPE_CURR)
METRIC(mount_dir_by_mount, ITL_VALUE_STRING, "", "%s", SLOPE_CURR)
METRIC(mount_type_by_mount, ITL_VALUE_STRING, "", "%s", SLOPE_CURR)
METRIC(all_mount_device_name, ITL_VALUE_STRING, "", "%s", SLOPE_CURR)
METRIC(number_of_mount, ITL_VALUE_UINT32, "", "%u", SLOPE_CURR)
METRIC(number_of_partition, ITL_VALUE_UINT32, "", "%u", SLOPE_CURR)
METRIC(all_partitions_mounted, ITL_VALUE_UINT32, "", "%u", SLOPE_CURR)


// #endif
