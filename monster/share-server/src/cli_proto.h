/**
 * =====================================================================================
 *       @file  cli_proto.h
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/30/2011 09:53:29 AM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef H_CLI_PROTO_H_20110730
#define H_CLI_PROTO_H_20110730

extern "C" 
{
#include <async_serv/dll.h>
}

#include "user_manager.h"

typedef struct
{
    uint8_t gender;
    char name[16];
} __attribute__((packed)) as_user_login_req_t;

typedef struct
{
    uint32_t user_id;
    uint8_t gender;
    char name[16];
} user_login_info_t;

/**
 * @brief 初始化不同协议对应的处理函数
 *
 * @return 无
 */
void init_cli_handle_funs();

/**
 * @brief  应用so自己实现的函数，分发客户端的协议包，调用不同的处理函数
 *
 * @param p_data    接收到数据包
 * @param len       接收到的数据包的长度
 * @param fdsess    对端连接信息
 *
 * @return 0:success -1:failed
 */
int dispatch(void *p_data, int len, fdsession_t *fdsess, bool first_tm = true);

/**
 * @brief 将用户请求缓存在队列里面 
 *
 * @param p_user    保存用户信息的结构体
 * @param buf       要缓存的数据包
 * @param buf_len   要缓存的数据包的长度
 *
 * @return 无
 */
int cache_a_pkg(usr_info_t *p_user, const char *buf, uint32_t buf_len);

/**
 * @brief 将缓存的命令全部遍历执行一遍 
 *
 * @return 无
 */
void proc_cached_pkgs();

/**
 * @brief  获取房间buf信息协议的处理函数
 *
 * @return 0:success -1:failed
 */
int process_get_room_info(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  获得用户信息
 *
 * @return 0:success -1:failed
 */
int process_get_user_info(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);


int process_new_user_login(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

int process_login_user(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

#endif //H_CLI_PROTO_H_20110730
