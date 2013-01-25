/**
 * =====================================================================================
 *       @file  mysql_proto.h
 *      @brief  获取数据库监控数据
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/04/2011 01:27:38 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  tonyliu (LCT), tonyliu@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef H_MYSQL_PROTO_H
#define H_MYSQL_PROTO_H

#include "so_proto.h"

extern "C" int handle_init(void * p);
extern "C" int handle_fini();
extern "C" int metric_handler(int index, const char *arg, c_value * v);
extern "C" const metric_info_t * get_metric_info(int index);

#endif
