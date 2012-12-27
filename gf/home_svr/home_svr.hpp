/*
 * =====================================================================================
 *
 *       Filename:  home_svr.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/04/2011 05:04:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#ifndef HOME_SVR_HPP
#define HOME_SVR_HPP
extern "C" {
#include <stdint.h>
}
#include "fwd_decl.hpp"

#pragma pack(1)

struct clothes_base_t {
    uint32_t    id;
    uint32_t    gettime;
    uint32_t    timelag;
    uint32_t    lv;
};


struct summon_t {
	uint32_t mon_type;
	uint32_t mon_tm;
	uint16_t lv;
	uint8_t nick[max_nick_size];
	uint8_t call_flag;
};

struct player_info_t {
	uint32_t id;
	uint32_t role_tm;
	uint32_t role_type;
	uint32_t power_user;
	uint32_t player_show_state;
	uint32_t vip;
	uint32_t vip_level;
	uint32_t achieve_title;
	char nick[max_nick_size];
	uint16_t lv;
	uint32_t flag;
	uint32_t app_mon;
	uint32_t honor;
	uint32_t coins;
	uint32_t xpos;
	uint32_t ypos;
	uint32_t clothes_cnt;
	uint16_t summon_cnt;
	uint32_t fight_mon_tm;
	//uint32_t clothes_id[];
    clothes_base_t clothes_arr[];
};

struct enter_home_rsp_t {
	uint32_t ownerid;
	uint32_t role_time;
	player_info_t info;
};

struct pet_move_t {
	uint32_t pet_tm;
	uint32_t x_pos;
	uint32_t y_pos;
	uint32_t dir;
};

struct player_off_line_rsp_t
{
	uint32_t userid;
	uint32_t role_regtime;
};

#pragma pack()

class Player;

/** 
 * @brief 进入自己的小屋
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int enter_home_cmd(Player * p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief 访问他人小屋
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int enter_friend_home_cmd(Player * p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief 离开当前的小屋
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int leave_home_cmd(Player * p, uint8_t * body, uint32_t bodylen);


/** 
 * @brief 宠物召唤出来
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int pet_call_cmd(Player * p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief 更改宠物状态
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int pet_callback_cmd(Player *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief 宠物移动
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int pet_move_cmd(Player *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief  转发小屋场景中玩家移动
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int player_walk_cmd(Player *p, uint8_t * body, uint32_t bodylen);


/** 
 * @brief 转发小屋场景中玩家移动
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int walk_keyboard_cmd(Player *p, uint8_t * body, uint32_t bodylen);


/** 
 * @brief  玩家站立停止
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int player_stand_cmd(Player *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief 玩家跳跃动作转发 
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int player_jump_cmd(Player *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief 说话转发
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int player_talk_cmd(Player *p, uint8_t * body, uint32_t bodylen);



/** 
 * @brief  小屋中玩家进入战斗 同步
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int player_enter_battle_cmd(Player *p, uint8_t * body, uint32_t bodylen);


/** 
 * @brief  小屋中玩家离开战斗 同步信息
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int player_leave_battle_cmd(Player *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief 拉去当前小屋中的玩家信息
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int list_user_cmd(Player *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief 拉去小屋主人的小屋宠物信息
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int list_pet_cmd(Player *p, uint8_t * body, uint32_t bodylen);

/** 
 * @brief 灵树保卫战 通知战斗服务器关卡中 灵兽数目 同步
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int syn_player_info_cmd(Player *p, uint8_t * body, uint32_t bodylen);

int player_off_line_cmd(Player *p, uint8_t * body, uint32_t bodylen);

int kick_off_player_cmd(Player *p, uint8_t * body, uint32_t bodylen);

int get_home_attr_cmd(Player *p, uint8_t * body, uint32_t bodylen);

int level_up_cmd(Player *p, uint8_t * body, uint32_t bodylen);


int query_player_count(fdsession_t* fdsess, uint8_t * body, uint32_t bodylen);
int query_player_count_cmd(Player *p, uint8_t * body, uint32_t bodylen);

#endif



