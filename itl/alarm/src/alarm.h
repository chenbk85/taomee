/** 
 * ========================================================================
 * @file alarm.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-20
 * Modify $Date: 2012-08-30 17:46:43 +0800 (四, 30  8月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_ALARM_H_2012_07_20
#define H_ALARM_H_2012_07_20


#include "node.h"

int handle_head_down(int fd);

int handle_alarm(c_metric_alarm * p_alarm);

int send_collect_fail_alarm(c_metric_alarm * p_alarm);


int send_metric_recovery_alarm(const c_metric_alarm * p_alarm);

int send_metric_alarm(const c_metric_alarm * p_alarm);

int init_send_host_alarm(c_node * p_node);

int send_host_alarm(c_node * p_node);

int send_node_alarm(c_node * p_node);

int send_db_alarm();

int send_head_alarm();


#endif

