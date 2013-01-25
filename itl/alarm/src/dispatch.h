/** 
 * ========================================================================
 * @file dispatch.h
 * @brief 
 * @version 1.0
 * @date 2012-7-11
 * Modify $Date: 
 * Modify $Author:
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_DISPATCH_H
#define H_DISPATCH_H

extern "C"
{

#include <libtaomee/list.h>
}
#include "itl_common.h"


extern list_head_t g_awaiting_node;

int dispatch(int fd, const char * buf, uint32_t len);

void proc_cached_pkg();

#endif
