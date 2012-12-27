/*
 * =====================================================================================
 *
 *       Filename:  other_active.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/19/2011 09:40:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */

#ifndef OTHER_ACTIVE_HPP
#define OTHER_ACTIVE_HPP
extern "C" {
#include <stdint.h>
}

#include <libtaomee++/conf_parser/xmlparser.hpp>

struct player_t ;

enum {
	ACTIVE_BUF_LEN = 40,
    ACTIVE_INFO_LEN = 44,
};

enum active_id_t {
	active_daily_zhanbo = 1,
	active_player_attribute = 2,
	active_player_bet = 3,
	active_red_blue = 9,
};

struct other_active_store_rsp_t {
	uint32_t active_id;
	uint8_t  info_buf[ACTIVE_BUF_LEN];
}__attribute__((packed));

struct other_active_t {
	uint32_t active_id;
	uint8_t  info_buf[ACTIVE_BUF_LEN];
}__attribute__((packed));

struct other_active_info_rsp_t {
	uint32_t active_cnt;
	uint8_t  datas[];
}__attribute__((packed));

struct single_other_active_rsp_t {
    uint32_t    active_id;
    uint8_t     datas[40];
}__attribute__((packed));

struct player_t;
int calc_can_drop_box_cnt_2_btl(player_t * p); 

int get_xunlu_reward_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int get_xunlu_active_info_cmd(player_t *p, uint8_t * body, uint32_t bodylen);


int save_player_can_drop_box_cnt(player_t *p, uint32_t cnt);


int random_master_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int change_master_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

void get_reward_time_when_login(player_t * p);

struct red_blue_info_t {
  red_blue_info_t()
  {
	  cur_banner = 0;
	  start_tm = 0;
	  pvp_lv = 0;
	  random_master = 0;
	  cur_score = 0;
	  left_time = 0;
	  protect_banner = 0;
	  rob_banner = 0;

	  db_flag = 0;
  }
  uint32_t cur_banner;
  uint32_t start_tm;
  uint32_t pvp_lv;
  uint32_t random_master;
  uint32_t cur_score;
  uint32_t left_time;
  uint32_t protect_banner;
  uint32_t rob_banner;
  uint32_t db_flag;
//  uint32_t total_score;
};

enum rand_team_t {
	no_team = 0,
	red_team = 1,
	blue_team = 2
};

struct red_blue_rsp_t {
	uint32_t red_blue_team;
	uint32_t protect_banner;
	uint32_t rob_banner;
	uint32_t red_blue_score;
}__attribute__((packed));

void proc_player_banner_logic_after_btl_over(player_t * p, uint32_t win_lose_flag);

int protect_banner_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int rob_banner_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

void change_player_banner_battle_lv(void * owner, void * data);

void proc_player_red_blue_logic(player_t * p);

int get_red_blue_info_callback(player_t* p, other_active_store_rsp_t * rsp);

int save_player_red_blue_info(player_t * p);

int list_all_banner_info_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int cancel_protect_banner_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int del_player_red_blue_score(player_t * p, uint32_t del_count);


int save_player_red_blue_info(player_t * p);

int db_get_other_active_info(player_t * p, uint32_t active_id);

int db_get_other_active_info_callback(player_t * p, uint32_t id, void * body, uint32_t bodylen, uint32_t ret);

struct active_out_date_data_t {
	uint32_t active_id;
	uint32_t reset_type;
	uint32_t out_tm;
};

active_out_date_data_t * get_active_out_date_info(uint32_t active_id);

int load_active_out_date(xmlNodePtr xml);

//--------------------------------
int get_player_other_active_info_cmd(player_t * p, uint8_t * body, uint32_t bodylen);


int db_get_other_active_info_list_callback(player_t * p, uint32_t id, void * body, uint32_t bodylen, uint32_t ret);

int save_player_active_info_2(player_t *p, uint32_t key, uint8_t *value);

//-------------------------------------

int get_player_single_other_active_cmd(player_t * p, uint8_t * body, uint32_t bodylen);
int db_get_single_other_active_callback(player_t * p, uint32_t id, void * body, uint32_t bodylen, uint32_t ret);

#endif


