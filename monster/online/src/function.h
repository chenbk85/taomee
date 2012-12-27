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

#include "data_structure.h"
#include "data.h"
#include "utility.h"
#include "badge.h"

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
 * @brief 向switch注册online
 *
 * @return 无
 */
int register_online();

/**
 * @brief 更新商店里面的物品列表
 *
 * @param shop_index: 商店的索引
 * @param level_index: 给对应级别的人生成的商店
 */
int update_shop(int shop_index, int level_index);

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
 * @brief 登录时，获得怪兽的健康值
 *
 * @return 无
 */
int get_monster_health_on_login(uint32_t now, uint32_t last_logout_time, int health);

/**
 * @brief 计算当前的健康值
 *
 * @return 无
 */
int get_now_health(int last_health, uint32_t last_update_health_time, uint32_t now);

/**
 * @brief 获得其他用户的健康值
 *
 * @return 无
 */
int get_other_monster_health(uint32_t now, uint32_t last_logout_time, uint32_t last_login_time, int last_health);

/**
 * @brief 获得植物增加的成长值
 *
 * @return 无
 */
uint32_t get_plant_growth_add(uint32_t now_happy, uint32_t last_growth_time);


/**
 * @brief 把用户经验转化成等级
 *
 * @param exp 用户的经验
 *
 * @return 用户的经验
 */
uint16_t get_level_from_exp(int exp);

/**
 * @brief 获得到这一级升级还需要的经验值
 *
 * @param exp 用户当前的经验
 * @param exp 用户当前的等级
 *
 * @return 剩下的经验值
 */
uint16_t get_level_left_exp(int exp, int level);

/**
 * @brief  解析房间buf中的所有物品id
 *
 * @param buf 保存房间所有物品及位置的数据
 * @param buf_len buf的大小
 * @param p_room_value 保存解析数据的结构体
 *
 * @return 0:success -1:failed
 */
int parse_room_buf(const char *buf, int buf_len, room_stuff_t *p_room_value);

/**
 * @brief 比较更新前和更新后房间物品的变化
 *
 * @param p_user 对每个用户缓存分配的结构体的指针
 * @param p_old_room 更新之前的房间的物品
 * @param p_new_room 更新之后的房间的物品
 * @param p_bag 用户背包里的所有物品
 * @param p_stuff_change 保存解析出的物品变化的结构体
 *
 * @return 0:success -1:failed
 */
int cmp_room_buf(usr_info_t *p_user, const room_stuff_t *p_old_room, const room_stuff_t *p_new_room, stuff_change_t *p_stuff_change);

/**
 * @brief 返回种植园的植物和吸引到的精灵
 *
 * @return 0:success -1:failed
 */
int plantation_get_pet(usr_info_t *p_user, all_plant_t *p_all_plant, uint32_t pet_id);

/**
 * @brief 返回空种植园的信息
 *
 * @return 0:success -1:failed
 */
int plantation_no_plant(usr_info_t *p_user);

/**
 * @brief 返回种植园的植物是否全部成熟
 *
 * @return true:是 false:没有全部成熟
 */
bool plant_all_maturity(all_plant_t *p_all_plant);

/**
 * @brief 进行精灵的匹配
 */
void get_attract_pet(all_plant_t *p_all_plant, uint32_t *p_pet_id, int *p_seed_match);

/**
 * @brief 判断植物有没有额外的成长值，并进行更新
 */
void get_extra_growth(usr_info_t *p_user, all_plant_t *p_all_plant);

/**
 * @brief 将用户的好友列表的用户设置到好友树请求的用户id
 */
void set_friend_list(friend_cache_t *p_friend_cache, db_msg_friend_list_rsp_t *p_friend_list);

/**
 * @brief 用户退出时更新健康值和登出时间,并通知switch用户的离线
 *
 * @return 无
 */
void update_on_logout(usr_info_t *p_user);

/**
 * @brief 获得活跃用户里的陌生人id
 *
 * @return 无
 */
int get_stranger_id(usr_info_t *p_user, stranger_t *p_stranger_info, switch_msg_active_user_t *p_active_usr);

