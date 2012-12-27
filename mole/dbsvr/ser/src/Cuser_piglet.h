/*
 * =====================================================================================
 *
 *       Filename:  Cuser_piglet.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/06/2011 10:47:08 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_PIGLET_INC
#define CUSER_PIGLET_INC


#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include <vector>
#include <set>

class Cuser_piglet:public CtableRoute100x10{
	private:
	
	uint32_t add_value(uint32_t now , uint32_t datetime);
	uint32_t cal_hungry_degree(uint32_t now, uint32_t feedtime, uint32_t feed_type);
	uint32_t set_piglet_state(uint32_t breed, uint32_t now, uint32_t birthday, uint32_t feedtime, 
				uint32_t feedtype, uint32_t pregnant, uint32_t lifetime, uint32_t growth_stage, 
				uint32_t train_cnt, uint32_t train_point, uint32_t sex);
	int check_able_make_pair(uint32_t feed_time, uint32_t birthday, uint32_t lifetime,
			uint32_t breed, uint32_t birth_cnt);
	int check_common_piglet(uint32_t feed_time, uint32_t birthday, uint32_t lifetime, uint32_t breed);
	uint32_t get_piglet_type(uint32_t breed);
	int get_for_cal_weight(userid_t userid, uint32_t index, uint32_t *growth_rate, uint32_t *feed_time, 
		uint32_t *growth_stage, uint32_t *cal_weight_time);
	public:
		Cuser_piglet(mysql_interface *db);
		uint32_t get_piglet_growth_state(uint32_t birthday, uint32_t lifetime, uint32_t breed, uint32_t now);
		int check_piglet_dead(uint32_t feed_time, uint32_t feed_type, uint32_t birthday, uint32_t lifetime);
		int get_piglet_real_breed(uint32_t pigletid, uint32_t &type);
		int birth(userid_t userid, uint32_t pigletid, char* nick, char* mother_nick, 
				char* father_nick, uint32_t breed, uint32_t sex, uint32_t weight, uint32_t charm,
			   	uint32_t strong, uint32_t birthday, uint32_t growth_rate, uint32_t growth_stage, 
				uint32_t lifetime, uint32_t parentid, uint32_t generate, uint32_t price, uint32_t state = 0);
		int update_two_cols(uint32_t userid, uint32_t id, const char* col_1, const char* col_2, 
				uint32_t value_1, uint32_t value_2);
		int select_two_cols(uint32_t userid, uint32_t id, const char* col_1, const char* col_2,
			   	uint32_t *value_1, uint32_t* value_2);
		int update(uint32_t userid, uint32_t id, const char* col, uint32_t value);
		int select(uint32_t userid, uint32_t id, const char* col, uint32_t *value);
		int get_all_piglet(uint32_t userid, user_get_piglet_house_out_item** p_out_item, uint32_t* p_count,
				uint32_t feed_type);
		int get_one_piglet(uint32_t userid, uint32_t index, uint32_t feed_type, 
				user_get_single_piglet_info_out* p_out, uint32_t effect_time, 
				uint32_t lasttime);
		int clear(userid_t userid);
		int get_live_piglet_feedtime(uint32_t userid, uint32_t** pp_list, uint32_t* p_count);
		int get_piglet_sum_hungry(uint32_t userid, uint32_t* sum_hungry, uint32_t feed_type);
		int add_attribute_value(uint32_t userid, uint32_t weight_inc, uint32_t charm, uint32_t strong,
				uint32_t feed_type);
		int get_piglet_counts(userid_t userid, uint32_t *count);
		int get_all_counts(userid_t userid, uint32_t* count);
		uint32_t get_level_piglet_limit(userid_t userid, uint32_t level);
		int get_newest_piglet(userid_t userid, uint32_t type, user_get_piglet_house_out_item* p_out);
		int get_max_index(userid_t userid, uint32_t *max_index);
		int get_revalant_train(uint32_t userid, uint32_t index, uint32_t *train_point, uint32_t *train_cnt, 
		uint32_t *weight, uint32_t *charm, uint32_t *strong, uint32_t *breed);
		int train_piglet(uint32_t userid, uint32_t index, uint32_t type, uint32_t *train_point, user_train_piglet_out *out);
		int  cal_growth_weight(uint32_t growth_rate, uint32_t feed_time, uint32_t feed_type,
			   	uint32_t growth_stage, uint32_t now, uint32_t* weight, uint32_t effect_time, 
				uint32_t lasttime, uint32_t cal_weight_time);
		int change_piglet_nick(userid_t userid, uint32_t index, char* nick);
		int check_mail_piglet_make_pair(userid_t userid, uint32_t index, uint32_t *state);
		int get_mail_piglet_make_pair(userid_t userid, uint32_t index, uint32_t* feed_time,
				uint32_t *birthday, uint32_t* lifetime, uint32_t *breed, uint32_t *birth_cnt);
		int get_piglet_dress(userid_t userid, uint32_t index, uint32_t *price,
				uint32_t *weight, uint32_t *sex, uint32_t *piglet_id, user_piglet_dress* dress,
				uint32_t effect_time, uint32_t lasttime);
		int get_weight_sex_dress(userid_t userid, uint32_t index, uint32_t *weight, uint32_t *sex,
				uint32_t *pigletid, user_piglet_dress* dress, uint32_t effect_time, uint32_t lasttime);
		int remove(userid_t userid, uint32_t index);
		int get_fancy_piglet(uint32_t userid, uint32_t index, uint32_t feed_type, user_get_piglet_house_out_item* p_out);
		int get_all_able_make_pair_mail_pig(uint32_t userid, user_get_able_make_pair_mail_piglet_out_item 
				**pp_list, uint32_t *p_count);
		int add_nutrition_value(userid_t userid, uint32_t weight_inc, uint32_t charm_inc, uint32_t strong_inc);
		int check_satisfy_process(userid_t userid, uint32_t index, uint32_t sort, uint32_t *state);
		int get_no_born_child(userid_t userid, uint32_t parent_index, uint32_t **pp_list, uint32_t *p_count);
		int get_piglets(userid_t userid, user_get_piglet_house_out_item *p_list, uint32_t* p_count, 
				std::vector<birth_info_t> &vec, std::set<uint32_t> &sets, uint32_t feed_type, uint32_t tool_cnt,
				buff_tool_t *p_item);
		int update_female_pregnent(userid_t userid, uint32_t index, uint32_t pregnent_time);
		int get_nick(userid_t userid, uint32_t index, char* nick);
		int update_inc_two(userid_t userid, uint32_t id, const char* col_1, const char* col_2,
				uint32_t value_1, uint32_t value_2);
		int update_inc_col(userid_t userid, uint32_t id, const char* col, uint32_t value);
		int lengthen_lifetime(uint32_t userid, uint32_t index, uint32_t breed);
		int update_fancy_born_state(userid_t userid, uint32_t index);
		int add_attribute_value(uint32_t userid, uint32_t index, uint32_t weight_inc, uint32_t charm_inc,
				uint32_t strong_inc, uint32_t growth_stage, uint32_t breed, uint32_t now);
		int change_attribute_value(userid_t userid, uint32_t weight_inc, uint32_t charm_inc, uint32_t strong_inc,
			   	uint32_t feed_type, uint32_t effect_time, uint32_t last_time, uint32_t now);
		int give_new_life(userid_t userid, uint32_t index);
		int check_use_more_child_tool(userid_t userid, uint32_t index, uint32_t *growth, uint32_t *growth_stage);
		int get_fbbl_attr(uint32_t userid, uint32_t index, uint32_t *birthday, uint32_t *feed_time, 
				uint32_t *breed, uint32_t *lifetime);
		int change_pregnant_state(userid_t userid, uint32_t index);
		int get_no_born_state_child(userid_t userid, uint32_t parent_index, no_born_piglet_st **pp_list, uint32_t *p_count);
		int get_piglet_survive(userid_t userid, uint32_t *count);
		int su_get_all(uint32_t userid, su_user_get_all_piglets_out_item **pp_list, uint32_t *p_count);
		int check_able_carry_piglet(userid_t userid, uint32_t index, user_piglet_carry_piglet_out *out );
		int get_outgo_count(userid_t userid, uint32_t *count_in_piglet);
		int clear_carry_flag(userid_t userid);
		int check_performance_by_hungry(userid_t userid, uint32_t *piglet_index, uint32_t *state );
		int get_fbbl_attr_itemid(uint32_t userid, uint32_t index, uint32_t *birthday, uint32_t *feed_time,
			uint32_t *breed, uint32_t *lifetime, uint32_t *pigletid, uint32_t *transform_id, uint32_t *trans_start,
			uint32_t *trans_last, user_piglet_dress *dress);
		int get_another_piglets(userid_t userid, user_get_another_pighouse_out_item** pp_list, uint32_t *p_count);
		int transfer_piglet(userid_t userid, uint32_t index, uint32_t effect_time, uint32_t lasttime, uint32_t flag);
		int put_in_primary(userid_t userid, uint32_t index, uint32_t flag);
		int get_breed_dress(userid_t userid, uint32_t index, uint32_t *breed, user_piglet_dress *dress);
		int get_secondary_piglet_cnt(userid_t userid, uint32_t *cnt_in_secondary, uint32_t flag);
		int put_on(userid_t userid, uint32_t index, user_piglet_dress *dress);
		int get_pregnant_piglet(userid_t userid, user_transfer_piglet_in_item *p_in_item, uint32_t in_count, 
				pregnant_piglet_t **p_out_item, uint32_t *out_count);
		int get_machine_piglets(uint32_t userid, uint32_t *p_count, user_get_piglet_machine_work_out_item_1 **pp_list);
		int send_to_mining(uint32_t userid, uint32_t pigletid, uint32_t need_energy, uint32_t *state);
		uint32_t get_inject_max(uint32_t breed, uint32_t generate);
		int get_info_for_inject_engergy(uint32_t userid, uint32_t index, uint32_t* sex, uint32_t* generate,
			   	uint32_t* energy, uint32_t *pour_times);
		int inject_energy(userid_t userid, uint32_t index, uint32_t *state, uint32_t dose = 0);
		int get_engergy_for_mining(uint32_t userid, uint32_t index, uint32_t* energy);
		int finish_mining(userid_t userid, uint32_t in_count, user_finish_some_map_mining_in_item *p_in_list);
		int get_some_piglet_for_work(userid_t userid, uint32_t in_count, user_piglet_melt_ore_in_item *p_in_item, 
				uint32_t **p_list, uint32_t *out_count);
		int check_piglets_whether_work(uint32_t userid, uint32_t count, user_piglet_melt_ore_in_item *p_in_item, 
				uint32_t *temp_state, uint32_t work_type, uint32_t other_flag);
		int get_special_piglets(userid_t userid, uint32_t shift, uint32_t *count);
		int check_able_inject_energy(uint32_t feed_time, uint32_t birthday, uint32_t lifetime,
			uint32_t sex, uint32_t pregnant, uint32_t breed, uint32_t pour_times, uint32_t generate, uint32_t now);
		int start_working(userid_t userid, user_piglet_melt_ore_in_item *p_in_item, uint32_t in_count, uint32_t need_energy);
		int get_some_piglet_itemid(userid_t userid, uint32_t in_count, user_get_piglet_mining_info_out_item *p_in_item, 
				piglet_item_t **pp_list, uint32_t *out_count);
		int finish_working(userid_t userid, uint32_t in_count, uint32_t *p_in_list);
};
#endif
