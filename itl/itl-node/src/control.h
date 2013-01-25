/** 
 * ========================================================================
 * @file control.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-17
 * Modify $Date: 2012-10-17 16:12:00 +0800 (三, 17 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_CONTROL_H_2012_10_17
#define H_CONTROL_H_2012_10_17


#include "server.h"


extern c_server * g_control;


int init_connect_to_control();

int on_connected_to_control();

int dispatch_control(int fd, const char * buf, uint32_t len);

#endif