/**
 * @brief 更新as缓存的用户的user info
 *
 * @return 无
 */
void notify_as_current_user_info(usr_info_t *p_user);

/**
 * @brief 给所有用户发的广播信息
 *
 * @return 无
 */
void broadcast_to_user(usr_info_t *p_user);

/**
 * @brief 定时发送广播的回调函数
 *
 * @return 无
 */
int broadcast_timeout_cb(void *p_owner, void *p_data);

/**
 * @brief 定时发送广播的回调函数
 *
 * @return 无
 */
time_t get_timestamp(uint32_t birthday);

/**
 * @brief 获得小游戏的道具奖励
 *
 * @return 无
 */
int get_game_reward_item(game_t *p_game);


/**
 * @brief 获得放沉迷之后的收益
 *
 * @return 无
 */
uint32_t get_current_gains(uint32_t now_gains, usr_info_t *p_user);

inline uint32_t get_new_seq(uint32_t old_seq, int pkg_len, uint16_t msg_type)
{
    return old_seq - old_seq / 7 + 147 + pkg_len % 21 + msg_type % 13;
}

int get_game_star(game_t *p_game, uint32_t one_star_score, uint32_t two_star_score, uint32_t three_star_score, uint32_t finish_score);

int send_game_lvl_to_db(usr_info_t *p_user, game_t *p_game);
int do_game_restrict_and_star(usr_info_t *p_user, game_t *p_game, uint32_t restrict_coins, uint32_t restrict_item, uint32_t day_time);


void send_pet_info(usr_info_t *p_user);
void send_cache_pet_info(usr_info_t *p_user, all_pet_t *p_all_pet);
void send_npc_score_to_as(usr_info_t *p_user, npc_score_info_t npc_score);

int get_stuff_npc_num(room_value_t *p_room, npc_score_info_t *p_npc);

int register_to_switch(void *owner, void *p_data);
int register_to_multi(void *owner, void *p_data);
int keepalive_check(void *owner, void *p_data);
int exit_online();
int compute_reward_commit_to_db(usr_info_t *p_user, as_msg_puzzle_req_t *p_req, uint32_t day_restrict_exp);
int encourage_guide_to_db(usr_info_t * p_user,uint8_t type,uint32_t score);

int update_wealth_to_db(usr_info_t *p_user, uint32_t money, uint32_t stuff_id, uint16_t stuff_num, const char *buf, int buf_len);
int update_level_relative_info(usr_info_t * p_user,uint32_t activity_id);


inline uint32_t check_body_sum(uint8_t body[], int body_len)
{
    uint32_t sum = 0;
    const uint32_t max_sum = 100000;
    int i = 0;
    for(i = 0; i < body_len ; i++)
    {
        sum += body[i];
        sum = sum % max_sum;
    }
    return sum;
}

/**
 * @brief is_depversion_id 是否为版署的米米号
 *
 * @param user_id
 *
 * @return 1:是  0：不是
 */
inline int is_depversion_id(uint32_t user_id)
{
    if(g_depversion_id_set.find(user_id) != g_depversion_id_set.end())
    {
        return 1;
    }

    return 0;
}

inline int check_for_depversion(usr_info_t *p_user, uint32_t peer_id)
{
    if(is_depversion_id(p_user->uid))
    {
        if(!is_depversion_id(peer_id))
        {//版署的号请求非版署用户的信息
            KWARN_LOG(p_user->uid, "dep version user request for non-dep user");
            pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_CANNOT_REQUEST_NONDEP);
            g_send_msg.end();
            send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
            return 1;
        }
    }
    else if(is_depversion_id(peer_id))
    {//普通用户请求版署号的信息
            KWARN_LOG(p_user->uid, "user request for dep user");
            pack_as_pkg_header(p_user->uid, p_user->waitcmd, 0, ERR_CANNOT_REQUEST_DEP);
            g_send_msg.end();
            send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());
            return 1;
    }

    return 0;
}

#endif //H_FUNCTION_H_2011_07_19
