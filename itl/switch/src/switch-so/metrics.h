/** 
 * ========================================================================
 * @file metrics.h
 * @brief 
 * @author tonyliu
 * @version 1.0.0
 * @date 2012-08-20
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */
#ifndef H_METRICS_H_20120820
#define H_METRICS_H_20120820

#include "so_proto.h"


#define SWITCH_SNMP_DEFAULT_TIMEOUT 3
#define SWITCH_SNMP_DEFAULT_RETRY 4
#define SWITCH_MAX_OID_LEN 512
#define IF_SPEED_BASE  (1000000)

typedef struct {
    unsigned int switch_type;    /**<交换机类型: 1-DELL，2-IBM，3-NSFOCUS，4-Cisco，5-H3C*/
    const char *ip;        /**<交换机IP*/
    const char *community; /**<交换机团提名*/
    unsigned int if_num;         /**<端口号, 没有则填0*/
} switch_base_info_t;


int metric_init();

int switch_cpu_used_func(const void *arg, value_t & val);
int switch_mem_free_func(const void *arg, value_t & val);
int switch_interface_info_func(const void *arg, value_t & val);
int switch_interface_in_func(const void *arg, value_t & val);
int switch_interface_out_func(const void *arg, value_t & val);

#endif
