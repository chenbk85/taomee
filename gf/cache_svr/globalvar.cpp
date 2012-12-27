/**
 * ============================================================================
 * @file   globalvar.cpp
 *
 * @brief  定义全局变量
 *
 * compiler : gcc version 4.1.2
 *
 * platfomr : Debian 4.1.1-12
 *
 * copyright : TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 * ============================================================================
 */


#include "./pkg/Cclientproto.h"
#include "user_time.h"



/*client ip*/
uint32_t client_ip;
/*about DB*/
Cclientproto *cpo;

/*time file descriptor*/
int time_file_fd;
/*time file map address*/
void *time_map_file;
/*time memory address*/
//nick_jy_lamu *time_mem;
pp_nick *time_mem;

/*max user id*/
uint32_t max_user;
/*max game id*/
uint32_t max_game;
