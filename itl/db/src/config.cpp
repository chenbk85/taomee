/** 
 * ========================================================================
 * @file config.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-17
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include <stdlib.h>
#include "config.h"
#include "async_server.h"



const char *g_p_head_ip = config_get_strval("head_ip", NULL);
int g_head_port = config_get_intval("head_port", 0);

int g_log_switch_flag = config_get_intval("log_switch_flag", 0);
log_node_t g_log_node = {-1, config_get_strval("log_node_ip", NULL), {0}};


const char * g_control_ip = config_get_strval("control_ip", NULL);
uint16_t g_control_port = config_get_intval("control_port", 0);
