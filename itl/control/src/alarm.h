/** 
 * ========================================================================
 * @file alarm.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-30
 * Modify $Date: 2012-10-31 16:38:11 +0800 (三, 31 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_ALARM_H_2012_10_30
#define H_ALARM_H_2012_10_30


#include "server.h"


extern c_server * g_alarm;

int init_connect_to_alarm();

int alarm_change_node_alarm_config(uint32_t node_id);


#endif


