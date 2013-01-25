/** 
 * ========================================================================
 * @file rrd.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-12
 * Modify $Date: 2012-10-17 11:44:16 +0800 (三, 17 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_RRD_H_2012_07_12
#define H_RRD_H_2012_07_12


#include "itl_common.h"
#include "node.h"
#include "server.h"


extern c_server * g_rrd;


int init_connect_to_rrd();

int send_metric_to_rrd(c_metric_arg * p_arg);


#endif
