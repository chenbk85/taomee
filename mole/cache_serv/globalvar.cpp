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


#include "gamescore.h"
#include "./pkg/Cclientproto.h"
#include "user_time.h"



/*point to map file*/
void *gm_score_map; 
/*game score file descriptor*/
int gm_score_fd; 
/*client ip*/
uint32_t client_ip;
/*store game id and flag*/
gmid_ptr_t gmid_array;
/*point to game score address*/
gs_mem_ptr_t gmsc_adr;
/*about DB*/
Cclientproto *cpo;

/*time file descriptor*/
int time_file_fd;
/*time file map address*/
void *time_map_file;
/*time memory address*/
nick_jy_lamu *time_mem;

/*max user id*/
uint32_t max_user;
/*max game id*/
uint32_t max_game;
/*the timer interval time */
int config_time_step;
