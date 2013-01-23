/**
 * =====================================================================================
 *       @file  dll.h
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/11/2010 04:28:53 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  tonyliu (LCT), tonyliu@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef _NB_DLL_H_
#define _NB_DLL_H_

#include <sys/types.h>
#include "newbench.h"

int register_plugin(const char *file_name);
void unregister_plugin ();

#endif
