/** 
 * ========================================================================
 * @file itl_head.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-06
 * Modify $Date: 2012-10-25 11:26:03 +0800 (四, 25 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_ITL_HEAD_H_2012_07_06
#define H_ITL_HEAD_H_2012_07_06

#include "server.h"


extern c_server * g_head;


int init_connect_to_head();

int on_connected_to_head();


int dispatch_head(int fd, const char * buf, uint32_t len);

#endif
