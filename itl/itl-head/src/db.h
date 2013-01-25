/** 
 * ========================================================================
 * @file db.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-12
 * Modify $Date: 2012-10-18 10:57:23 +0800 (四, 18 10月 2012) $
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



int send_to_db(c_node * p_node, uint16_t cmd, Cmessage * p_out);


int send_metric_to_db(c_metric_arg * p_arg);

int db_store_host_status(c_node * p_node, uint32_t status);

#endif
