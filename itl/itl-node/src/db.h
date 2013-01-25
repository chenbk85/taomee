/** 
 * ========================================================================
 * @file db.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-12
 * Modify $Date: 2012-11-15 18:04:16 +0800 (四, 15 11月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_DB_H_2012_07_12
#define H_DB_H_2012_07_12

#include "itl_common.h"
#include "node.h"

class c_server;

extern c_server * g_db;


int init_connect_to_db();

int dispatch_db(int fd, const char * buf, uint32_t len);

int db_get_node_server_config(const char * node_ip);

int db_get_node_mysql_config(const char * node_ip);

int db_get_node_head_config(const char * node_ip);
int db_get_node_control_config(const char * node_ip);

int db_get_node_mysql_instance(const char * node_ip);

int db_store_db_mgr_result(uint32_t manage_id, uint32_t manage_value_id, uint32_t sql_no, uint32_t status_code, const char * sql, const char * error_desc = NULL);

#endif
