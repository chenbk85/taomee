/**
 * =====================================================================================
 *       @file  common_proto.h
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/24/2010 01:20:00 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef H_COMMON_PROTO_H_2010_08_24
#define H_COMMON_PROTO_H_2010_08_24

#include "so_proto.h"

extern "C" int handle_init(void * p);
extern "C" int handle_fini();
extern "C" int metric_handler(int index, const char * arg, c_value * v);
extern "C" const metric_info_t * get_metric_info(int index);

#endif //H_COMMON_PROTO_H_2010_08_24
