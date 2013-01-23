/**
 * =====================================================================================
 *       @file  newbench.h
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/14/2010 03:10:46 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  tonyliu (TCL), tonyliu@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef _NEWBENCH_H_
#define _NEWBENCH_H_

#include <stdlib.h>

#include "log.h"
#include "i_net_io_server.h"

#define MAX_CONF_STR_LEN 512//配置文件中字符串最大值
#define MAX_PACKAGE_LEN (1024*4)

#define NB_BOOT_LOG(OK, fmt, args...) do{\
    boot_log(OK, 0, fmt, ##args);\
}while(0)

enum {
    PROC_MAIN = 0,
    PROC_CONN,
    PROC_WORK,
};

/**
 * skinfo_t - socket infomation
 * @fd - socket description
 * @type - sock type (%SOCK_STREAM, %SOCK_DGRAM, etc)
 * @local - connection local address
 * @remote - connection peer address
 */
typedef struct skinfo_struct {
    int sockfd;
    int type;
    long long recvtm;
    long long sendtm;

    u_int local_ip;
    u_short local_port;
    u_int remote_ip;
    u_short remote_port;

    union net_io_storage storage;
    int connection_id;
    void *ptr_lookout;
}skinfo_t;

extern "C"{
char* config_get_strval(const char* key);
int config_get_intval(const char* key, int def);
};

extern int send_data(int connection_id, const char *p_data, int data_len, int is_atomic, int is_broadcast);

#endif
