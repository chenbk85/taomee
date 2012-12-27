#ifndef _HOME_ATTR_H_
#define _HOME_ATTR_H_

#include"fwd_decl.hpp"


typedef struct home_attr_data
{
	home_attr_data()
	{
		home_level = 0;
		home_exp = 0;
		db_flag = 0;
	}
	
	uint32_t home_level;
	uint32_t home_exp;
	uint32_t db_flag;
	uint32_t last_randseed_tm;
}home_attr_data;

struct home_attr_data_rsp_t
{
	uint32_t home_level;
	uint32_t home_exp;
	uint32_t last_randseed_tm;
}__attribute__((packed));

struct home_replace_exp_level_rsp_t
{
	uint32_t home_level;
	uint32_t home_exp;
	uint32_t fumo_point;
}__attribute__((packed));


int db_get_home_data(Player* p);

int db_get_home_data_callback( Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret );

int db_set_home_update_tm(uint32_t owner_id, uint32_t owner_tm, uint32_t last_update_tm);

int db_replace_home_exp_level(Player* p, uint32_t exp, uint32_t level, uint32_t fumo_point, bool callback = true);

int db_replace_home_exp_level_callback( Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int db_update_active_point(Player* p, uint32_t active_point, uint32_t last_tm);

 
#endif
