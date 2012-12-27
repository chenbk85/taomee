/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_jy.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 13时47分01秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_JY_INCL
#define  CUSER_JY_INCL
#include <cstdlib>
#include <ctime>
#include "serlib/db_utility.hpp"
#include <map>
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

bool attire_id_is_fruitid_in_pic_list(const uint32_t id);

class Cuser_jy:public CtableRoute100x10
{
public:
	Cuser_jy(mysql_interface * db ); 

	int get_list_db(userid_t userid,  uint32_t *p_count, 
	 jy_item **pp_out_item );
	int kill_bug(jy_item *p_out_item);

	int get(userid_t userid, uint32_t id, jy_item *p_out_item);
	int get(userid_t userid, uint32_t id, jy_item_muck *p_out_item);

	int water_seed(jy_item *P_out_item);
	/*判断果实是否过期*/
	int fruit_is_bad(jy_item *p_out_item);
	/*得到养成值*/
	int get_value_ex(uint32_t *value, uint32_t calc_value_time, uint32_t drought_time,
					uint32_t insect_time, uint32_t id);
	/*更新种子状态*/
	int update_seed_info(userid_t userid, jy_item *pp_out_item);
	int update_seed_info(userid_t userid, jy_item_muck *p_out_item);
	/*对种子信息进行处理*/
	int get_list(userid_t userid,  uint32_t *p_count, 
	 jy_item **pp_out_item, uint32_t end_time, uint32_t& insect_kill, uint32_t& plant_water);

	int add(userid_t userid,  uint32_t now,  user_add_seed_in *p_in, uint32_t  *p_id);
	int del(userid_t userid,  uint32_t  id);
	int get_fruitid(uint32_t seedid, uint32_t pollinate_num, uint32_t *p_fruitid);
	int update_fruit(userid_t userid,uint32_t id, uint32_t fruitnum );

	int get_count(userid_t userid, uint32_t *p_count );

	int update_seed_info_ex(userid_t userid, user_set_seed_ex_in *p_out_item);
	int get_xy_count(userid_t userid, uint32_t *p_count, uint32_t x, uint32_t y);
	int thieve_fruit(userid_t userid, userid_t thiever_id, uint32_t id, jy_item *out);
	int get_list_db_diff(userid_t userid, uint32_t *p_count, jy_item **pp_out_item);
	int get_thiever(userid_t userid, uint32_t id, thiever_info *p_list);
	int get_ex(userid_t userid, uint32_t id, jy_item *p_item);
	int update_earth(userid_t userid, uint32_t id);
	int get_exp(uint32_t seedid, uint32_t *p_exp);
	int get_flower_max_pollinate_num(uint32_t seedid, uint32_t *p_fruitid);
	int get_column(userid_t userid, uint32_t id, const char *p_column, uint32_t *p_value);
	int update_column(userid_t userid, uint32_t id, const char *p_column, uint32_t value);
	int butterfly_pollinate(uint32_t userid, uint32_t id, jy_item *p_item);
	int fill_attribute(jy_item *p_in);
	int get_mature_fruit(userid_t userid, uint32_t id, uint32_t *p_mature_fruit);


	/**
	 * @brief 当用户可以施肥的时候，对数据进行清理和计算
	 * @param struct jy_item_muck& jy_item 需要清理的数据
	 * @param user_plant_growth_add_by_hour_in& in 可能需要的依赖数据
	 */
	void plant_muck_clean(struct jy_item_muck& jy_item, user_plant_growth_add_by_hour_in& in);
	int get_jy_item_list(userid_t userid, uint32_t *p_count, jy_item **pp_out_item );
	int get_jy_attireid_and_cnt(userid_t userid, uint32_t *p_count, user_get_attireid_and_cnt_out_item** pp_list);

private:
	int cal_seed(userid_t userid, jy_item *p_item, uint32_t end_time,uint32_t& insent_kill, uint32_t& plant_water);

	int get_value(uint32_t *value, uint32_t calc_value_time,
			uint32_t drought_time, uint32_t insect_time, uint32_t id);
	int update_thiever_fruitnum(userid_t userid, uint32_t id, thiever_info *p_list, uint32_t fruitnum);
	/*得到健康状态*/
	int drought_insect_good(uint32_t water_time, uint32_t calc_value_time, 
			uint32_t id, uint32_t *state, uint32_t *drought_time, uint32_t *insect_time);
	int get_pollinate_num(uint32_t seedid, uint32_t *p_num);
	int get_pollinate_min_max_grow(uint32_t seedid, uint32_t *p_min, uint32_t *p_max);
	int get_fruitid_accrod_pollinate_num(uint32_t seedid, uint32_t pollinate_num, uint32_t *p_fruitid);
	int get_init_pollinate_num(uint32_t seedid, uint32_t *p_init);
	int get_grow_rate(uint32_t attireid, uint32_t sick_flag, uint32_t *p_grow_rate);
	int get_diff_mature(uint32_t attireid, uint32_t grow_rate, uint32_t cur_value, uint32_t *p_diff_mature);
	
	char *msglog_file;
};

#endif   /* ----- #ifndef CUSER_JY_INCL  ----- */

