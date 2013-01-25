/**
 * =====================================================================================
 *       @file  mount_info_proto.h
 *      @brief  get mount info
 *
 *     Created  2011-09-19 18:22:36
 *    Revision  1.0.0.0
 *    Compiler  g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  ping, ping@taomee.com
 * =====================================================================================
 */

#ifndef  MOUNT_INFO_PROTO_H
#define  MOUNT_INFO_PROTO_H

#include "so_proto.h"

extern "C" int handle_init(void * p);
extern "C" int handle_fini();
extern "C" int metric_handler(int index, const char *arg, c_value * v);
extern "C" const metric_info_t * get_metric_info(int index);


#endif  /*MOUNT_INFO_PROTO_H*/
