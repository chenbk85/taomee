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

/*
 *  struct app_config - application program interface
 */
typedef struct  dll_func_struct {
    void *handle;
    int (*handle_init) (int, char **, int);

    //返回值: 大于0-屏蔽，0-不屏蔽， 小于0-出错
    int (*handle_dispatch) (const char* buf, int buf_len, int proc_num, int* mimi_num);

    //返回值
    //-1:数据异常，通知上层结束掉本次连接
    //0:当前不能确认协议包长度，需要继续接收数据
    //大于零:本次通信的数据包长度(协议级的)
    int (*handle_input) (const char*, int, skinfo_t*);

    int (*handle_process) (char *, int , char **, int *, skinfo_t*);

    // baron add for send client data any time
    int (*handle_schedule) ();

    int (*handle_open) (char **, int *, skinfo_t*);
    int (*handle_close) (const skinfo_t*);
    int (*handle_timer) (int *);
    void (*handle_fini) (int);
} dll_func_t;

int register_plugin(const char *file_name);
void unregister_plugin ();

#endif

