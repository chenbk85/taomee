/**
 * =====================================================================================
 *       @file  function.h
 *       @brief 
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/21/2011 01:43:16 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef H_FUNCTION_H_2011_07_19
#define H_FUNCTION_H_2011_07_19

#include <stdint.h>
#include <stdio.h>

#include "data_structure.h"

#include "user_manager.h"

/**
 * @brief 打包给as回的数据的包头 
 *
 * @return 无
 */
void pack_as_pkg_header(uint32_t user_id, uint16_t msg_type, uint32_t result, uint32_t err_no);

/**
 * @brief 打包包头并将数据发送给as 
 *
 * @return 0:成功,-1:失败 
 */
void send_header_to_user(usr_info_t *p_user, uint32_t result, uint32_t err_no, bool clear_waitcmd = true);  

/**
 * @brief 将数据发送给as 
 *
 * @return 0:成功,-1:失败 
 */
void send_data_to_user(usr_info_t *p_user, const char *p_buf, uint32_t buf_len, bool clear_waitcmd = true);  

/**
 * @brief 设置发给服务端数据的包头
 *
 * @return 无
 */
void pack_svr_pkg_head(svr_msg_header_t *p_head, uint16_t msg_type, uint32_t user_id, uint32_t seq_id, uint32_t body_len);

/**
 * @brief 给服务端发送数据超时的回调函数
 */
int timeout_cb(void *p_onwer, void *p_data);

/**
 * @brief 计算用户当前的愉悦度 
 *
 * @param now_time 
 * @param last_time 
 * @param last_happy 
 *
 * @return 0:success -1:failed
 */
uint32_t get_new_happy(uint32_t now_time, uint32_t last_time, uint32_t last_happy);

/**
 * @brief 获得其他用户的健康值
 *
 * @return 无
 */
int get_other_monster_health(uint32_t now, uint32_t last_logout_time, uint32_t last_login_time, int last_health);

#endif //H_FUNCTION_H_2011_07_19
