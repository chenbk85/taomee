/*
 * =====================================================================================
 *
 *       Filename:  rank_top.hpp
 *
 *    Description:  单人排行榜信息相关
 *
 *        Version:  1.0
 *        Created:  10/24/2011 01:03:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */

#ifndef RANK_TOP_HPP
#define RANK_TOP_HPP

extern "C" {
#include <stdint.h>
}
#include "fwd_decl.hpp"
#include <map>

struct rank_info_t {
  uint32_t userid;
  uint32_t role_tm;
  uint32_t role_type;
  char nick_name[max_nick_size];
  uint32_t score; 
}__attribute__((packed));

struct single_rank_info_t {
  uint32_t userid;
  uint32_t role_tm;
  uint32_t role_type;
  uint32_t level;
  char nick_name[max_nick_size];
  uint32_t score; 
}__attribute__((packed));

struct team_rank_info_t {
	uint32_t team_id;
	uint32_t team_score;
	uint32_t captain_id;
	uint32_t captain_role_tm;
	uint8_t  captain_nick[max_nick_size];
	uint8_t  team_nick[max_nick_size];
	uint32_t  team_member_cnt;
}__attribute__((packed));

enum {
 ranker_refresh_time = 60
};

struct ranker_range_t {
	uint32_t active_id;
	int      range; // -1, or 0 ->
};

struct player_range_t {
	std::map<uint32_t, ranker_range_t> * single_active_range; 
	std::map<uint32_t, ranker_range_t> * team_active_range;
	player_range_t()
	{
		single_active_range = new std::map<uint32_t, ranker_range_t>;
		team_active_range = new std::map<uint32_t, ranker_range_t>;
	}
};
//active > 10000 for team_active
int get_player_active_range(player_t * p, uint32_t active_id);



struct player_t;

void proc_auto_ranker_event();

int proc_auto_list_rank_info_callback(void * body, uint32_t bodylen, uint32_t ret);

int db_insert_rank_info(player_t * p, uint32_t rank_id, rank_info_t * info);

int db_save_rank_info(player_t * p, uint32_t rank_id, uint32_t score);

int player_list_ranker_info_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int db_list_ranker_callback(player_t *p, uint32_t uid, void * body, uint32_t bodylen, uint32_t ret);

int get_player_ranker_range(player_t *p, uint32_t ranker_id);

void init_player_rank(player_t * p, rank_info_t * rank, uint32_t score);

int list_team_active_ranker_cmd(player_t * p, uint8_t * body, uint32_t bodylen);
struct cachesvr_proto_t;
int list_team_active_ranker_callback(player_t * p, cachesvr_proto_t * data);

void add_player_active_score(player_t * p, uint32_t active_id, uint32_t add_score);
int list_single_active_ranker_cmd(player_t * p, uint8_t * body, uint32_t bodylen);
int list_single_active_ranker_callback(player_t *p, cachesvr_proto_t * data);
#endif


