/**
 * ====================================================================
 *  @file       globalvar.h
 *
 *  @brief      声明全局变量
 *
 *  platform   Debian 4.1.1-12 
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 * ====================================================================
 */



#ifndef  GLOBALVAR_INC
#define  GLOBALVAR_INC


extern "C"{
#include <stdint.h>
}

#include "./pkg/Cclientproto.h"

#include "gamescore.h"
#include "user_time.h"

/*接收到的应该最小长度包*/
#define MIN_HEAD_LENGTH 4
/*FLASH请求的长度*/
#define FLASH_REQ 23

#define MIN_LENGTH 17 
/*包的最大长度*/
#define MAX_LENGTH 4096
/*昵称的长度*/
#define NICK_LEN 16
/*表示成功*/
#define SUCCESS 0


/*保存map返回的地址*/
extern void *gm_score_map; 
/*游戏分数文件描述符*/
extern int gm_score_fd; 
/*保存客户端的地址*/
extern uint32_t client_ip;
/*数据库类的指针*/
extern Cclientproto *cpo;
/*保存游戏的ID和FLAG*/
extern gmid_ptr_t gmid_array;
/*游戏分数的内存地址*/
extern gs_mem_ptr_t gmsc_adr;


/*时间文件描述符*/
extern int time_file_fd;
/*时间文件的MAP地址*/
extern void *time_map_file;
/*时间文件的内存地址*/
extern nick_jy_lamu *time_mem;
/*最大用户数*/
extern uint32_t max_user;
/*最大游戏数*/
extern uint32_t max_game;
/*定时器间隔时间*/
extern int config_time_step;

#endif   /* ----- #ifndef GLOBALVAR_INC  ----- */

