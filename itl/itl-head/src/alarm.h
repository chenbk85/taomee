/** 
 * ========================================================================
 * @file alarm.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-12
 * Modify $Date: 2012-10-17 11:44:16 +0800 (三, 17 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_ALARM_H_2012_07_12
#define H_ALARM_H_2012_07_12


#include "itl_common.h"
#include "node.h"
#include "server.h"

extern c_server * g_alarm;

int init_connect_to_alarm();


int send_to_alarm(c_node * p_node, uint16_t cmd, Cmessage * p_out);

int alarm_node_up(c_node * p_node);

int alarm_node_down(c_node * p_node);

int send_metric_to_alarm(c_metric_arg * p_arg);


int dispatch_alarm(int fd, const char * buf, uint32_t len);

#endif
