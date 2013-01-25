/** 
 * ========================================================================
 * @file alarm.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-30
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "alarm.h"
#include "proto.h"


c_server g_alarm_server;
c_server * g_alarm = &g_alarm_server;


int init_connect_to_alarm()
{
    g_alarm->connect(config_get_strval("alarm_ip", NULL), config_get_intval("alarm_port", 0));
    return 0;
}



int alarm_change_node_alarm_config(uint32_t node_id)
{
    alarm_proto_t head;
    init_alarm_proto_header(&head, sizeof(head), 0, alarm_p_change_node_alarm_config_cmd, 0, node_id);

    g_alarm->send(&head, head.len);
    INFO_LOG("notify alarm config update to node[%u]", node_id);
    return 0;
}
