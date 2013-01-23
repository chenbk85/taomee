/**
 * =====================================================================================
 *       @file  connect.h
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/04/2010 10:56:21 AM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  tonyliu(LCT) , tonyliu@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef _NB_CONNECT_H_
#define _NB_CONNECT_H_

#include "dll.h"

///控制标志位
enum {
    DAT_BLOCK,
    BROADCAST_BLOCK,
    PAD_BLOCK,
    FIN_BLOCK
};

/**
 * shm_block_t - share memory block infomation
 * @blk_id - block id
 * @length - total block length,including data
 * @type - socket type (%SOCK_STREAM, %SOCK_DGRAM, etc)
 * @skinfo - socket info
 * @accept_fd - accept socket id
 * @data - receive or send data
 */
typedef struct shm_block {
    int32_t length;
    int32_t blk_id;
    int8_t type;
    skinfo_t skinfo;
    char data[0];
}__attribute__((packed)) shm_block_t;

int conn_proc(int argc, char **argv);

#endif

