/*
 * =====================================================================================
 *
 *       Filename:  contest.hpp
 *
 *    Description:  天下第一比武大会
 *
 *        Version:  1.0
 *        Created:  05/23/2011 05:34:40 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#ifndef CONTEST_HPP
#define CONTEST_HPP
extern "C" {
#include <stdint.h>
}

struct player_t;

#pragma pack(1)

struct join_contest_team_rsp_t {
	uint32_t team_id; //加入队伍 1-5， 仁义礼智信
};

struct contest_team_rsp_t {
	uint32_t team_id; //队伍ID
	uint32_t leader_id; //队长ID
	uint32_t leader_tm; //队长tm
	uint8_t  leader_name[16]; //队长昵称
	uint32_t team_member; //队员人数
	uint32_t team_plant; //总的复活草数目
};

struct get_contest_team_t {
	uint32_t team_cnt;
	uint8_t  data[];
};

struct top_donate_t {
	uint32_t max_id;
	uint32_t max_tm;
	uint8_t  max_name[16];
	uint32_t max_plant;
};

struct contest_donate_rsp_t {
	uint32_t self_team_id;
	uint32_t self_plant; //自己捐献的复活草数目
	uint32_t top_cnt;
	top_donate_t top[];
};

struct contest_donate_plant_rsp_t {
	uint32_t type_cnt1; //赠送类复活草捐赠数目
	uint32_t type_cnt2; //商场类复活草捐赠数目
};

struct pvp_game_data_t {
    pvp_game_data_t() {
        do_times = 0;
        win_times = 0;
    }
    uint32_t    do_times;
    uint32_t    win_times;
};

struct pvp_watch_data_t {
	pvp_watch_data_t() {
		battle_id = 0;
		room_id = 0;
		watch_id = 0;
		room_status = 0;
	}


	uint32_t battle_id;
	uint32_t room_id;
	uint32_t watch_id;
	uint32_t room_status;
};

struct final_game_score_t {
    uint32_t    userid;
    uint32_t    roletm;
    uint8_t    nick[16];
    uint32_t    lv;
    uint32_t    score;
};

struct pvp_game_rank_t {
    uint32_t    timestamp;
    //uint32_t    type;
    uint32_t    cnt;
    final_game_score_t score[100];
};

struct pvp_game_rank_cache_t {
    pvp_game_rank_t rank[2];
};

#pragma pack()

pvp_watch_data_t * create_watch_info(player_t * p);
void destory_watch_info(player_t * p, pvp_watch_data_t * info);


bool is_hundred_hunter(player_t *p, uint32_t id);
uint32_t get_taotai_can_join_times(player_t* p);
uint32_t get_advance_can_join_times(player_t *p);
uint32_t get_final_can_join_times(player_t *p);

/**
 * @brief 监测比武大会时间
 */
bool is_contest_first_stage_over();
bool is_contest_second_stage_over();
bool is_contest_final_stage_over();


bool check_contest_game_limit(player_t* p, uint32_t swap_id);
int form_contest_game_errcode(uint32_t swap_id);
/** 
 * @brief 加入一个武斗队伍（仁， 义， 礼， 智， 信） 
 * 
 */
int join_contest_team_cmd(player_t * player, uint8_t * body, uint32_t bodylen);


/** 
 * @brief 获取武斗队伍信息 
 * 
 */
int get_contest_team_info_cmd(player_t * player, uint8_t * body, uint32_t bodylen);


/** 
 * @brief  获得当期队伍的复活草捐献信息
 * 
 */
int get_contest_donate_cmd(player_t * player, uint8_t * body, uint32_t bodylen);

int get_self_contest_cmd(player_t * player, uint8_t * body, uint32_t bodylen);


/** 
 * @brief 捐献复活草给武斗队伍 
 * 
 */
int contest_donate_plant_cmd(player_t * player, uint8_t * body, uint32_t bodylen);

