/*
 * =====================================================================================
 *
 *       Filename:  Cuser_farm_animal.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/03/2009 11:55:36 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef  CUSER_FARM_ANIMAL_INC
#define  CUSER_FARM_ANIMAL_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
/* 农场的相关操作 */

bool attire_id_is_mature_id_in_pic_list(const uint32_t id);

class Cuser_farm_animal : public CtableRoute100x10 {
	private:

		int set_eattime(animal_select *p_list, uint32_t *food_num);

		int get_sick_time(animal_select *p_animal, uint32_t *drink_time, uint32_t *eat_time, uint32_t water_time);

		int cal_growth_ex(uint32_t id, uint32_t cal_time, uint32_t eat_time, uint32_t drink_time, uint32_t *value);

		int cal_growth(uint32_t id, uint32_t cal_time, uint32_t eat_time,
			uint32_t drink_time, uint32_t *value);
		int set_favor(uint32_t animal_id, uint32_t *p_favor);

		int set_diff_mature(uint32_t animal_id, uint32_t grow_rate, uint32_t cur_value, uint32_t *p_diff_mature);
		
		int set_pollinate_num(uint32_t animal_id, uint32_t pollinate_tm, uint32_t *p_pollinate_num);

		int set_total_output(uint32_t animal_id, uint32_t *p_total_output);

		uint32_t rand_max_output(uint32_t animalid, uint32_t *p_count);

	public:

		Cuser_farm_animal(mysql_interface *db);

		int  insert(userid_t userid, uint32_t animalid, uint32_t type, uint32_t *p_id, 
				uint32_t *p_max_output, uint32_t level = 0);

		int get_animal(userid_t userid, animal_select **pp_list, uint32_t *p_count);

		int update(userid_t userid, animal_select *p_list);

		int del(userid_t userid, uint32_t id);
		
		int del_insect(userid_t userid, uint32_t animalid);

		int get_adult_id(uint32_t animalid, uint32_t *p_adultid);

		int get_output_id(uint32_t animalid, uint32_t *p_output_id);

		int get_drought_interval(uint32_t animalid, uint32_t *p_id);

		int get_one_animal_info(userid_t userid, uint32_t id, animal_select *p_list, uint32_t *p_animal_type);

		int cal_list(userid_t userid, animal_select *p_list, noused_homeattirelist *p_farm, 
				    uint32_t water_time, uint32_t *p_output, uint32_t *p_runout, uint32_t child_flag,
					uint32_t end_time, uint32_t& animal_drink, uint32_t& animal_catch);

		int animal_drink(userid_t userid, animal_select **p_list, uint32_t *p_count);

		int land_animal_count(userid_t userid, uint32_t *p_count);

		int get_all_animal(userid_t userid, stru_farm **pp_list, uint32_t *p_count);

		int water_animal_count(userid_t userid, uint32_t *p_count);

		int update_all(userid_t userid, stru_farm *p_list);

		int get_animal_by_type(userid_t userid,uint32_t type, animal_select **pp_list, uint32_t *p_count);

		int get_animal_type(uint32_t animalid, uint32_t *p_type);

		int cal_favor(uint32_t drink_time, uint32_t eat_time, uint32_t *p_favor_time,
				      uint32_t *p_favor, uint32_t *p_outgo, uint32_t *p_runout);

		int set_or_cancel_outgo(userid_t userid, uint32_t id, uint32_t flag);

		int set_eat_time(userid_t userid, uint32_t id);

		int get_animal_id(userid_t userid, uint32_t id, uint32_t *p_animalid);

		int get_outgo_count(userid_t userid, uint32_t *p_count);

		int update_outgo_zero(userid_t userid);

		int get_init_favor(uint32_t animal_id); 

		int net_catch_fish(userid_t userid, user_farm_net_catch_fish_out_item **pp_list, uint32_t *p_out_count);

		int get_mature_fish(userid_t userid, user_farm_get_mature_fish_out **pp_list, uint32_t *p_out);

		int get_animal_by_type_ex(userid_t userid, uint32_t type, animal_select **pp_list, uint32_t *p_count);

		int get_cur_grow(uint32_t animal_id, uint32_t state,  uint32_t *p_cur_grow);

		int get_mature(uint32_t animal_id, uint32_t *p_mature);

		int get_exp(uint32_t animalid, uint32_t *p_output_id);

		int get_outgo(userid_t userid, uint32_t id, uint32_t *p_count);

		int get_wool(userid_t userid, uint32_t id);

		uint32_t fruit_max(uint32_t animal_type);

		uint32_t check_insect_id(uint32_t animal_id, uint32_t *p_ret);

		int type_animal_count(userid_t userid, uint32_t type, uint32_t *p_count);

		int release_insect(userid_t userid, uint32_t id, uint32_t type);

		int get_land_animal_insect(userid_t userid, animal_select **pp_list, uint32_t *p_count);
		
		int update_animal_flag_sql(userid_t userid, uint32_t index, uint32_t id);

		int  butterfly_pollinate(userid_t userid, uint32_t id);

		int update_colum_sql(userid_t userid, uint32_t id, const char *p_char, uint32_t value);

		uint32_t get_butterfly_max_polliname_num(const uint32_t animal_type, uint32_t *p_pollinate_num);

		int get_one_animal_info_ex(userid_t userid, uint32_t id, animal_select *p_list,
	        uint32_t *p_animal_type, uint32_t *p_pollinate_tm);

		int update_two_colum_sql(userid_t userid, uint32_t id, const char *p_colum1,
	                 	const char *p_colum2, uint32_t colum1_value, uint32_t colum2_value);

		int fill_attribute(animal_select *p_item);

		int get_one_animal_count_sql(userid_t userid, uint32_t attireid, uint32_t *p_count);

		int special_animal_check(userid_t userid, uint32_t animalid);

		uint32_t get_animal_kind(const uint32_t animal_type, uint32_t *p_kind);

		uint32_t check_animal_max_limit(userid_t userid, uint32_t type);

		uint32_t check_both_buck_doe(animal_select **pp_list, uint32_t count, uint32_t *p_flag);

		uint32_t set_animal_flag( userid_t userid, uint32_t id, uint32_t index);

		int get_table_mature(userid_t userid, userid_t id,  uint32_t &mature);
		
		int  milk_cow(userid_t userid, uint32_t id);

		int get_animal_attireid_and_cnt(userid_t userid, uint32_t *p_count, user_get_attireid_and_cnt_out_item** pp_list);
		int speed_cal_growth(userid_t userid, uint32_t id, user_farm_speed_animal_growth_out *out, 
				uint32_t up_growth_value, uint32_t &output);
		int get_growth_level(userid_t userid, uint32_t id,  user_fairy_get_growth_out *p_out);
		int judge_egg(uint32_t &animalid, uint32_t &level);

		const uint32_t insetct_animal;

		const uint32_t aquatic_animal;

		const uint32_t terrestrial_animal;	

		const uint32_t silkworm;	

};


#endif   /* ----- #ifndef CUSER_INC  ----- */
