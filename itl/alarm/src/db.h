/** 
 * ========================================================================
 * @file db.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-12
 * Modify $Date: 2012-10-17 17:20:00 +0800 (三, 17 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_DB_H_2012_07_12
#define H_DB_H_2012_07_12

#include "itl_common.h"
#include "node.h"
#include "server.h"


extern c_server * g_db;


int init_connect_to_db();

int send_to_db(const c_node * p_node, uint16_t cmd, Cmessage * p_out);



int dispatch_db(int fd, const char * buf, uint32_t len);

int db_get_node_alarm_config(c_node * p_node);

int db_store_alarm_message_data(const c_metric_alarm * p_alarm, char * msg);

int db_store_alarm_message_data(const c_node * p_node, char * msg);

int db_store_alarm_event_data(const c_metric_alarm * p_alarm, const char * msg);

int db_store_alarm_event_data(c_node * p_node, const char * msg);


int db_store_host_status(c_node * p_node, uint32_t status);

#endif

