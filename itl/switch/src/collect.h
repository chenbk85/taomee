/** 
 * ========================================================================
 * @file collect.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-06
 * Modify $Date: 2012-08-14 11:23:39 +0800 (Tue, 14 Aug 2012) $
 * Modify $Author: (local) $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_COLLECT_H_2012_07_06
#define H_COLLECT_H_2012_07_06

#include "itl_common.h"
#include "proto.h"


int init_switch_collect_so(Cmessage * c_out);

int init_metric_group_collect_timer();

int fini_metric_timer();

int fini_collect();

#endif
