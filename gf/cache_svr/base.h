/**
 * ====================================================================
 *  @file       base.h
 *
 *  @brief     申明base.cpp中定义的函数
 *
 *  platform   Debian 4.1.1-12 
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 * ====================================================================
 */


#ifndef  BASE_INC
#define  BASE_INC

extern "C"{

#include <stdint.h>
}

#include "./pkg/Cclientproto.h"

#include "packet.h"

int set_sndbuf_head(char *sendbuf, int *sndlen, proto_head_t *rcvph, int private_size, int ret);

int is_user_exist(uint32_t gm_id, uint32_t user_id, uint32_t score, int *opy_pos);

int trim(char *s);

#endif   /* ----- #ifndef BASE_INC  ----- */

