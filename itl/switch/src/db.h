/** 
 * ========================================================================
 * @file db.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-12
 * Modify $Date: 2012-08-02 18:30:56 +0800 (Thu, 02 Aug 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_DB_H_2012_07_12
#define H_DB_H_2012_07_12

#include "itl_common.h"


void init_connect_to_db();

bool is_db_fd(int fd);

bool is_connected_to_db();

void close_db_fd();

int send_to_db(const void * buf, uint32_t len);

int send_to_db(uint16_t cmd, Cmessage * p_out);


int dispatch_db(int fd, const char * buf, uint32_t len);

int db_get_work_switch_config();

void init_time_event();

#endif
