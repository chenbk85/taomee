/**
 * =====================================================================================
 *       @file  dll.cpp
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/11/2010 04:36:32 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  tonyliu (LCT), tonyliu@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <dlfcn.h>
#include <string.h>
#include "dll.h"

dll_func_t dll;

int register_plugin(const char *file_name)
{
    char *error;
    int ret_code = -1;
    bzero(&dll, sizeof(dll_func_t));

#define DLFUNC_NO_ERROR(h, v, name) do { \
    v = (typeof(v))dlsym(h, name); \
    dlerror(); \
}while (0)

#define DLFUNC(h, v, name) do { \
    v = (typeof(v))dlsym(h, name); \
    if ((error = dlerror()) != NULL)\
    { \
        NB_BOOT_LOG(-1, "dlsym error, %s", error); \
        dlclose(h); \
        h = NULL; \
        goto out; \
    } \
}while (0)

    dll.handle = dlopen(file_name, RTLD_NOW);

    if ((error = dlerror()) != NULL)
    {
        NB_BOOT_LOG(-1, "%s", error);
        goto out;
    }



    DLFUNC_NO_ERROR(dll.handle, dll.handle_init, "handle_init");
    DLFUNC_NO_ERROR(dll.handle, dll.handle_fini, "handle_fini");
    DLFUNC_NO_ERROR(dll.handle, dll.handle_open, "handle_open");
    DLFUNC_NO_ERROR(dll.handle, dll.handle_close, "handle_close");
    DLFUNC_NO_ERROR(dll.handle, dll.handle_timer, "handle_timer");
    DLFUNC_NO_ERROR(dll.handle, dll.handle_dispatch, "handle_dispatch");

    // baron add for send user data any time
    DLFUNC_NO_ERROR(dll.handle, dll.handle_schedule, "handle_schedule");

    DLFUNC(dll.handle, dll.handle_input, "handle_input");
    DLFUNC(dll.handle, dll.handle_process, "handle_process");
    ret_code = 0;
out:
    return ret_code;
}

void unregister_plugin()
{
    if (dll.handle != NULL)
    {
        dlclose(dll.handle);
        dll.handle = NULL;
    }
}

