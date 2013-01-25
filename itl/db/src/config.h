/** 
 * ========================================================================
 * @file config.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-17
 * Modify $Date: 2012-10-17 11:24:59 +0800 (三, 17 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_CONFIG_H_2012_10_17
#define H_CONFIG_H_2012_10_17

#include <stdint.h>
#include "db_macro.h"

extern const char *g_p_head_ip;
extern int g_head_port;

extern int g_log_switch_flag;
extern log_node_t g_log_node;

extern const char * g_control_ip;
extern uint16_t g_control_port;

#endif

