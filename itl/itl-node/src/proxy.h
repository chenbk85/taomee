/** 
 * ========================================================================
 * @file proxy.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-17
 * Modify $Date: 2012-10-18 14:05:10 +0800 (四, 18 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "server.h"

extern c_server * g_node;

int init_proxy_process();


int dispatch_node(int fd, const char * buf, uint32_t len);