/**
  * @brief player taotai game
  * @param p the player who scaned the times
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_taotai_game_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int get_advance_game_info_cmd(player_t *p, uint8_t * body, uint32_t bodylen);

int get_final_game_info_cmd(player_t *p, uint8_t * body, uint32_t bodylen);

int get_final_game_rank_list_cmd(player_t *p, uint8_t *body, uint32_t bodylen);


/**
 * @brief Team member fatch reward
 */
int team_member_reward_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

/**
 * @brief  Team member fatch reward callback
 */
int db_team_member_reward_callback(player_t *p, userid_t uid, void *body, uint32_t bodylen, uint32_t ret);

/** 
 * @brief  DB callback for join contest team
 * 
 */
int db_join_contest_team_callback(player_t * player, uint32_t uid, void * body, uint32_t bodylen,
		uint32_t ret);

/** 
 * @brief  DB callback for get all contest_team_info
 * 
 */
int db_get_contest_team_info_callback(player_t * player, uint32_t uid, void * body,uint32_t bodylen,
		uint32_t ret);

/** 
 * @brief  DB callback handle for donate plant
 * 
 */
int db_get_contest_donate_callback(player_t * player, uint32_t uid, void * body, uint32_t bodylen,
		uint32_t ret);

/** 
 * @brief 
 * 
 */
int db_contest_donate_plant_callback(player_t * player, uint32_t uid, void * body, uint32_t bodylen,
		uint32_t ret);


int db_donate_item_callback(player_t * player, uint32_t uid, void * body, uint32_t bodylen,
		uint32_t ret);

int db_get_self_contest_info_callback(player_t * player, uint32_t uid, void * body, 
		uint32_t bodylen, uint32_t ret);

int send_final_game_rank_list_to_player(player_t* p, uint32_t rank);

/**
  * @brief callback for get taotai game information
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param bodylen length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_get_taotai_game_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);


int db_get_advance_game_info_callback(player_t *p, userid_t id, void * body, uint32_t bodylen, uint32_t ret);

int db_get_pvp_game_info_callback(player_t * p, userid_t id, void * body, uint32_t bodylen, uint32_t ret);

int db_get_final_game_rank_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int save_taotai_game_data(player_t* p, bool win_flag);

int save_advance_game_data(player_t *p, bool win_flag);

int save_pvp_game_data(player_t* p, uint32_t pvp_type, bool win_flag);

int save_pvp_game_flower(player_t* p, uint32_t pvp_type, uint32_t flower);

int db_save_taotai_game_data_callback(player_t *p, userid_t uid, void *body, uint32_t bodylen, uint32_t ret);

int db_save_advance_game_data_callback(player_t *p, userid_t uid, void * body, uint32_t bodylen, uint32_t ret);

int player_gain_item_cmd(player_t * player, uint8_t * body, uint32_t bodylen);


int get_contest_pvp_list_cmd(player_t * player, uint8_t * body, uint32_t bodylen);


int contest_pvp_yazhu_cmd(player_t * player, uint8_t * body, uint32_t bodylen);

int contest_cancel_yazhu_cmd(player_t * player, uint8_t * body, uint32_t bodylen);

int db_contest_yazhu(player_t * p);

int get_city_battle_info_cmd(player_t * player, uint8_t * body, uint32_t bodylen);

int auto_join_contest_cmd(player_t * player, uint8_t * body, uint32_t bodylen);

int list_contest_group_info_cmd(player_t * player, uint8_t * body, uint32_t bodylen);

int leave_contest_group_cmd(player_t * player, uint8_t * body, uint32_t bodylen);


int contest_guess_champion_cmd(player_t * player, uint8_t * body, uint32_t bodylen);

int list_all_contest_player_cmd(player_t * player, uint8_t * body, uint32_t bodylen);

int list_all_passed_contest_cmd(player_t * player, uint8_t * body, uint32_t bodylen);

int	re_join_contest_cmd(player_t * player, uint8_t * body, uint32_t bodylen); 

#endif


