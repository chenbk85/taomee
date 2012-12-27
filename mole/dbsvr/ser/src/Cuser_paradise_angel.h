/*
 * =====================================================================================
 *
 *       Filename:  Cuser_paradise_angel.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/16/2011 01:54:20 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_PARADISE_ANGEL_INC
#define CUSER_PARADISE_ANGEL_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_paradise_angel:public CtableRoute100x10{
	private:
		char *msglog_file;
		int cal_growth(uint32_t index , uint32_t &growth, uint32_t last_caltime, 
        uint32_t hungry_time );	
		uint32_t cal_diff_mature(uint32_t index, uint32_t mature, uint32_t growth);
		int angel_variate(userid_t userid, uint32_t index, uint32_t variate_rate, uint32_t *variate_flag, uint32_t *variate_id
				, uint32_t *unsuccess_count, uint32_t count_1, uint32_t count_2, uint32_t angelid);
	public:
		int get_variate_specify(uint32_t userid,  uint32_t &count, uint32_t angelid);
		Cuser_paradise_angel(mysql_interface *db);
		bool  if_mature(uint32_t id, uint32_t growth);
		int cal_list(uint32_t userid,user_get_paradise_out_item *p_out_item , uint32_t *nimbus, 
				uint32_t *unsuccess_count, uint32_t count_1, uint32_t count_2);
		int set_angel_attribute_growth(uint32_t userid, uint32_t id, uint32_t growth,
				uint32_t last_caltime, uint32_t state, uint32_t variate_id, uint32_t variate_flag);
		int get_all(uint32_t userid, user_get_paradise_out_item **pp_list, uint32_t *count, uint32_t layer);
		int get_angelid_growth_variate(uint32_t userid, uint32_t id, uint32_t *angelid,       
				uint32_t *growth, uint32_t *variate_id, uint32_t *variate_flag);
		int get_all_angel_pos(userid_t userid, uint32_t *pos, uint32_t *count, uint32_t layer);
		int set_pos(userid_t userid, uint32_t id, uint32_t position);
		int insert(uint32_t userid, uint32_t angel_id, uint32_t postion, uint32_t layer);
		int free_angel(uint32_t userid, uint32_t id);
		int get_angel_by_position(uint32_t userid, uint32_t position, user_change_angel_pos_out *out);
		int get_two_column_by_id(uint32_t userid, uint32_t id, char column1[], char column2[],
			   	uint32_t *out_1, uint32_t *out_2);
		int get_increase_exp(uint32_t angelid, uint32_t *exp_inc);
		int get_angel_variate_id(uint32_t userid, uint32_t id, uint32_t *variate_id);
		int set_angel_attribute_growth_not_include_variate(uint32_t userid, uint32_t id, 
        		uint32_t growth, uint32_t last_caltime, uint32_t state);
		int update_only_state(uint32_t userid, uint32_t id, uint32_t state);
		int get_angelid_by_index(uint32_t userid, uint32_t id, uint32_t &angelid);
		int update_eattime(uint32_t userid, uint32_t id, uint32_t eattime);
		int get_last_eattime(uint32_t userid, uint32_t id, uint32_t &last_eattime);
		int get_angel_info_by_position(uint32_t userid, uint32_t position, 
          paradise_angel_item *out);
		int speed_up(uint32_t userid, uint32_t up_growth,  paradise_angel_item *out, uint32_t *count, uint32_t count_1, uint32_t count_2);
		bool is_mature(uint32_t angelid,  uint32_t growth);
		int update_growth(uint32_t userid, uint32_t id,  uint32_t up_growth);	
		int	update_growth_variate(uint32_t userid, uint32_t id, uint32_t vairate_flag, uint32_t variate_id);
		int speed_variate_rate(userid_t userid, uint32_t id, uint32_t angelid, uint32_t toolid, uint32_t *rate_inc);
		int get_pos_limit(uint32_t userid, uint32_t **pp_list, uint32_t *count);
};

#endif
