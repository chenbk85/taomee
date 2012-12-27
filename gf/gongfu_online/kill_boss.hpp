#ifndef _KILL_BOSS_H_
#define _KILL_BOSS_H_

#include"fwd_decl.hpp"
#include <vector>
#include <map>
using namespace std;


struct player_t;

typedef struct kill_boss_data
{
	kill_boss_data()
	{
		stage_id_ = 0;
		diffcult_ = 0;
		boss_id_  =0;
		kill_time_ = 0;
		pass_cnt_ = 0;
	}
	uint32_t stage_id_;
	uint32_t diffcult_;
	uint32_t boss_id_;
	uint32_t kill_time_;
	uint32_t pass_cnt_;
}kill_boss_data;


typedef struct kill_boss_element
{
	kill_boss_element()
	{
		stage_id = 0;
		boss_id = 0;
		kill_time = 0;
		pass_cnt = 0;
	}
	uint32_t stage_id;
	uint32_t boss_id;
	uint32_t kill_time;
	uint32_t pass_cnt;
}kill_boss_element;

typedef struct get_kill_boss_list_rsp_t
{
	uint32_t count;
	kill_boss_element datas[];
}get_kill_boss_list_rsp_t;



bool init_player_kill_boss_data(player_t* p);
bool final_player_kill_boss_data(player_t* p);
bool add_player_kill_boss_data(player_t* p, kill_boss_data* pdata);
bool add_player_kill_boss_data(player_t* p, uint32_t stage_id, uint32_t boss_id, uint32_t kill_time, uint32_t pass_cnt = 0);
bool is_player_finish_stage(player_t* p, uint32_t stage_id, uint32_t diffcult);
bool is_player_get_stage_score(player_t* p, uint32_t stage_id, uint32_t diff, uint32_t score);
int is_player_kill_boss_data_exist(player_t* p, uint32_t stage_id, uint32_t diffcult);
int is_player_kill_boss_data_exist(player_t* p, uint32_t stage_id);
kill_boss_data* get_player_kill_boss_data(player_t* p, uint32_t stage_id, uint32_t diffcult);
uint32_t get_player_kill_boss_count(player_t* p, uint32_t diffcult);

int db_kill_boss_list(player_t* p);
int db_update_kill_boss_list(player_t* p,  uint32_t stage_id, uint32_t boss_id, uint32_t kill_time, uint32_t pass_cnt);

int db_get_kill_boss_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
#endif
