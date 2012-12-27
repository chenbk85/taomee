/**
 * =====================================================================================
 *       @file  ucount.h
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/30/2011 01:38:05 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef H_UCOUNT_H_20110810
#define H_UCOUNT_H_20110810

#include "data_structure.h"
#include "user_manager.h"

/**
 * @brief 处理ucount过来的消息
 *
 * @param:data ucount发过来的数据
 * @param:len ucount发过来的数据的长度
 *
 * @return 无
 */
void handle_ucount_return(svr_msg_header_t *p_ucount_pkg, int len);

/**
 * @brief 处理ucount过来的用户当天是否顶过用户小屋的消息
 *
 * @return 无
 */
int handle_thumb_user_room_return(usr_info_t *p_user, svr_msg_header_t *p_ucount_pkg);  

/**
 * @brief 处理ucount过来的当天是否访问过用户小屋的消息
 *
 * @return 无
 */
int handle_visit_user_room_return(usr_info_t *p_user, svr_msg_header_t *p_ucount_pkg);  

/**
 * @brief 处理ucount过来的最近访问用户的消息
 *
 * @return 无
 */
int handle_latest_visit_return(usr_info_t *p_user, svr_msg_header_t *p_ucount_pkg);  

int handle_unread_count_return(usr_info_t *p_user, svr_msg_header_t *p_ucount_pkg);  


#endif //H_UCOUNT_H_20110810

