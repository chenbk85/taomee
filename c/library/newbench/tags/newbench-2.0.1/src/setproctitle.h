/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
* =====================================================================================
*       @file  setproctitle.h
*      @brief
*
*  Detailed description starts here.
*
*   @internal
*     Created  03/17/2010 02:43:28 PM
*    Revision  1.0.0.0
*    Compiler  gcc/g++
*     Company  TaoMee.Inc, ShangHai.
*   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
*
*     @author  jasonwang (王国栋), jasonwang@taomee.com
* This source code was wrote for TaoMee,Inc. ShangHai CN.
* =====================================================================================
*/

#ifndef _NB_SETPROCTITLE_H_
#define _NB_SETPROCTITLE_H_

void initproctitle (int argc, char **argv);
void setproctitle (const char *fmt, ...);

#endif // ! SETPROCTITLE_H
