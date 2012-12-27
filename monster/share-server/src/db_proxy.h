/**
 * =====================================================================================
 *       @file  db_proxy.h
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/30/2011 02:25:22 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef H_DB_PROXY_H_20110730
#define H_DB_PROXY_H_20110730

#include "data_structure.h"

#include "user_manager.h"

/**
 * @brief 初始化不同db协议对应的处理函数
 *
 * @return 无
 */
void init_db_handle_funs();

/**
 * @brief 处理db-proxy过来的消息
 *
 * @param:data db-proxy发过来的数据
 * @param:len db-proxy发过来的数据的长度
 *
 * @return 无
 */
void handle_db_return(svr_msg_header_t *p_db_pkg, int len);

/**
 * @brief 处理db-proxy过来的登录协议返回包
 *
 * @return 0:成功,-1:失败 
 */
int process_db_add_role_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);


/**
 * @brief 处理db-proxy过来的登录协议返回包
 *
 * @return 0:成功,-1:失败 
 */
int process_db_role_info_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief 处理db-proxy过来的修改登录数据的返回包
 *
 * @return 0:成功,-1:失败 
 */
int process_db_update_login_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief 处理数据库获得背包信息的返回
 *
 * @return 0:成功,-1:失败 
 */
int process_db_get_bag_stuff_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief  处理数据库获得用户房间buf信息的返回
 *
 * @return 0:success -1:failed
 */
int process_db_get_room_info_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief  处理更新用户房间buf信息的返回
 *
 * @return 0:success -1:failed
 */
int process_db_update_room_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief  处理获得用户房间id的返回
 *
 * @return 0:success -1:failed
 */
int process_db_get_room_num_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief  增加背包物品的返回
 *
 * @return 0:success -1:failed
 */
int process_db_add_stuff_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief  获取益智游戏信息的返回
 *
 * @return 0:success -1:failed
 */
int process_db_get_puzzle_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief  提交益智游戏信息的返回
 *
 * @return 0:success -1:failed
 */
int process_db_commit_puzzle_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief  更新植物的成长值和成长时间
 *
 * @return 0:success -1:failed
 */
int process_db_update_plant_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief  获得其他用户的role信息
 *
 * @return 0:success -1:failed
 */
int process_db_other_role_info_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief  获得留言板信息
 *
 * @return 0:success -1:failed
 */
int process_db_get_pinboard_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief  获得用户基本信息
 *
 * @return 0:success -1:failed
 */
int process_db_get_friend_info_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief 增加一条新留言
 *
 * @return 0:success -1:failed
 */
int process_db_add_message_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief 更新留言的状态
 *
 * @return 0:success -1:failed
 */
int process_db_update_msg_status_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief 增加用户的role信息
 *
 * @return 0:success -1:failed
 */
int process_db_add_role_value_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief 更改用户的role信息
 *
 * @return 0:success -1:failed
 */
int process_db_update_role_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief 更改用户的个性签名
 *
 * @return 0:success -1:failed
 */
int process_db_modify_sign_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief 吃食物的db返回协议处理
 *
 * @return 0:success -1:failed
 */
int process_db_eat_food_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief 获得小游戏天限制的db返回协议处理
 *
 * @return 0:success -1:failed
 */
int process_db_get_day_restrict_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief 增加小游戏天限制的db返回协议处理
 *
 * @return 0:success -1:failed
 */
int process_db_add_day_restrict_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief 删掉种植园植物的db返回协议处理
 *
 * @return 0:success -1:failed
 */
int process_db_del_plant_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief 对吸引到的精灵的操作的db返回协议处理
 *
 * @return 0:success -1:failed
 */
int process_db_pet_op_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief 种植园种一株植物的db返回协议处理
 *
 * @return 0:success -1:failed
 */
int process_db_add_plant_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief 获得用户的好友列表
 *
 * @return 0:success -1:failed
 */
int process_db_get_friend_id_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief 获得用户的好友列表
 *
 * @return 0:success -1:failed
 */
int process_db_del_friend_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief 更改好友的状态位
 *
 * @return 0:success -1:failed
 */
int process_db_set_friend_status_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);


/**
 * @brief 通过用户的名字查找用户的米米号
 *
 * @return 0:success -1:failed
 */
int process_db_search_name_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief 插入用户的好友申请信息
 *
 * @return 0:success -1:failed
 */
int process_db_apply_for_friend_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief  获得用户返回的好友申请信息
 *
 * @return 0:success -1:failed
 */
int process_db_get_friend_apply_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief  获得活跃用户id
 *
 * @return 0:success -1:failed
 */
int process_db_active_user_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief  获得用户的profile信息
 *
 * @return 0:success -1:failed
 */
int process_db_get_profile_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);

/**
 * @brief  验证用户的session返回包的处理函数
 *
 * @return 0:success -1:failed
 */
int process_db_check_session_return(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);



















#endif //H_DB_PROXY_H_20110730
