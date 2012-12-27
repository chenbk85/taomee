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
int cache_a_pkg(usr_info_t *p_user, const char *buf, uint32_t buf_len, bool clear_cmd = false);

/**
 * @brief 将缓存的命令全部遍历执行一遍
 *
 * @return 无
 */
void proc_cached_pkgs();

/**
 * @brief  登录协议的处理函数
 *
 * @param p_user 用户缓存
 * @param p_msg_body 协议包体
 * @param msg_body_len 协议包体的长度
 *
 * @return 0:success -1:failed
 */
int process_login(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  注册协议的处理函数
 *
 * @return 0:success -1:failed
 */
int process_register(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  注册协议的处理函数
 *
 * @return 0:success -1:failed
 */
int process_get_bag_stuff(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  获取房间buf信息协议的处理函数
 *
 * @return 0:success -1:failed
 */
int process_get_room_info(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  更新房间buf信息协议的处理函数
 *
 * @return 0:success -1:failed
 */
int process_update_room_buf(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  获得用户的所有房间id
 *
 * @return 0:success -1:failed
 */
int process_get_room_num(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  购买商品
 *
 * @return 0:success -1:failed
 */
int process_buy_stuff(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  拉取益智游戏信息
 *
 * @return 0:success -1:failed
 */
int process_get_puzzle_info(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  提交益智游戏信息
 *
 * @return 0:success -1:failed
 */
int process_commit_puzzle(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  获得用户信息
 *
 * @return 0:success -1:failed
 */
int process_get_user_info(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  获得用户留言板
 *
 * @return 0:success -1:failed
 */
int process_get_pinboard(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  新增留言
 *
 * @return 0:success -1:failed
 */
int process_add_message(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  新增留言
 *
 * @return 0:success -1:failed
 */
int process_update_message_status(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  顶用户的小屋
 *
 * @return 0:success -1:failed
 */
int process_rating_user_room(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  更新新手引导标志位
 *
 * @return 0:success -1:failed
 */
int process_update_flag(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  更新用户的profile信息
 *
 * @return 0:success -1:failed
 */
int process_update_profile(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  怪兽吃东西的协议
 *
 * @return 0:success -1:failed
 */
int process_eat_food(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief 提交小游戏答题结果
 *
 * @return 0:success -1:failed
 */
int process_commit_game(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief 对吸引到的小怪兽的操作
 *
 * @return 0:success -1:failed
 */
int process_attract_pet(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief 删除一株植物
 *
 * @return 0:success -1:failed
 */
int process_maintain_plant(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief 种一株植物
 *
 * @return 0:success -1:failed
 */
int process_grow_plant(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief 获得种植园种子信息
 *
 * @return 0:success -1:failed
 */
int process_get_plant_info(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  获得好友列表
 *
 * @return 0:success -1:failed
 */
int process_get_friend_list(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief 对好友的操作
 *
 * @return 0:success -1:failed
 */
int process_friend_op(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief 查找好友的操作
 *
 * @return 0:success -1:failed
 */
int process_find_friend(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief 增加好友的操作
 *
 * @return 0:success -1:failed
 */
int process_add_friend(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief 获得好友的申请
 *
 * @return 0:success -1:failed
 */
int process_get_friend_apply(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief 好友的申请的操作
 *
 * @return 0:success -1:failed
 */
int process_friend_apply_op(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  拉取商店的物品
 *
 * @return 0:success -1:failed
 */
int process_get_shop_item(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  获得街道上的活跃用户
 *
 * @return 0:success -1:failed
 */
int process_get_stranger(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  将用户添加到好友缓存
 *
 * @return 0:success -1:failed
 */
int process_add_friend_to_cache(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  将用户的物品兑换成金币
 *
 * @return 0:success -1:failed
 */
int process_trade_stuff(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief  获得精灵园里的所有精灵
 *
 * @return 0:success -1:failed
 */
int process_get_pet(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);


/**
 * @brief process_enter_two_puzzle 进入二人益智游戏比赛
 *
 * @param p_user
 * @param p_msg_body
 * @param msg_body_len
 *
 * @return
 */
int process_enter_two_puzzle(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief process_start_two_puzzle 点击start按钮
 *
 * @param p_user
 * @param p_msg_body
 * @param msg_body_len
 *
 * @return
 */
int process_start_two_puzzle(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);//40135

/**
 * @brief process_answer_two_puzzle 答题
 *
 * @param p_user
 * @param p_msg_body
 * @param msg_body_len
 *
 * @return
 */
int process_answer_two_puzzle(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

int process_interactive_element(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);


/**
 * @brief  向ucount查询用户有没有访问过
 *
 * @return 0:success -1:failed
 */
int process_ucount_add_visit(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief 益智游戏多人对战增加金币
 *
 * @return 0:success -1:failed
 */
int process_multi_puzzle_add_coin(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief as向后台发送的统计数据
 *
 * @return 0:success -1:failed
 */
int process_stat_data(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief as向后台拉取最近访客
 *
 * @return 0:success -1:failed
 */
int process_get_latest_visit(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief as向后台拉取实际可见的留言条数
 *
 * @return 0:success -1:failed
 */
int process_get_real_message_count(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);


/**
 * @brief process_get_all_badge 打开成就面板，查看所有的成就项
 *
 * @param p_user
 * @param p_msg_body
 * @param msg_body_len
 *
 * @return 0:success -1:failed
 */
int process_get_all_badge(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);


int process_update_badge_status(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

/**
 * @brief process_get_badge_reward 领取成就项奖杯
 *
 * @param p_user
 * @param p_msg_body
 * @param msg_body_len
 *
 * @return
 */
int process_get_badge_reward(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

int process_get_unread_visit(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

int process_get_factory(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

int process_create_stuff(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

int process_factory_op(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

int process_add_new_unlock_map(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);
int process_get_game_level(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);
int process_check_dirty_word(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);
int process_modify_monster_name(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);
int process_get_npc(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);
int process_get_random_name(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);
int process_get_bobo_reward(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);
int process_read_bobo_newspaper(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);
int process_get_sun_reward(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);
int process_enter_npc_score(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);
int process_npc_score(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

int process_get_game_change(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);
int process_game_change(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);


int process_get_finished_task(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);
int process_finish_task(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);


int process_cur_denote_info(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);
int process_denote_coins(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);
int process_history_denote(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);



int process_enter_museum(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

int process_commit_museum_game(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

int process_get_museum_reward(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

int process_commit_game_change_score(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);


int process_update_guide_flag(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);


int process_reload_conf(fdsession_t *fdsess, char *p_msg_body, uint16_t msg_body_len);


int process_history_donate(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);//重写捐助计划的三个协议
int process_cur_donate_info(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);//重写捐助计划的三个协议
int process_donate_coins(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);//重写捐助计划的三个协议
int process_prize_lottery(usr_info_t * p_user,char * p_msg_body,uint16_t msg_body_len);

int process_get_a_activity_info(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);
int process_get_activity_reward(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);
int process_get_activity_list(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);
int process_get_activity_reward_ex(usr_info_t * p_user,char * p_msg_body,uint16_t msg_body_len);

int process_update_activity_status(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len);

int process_enter_show(usr_info_t * p_user,char * p_msg_body,uint16_t msg_body_len);
int process_join_show(usr_info_t * p_user,char * p_msg_body,uint16_t msg_body_len);
int process_guess_show(usr_info_t * p_user,char * p_msg_body,uint16_t msg_body_len);
int process_history_show(usr_info_t * p_user,char * p_msg_body,uint16_t msg_body_len);


#endif //H_CLI_PROTO_H_20110730
