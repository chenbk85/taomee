/*
 * =====================================================================================
 *
 *       Filename:  Cuser_fish.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/22/2012 11:22:09 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef CUSER_FISH_INC
#define CUSER_FISH_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

struct fish_t{
	uint32_t index;
	uint32_t fishid;
	uint32_t breed;
	uint32_t birthday;
	uint32_t growth;
	int32_t state;
	uint32_t hungry;
	int32_t lifetime;
	uint32_t feed_time;
	uint32_t caltime; 
	uint32_t yield_time; 
};

struct fish_drop_t{
	uint32_t index;
	uint32_t fishid;
	uint32_t birthday;
	uint32_t growth;
	int32_t state;
	int32_t lifetime;
	uint32_t drop_time; 
};

struct fish_info_t{
	uint32_t fishid;
	int32_t lifetime;
	uint32_t interval;
	uint32_t checkpoint[2];
	uint32_t product[3];
	uint32_t food[3];
	uint32_t shell[2];
	uint32_t price;
};

struct cal_shell_t{
	uint32_t index;
	uint32_t fishid;
	uint32_t growth;
	int32_t lifetime;
	uint32_t birthday;
	uint32_t yield_time;
};

class Cuser_fish:public CtableRoute100x10{
	private:
	
		int cal_fish_info(fish_t *p_tmp, uint32_t p_cnt, user_enter_ocean_out_item_2* list,
			uint32_t *count, uint32_t userid, uint32_t oceanid, uint32_t* shells);
		int change_fish(userid_t userid, uint32_t oceanid, fish_t *fish);
		int get_pos(uint32_t fishid);
	public:
		Cuser_fish(mysql_interface *db);

		int raise_fish(uint32_t userid, uint32_t oceanid, uint32_t fishid, uint32_t breed);
		
		int update_one_col(userid_t userid, uint32_t oceanid, uint32_t fishid, const char* col, uint32_t value);
		int update_double_col(userid_t userid, uint32_t oceanid, uint32_t fishid,
			   	const char* col_1, const char* col_2, uint32_t value_1, uint32_t value_2);
		int get_one_col(userid_t userid, uint32_t oceanid, uint32_t fishid, const char* col, uint32_t* value);
		int get_double_col(userid_t userid, uint32_t oceanid, uint32_t fishid, 
				const char* col_1, const char* col_2, uint32_t *value_1, uint32_t* value_2);

		int get_ocean_fish(userid_t userid, uint32_t oceanid, user_enter_ocean_out_item_2* p_list, uint32_t *p_count,
				uint32_t* shells);
		int get_count(userid_t userid, uint32_t oceanid, uint32_t *count);
		int check_match_food(uint32_t fishid, uint32_t foodid);
		int cal_output_shells(userid_t userid, uint32_t oceanid, uint32_t *shells);
		int get_one_fish(userid_t userid, uint32_t oceanid, user_put_in_ocean_out* out);
		int clear_last_drop_shell_time(userid_t userid);
		int get_online_shells(userid_t userid, uint32_t oceanid, 
				fish_drop_t& fish, uint32_t *itemid, uint32_t *count);
		int get_online_fish_output(userid_t userid, uint32_t oceanid, uint32_t *p_count, 
				user_get_ocean_online_shells_out_item* p_out_item);
		int cal_price_result(userid_t userid, user_sale_ocean_fish_in_item* p_in_item, 
			uint32_t count, uint32_t* shell_cnt);
		int drop_fish(userid_t userid, uint32_t fish_index);
		int raise_fish(uint32_t userid, uint32_t oceanid, uint32_t fishid, uint32_t breed, uint32_t growth, uint32_t hungry);
		int raise_fish(uint32_t userid, uint32_t oceanid, uint32_t fishid, uint32_t breed, uint32_t hungry);
		int cal_offline_shells(cal_shell_t* p_tmp, uint32_t k, uint32_t now, uint32_t* shells);
};
#endif

