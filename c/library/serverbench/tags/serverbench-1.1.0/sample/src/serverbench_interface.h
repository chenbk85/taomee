/**
 *====================================================================
 *     @file	serverbench_interface.h
 *    @brief	衔接workbench接口
 * 
 *  @internal
 *   @created	13:3:2009   17:02:41
 *   @version   1.0.0.0
 *   compiler   gcc/g++
 *   platform 	linux
 *
 *    @author	aceway taomee  www.taomee.com
 *	  company   TaoMee,Inc. ShangHai CN (www.taomee.com)
 *
 *	copyright:	2008 TaoMee, Inc. ShangHai CN. All right reserved.
 *====================================================================
 */

#include <benchapi.h>

#ifndef _SERVERBENCHE_INTERFACE_H_
#define _SERVERBENCHE_INTERFACE_H_

extern "C" int handle_init(int, char **, int);
extern "C" int handle_input(const char*, int, const skinfo_t*);
extern "C" int handle_process(char *, int , char **, int *, const skinfo_t*);
extern "C" void handle_close(const skinfo_t*);
extern "C" void handle_fini(int);

#endif

