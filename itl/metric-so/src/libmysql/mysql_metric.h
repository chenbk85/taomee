/**
 * =====================================================================================
 *       @file  mysql_metric.h
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/04/2011 05:11:56 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author mason@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef H_MYSQL_METRIC_H
#define H_MYSQL_METRIC_H

#include "so_proto.h"





int get_value_by_metric_name(uint32_t port, const char * metric_name, c_value * val);

#endif
