/** 
 * ========================================================================
 * @file dispatch.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-10
 * Modify $Date: 2012-10-16 11:50:01 +0800 (二, 16 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_DISPATCH_H_2012_07_10
#define H_DISPATCH_H_2012_07_10



#include "itl_common.h"


class c_node;

int dispatch(int fd, const char * buf, uint32_t len);

int dispatch_node(c_node * p_node, const char * buf, uint32_t len);

#endif
