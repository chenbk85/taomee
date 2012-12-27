#ifndef _HOME_IMPL_H_
#define _HOME_IMPL_H_

#include"fwd_decl.hpp"

struct set_home_back_rsp_t {
		uint32_t home_level;
		uint32_t home_exp;
		uint32_t dec_fumo;
}__attribute__((packed));
	

typedef struct player_home_data
{
	player_home_data()
	{
		home_exp = 0;
		home_level = 0;
		home_action_point = 0;
		last_tm = 0;
	}
	
//	
//	bool is_in_same_day()
//	{
//		uint32_t cur_tm = time(NULL);
//		return (cur_tm / 86400) == (last_tm/ 86400);
//	}
//
//	void reset_action_point();
//	void add_action_point(uint32_t point);
//	void dec_action_point(uint32_t point);	
//	void set_action_point(uint32_t point, uint32_t tm);
	uint32_t home_exp;
	uint32_t home_level;
	uint32_t home_action_point;
	uint32_t last_tm;
}player_home_data;

typedef struct get_home_data_rsp_t
{
	uint32_t home_level;
	uint32_t home_exp;
}get_home_data_rsp_t;


bool init_player_home_data(player_t *p );

bool final_player_home_data(player_t *p );

bool load_player_home_data(player_t *p, get_home_data_rsp_t* rsp);

int  db_set_home_data(player_t * p, uint32_t home_lv, uint32_t home_exp, uint32_t dec_fumo = 0); 

int db_set_home_data_callback(player_t *p, userid_t id, void * body, uint32_t bodylen, uint32_t ret);

int  db_get_player_home_data(player_t *p);

int  db_get_home_data_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

//int  db_insert_active_point_2_home(player_t * p, uint32_t active_point);

//int  db_home_lv_up(player_t * p, uint32_t lv, uint32_t dec_fumo_point);

//int  db_update_home_active_point(player_t* p, uint32_t point, uint32_t last_tm);


#endif
