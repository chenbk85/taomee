/** 
 * ========================================================================
 * @file proxy.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-18
 * Modify $Date: 2012-11-02 10:09:50 +0800 (五, 02 11月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_PROXY_H_2012_10_18
#define H_PROXY_H_2012_10_18

#include "node.h"

int dispatch_from_node(c_node * p_node, const char * buf, uint32_t len);
int dispatch_to_node(int fd, const c_node * p_node, const char * buf, uint32_t len);

int clear_node_proxy(c_node * p_node);


#endif

