/** 
 * ========================================================================
 * @file switch_proto.h
 * @brief 
 * @author tonyliu
 * @version 1.0.0
 * @date 2012-08-20
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_SWITCH_PROTO_H_20120820   
#define H_SWITCH_PROTO_H_20120820   

#include "so_proto.h"

extern "C" int handle_init(void * p_handler);
extern "C" int handle_fini();
extern "C" int metric_handler(int index, const char *arg, value_t & val);
extern "C" const metric_info_t * get_metric_info(int index);

#endif
