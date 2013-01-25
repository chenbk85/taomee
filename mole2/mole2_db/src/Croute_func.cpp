/*
 * =====================================================================================
 *
 *
 *	   Filename:  route_func.cpp
 *
 *	Description:  
 *
 *		Version:  1.0
 *		Created:  2009年3月02日 16时39分06秒 CST
 *	   Revision:  none
 *	   Compiler:  gcc
 *
 *		 Author:  jim(jim), jim@taomee.com
 *		Company:  TAOMEE
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <string.h>
#include "Croute_func.h"
#include "benchapi.h"
#include "proto.h"
#include <limits.h>
#include <string>
#include "db_error.h"
#include "Ccommon.h"
#include "util.h"
#include  <libtaomee++/utils/strings.hpp>

static inline uint32_t get_vip_level(uint32_t vipexp)
{
	int loop;
	if(!vipexp) return 0;
	uint32_t vipexps[10] = {150, 450, 900, 1800, 2700, 4500, 6300, 9000, 11700, 15300};
	for (loop = 0; loop < 6; loop ++) {
		if (vipexps[loop] >= vipexp)
			return loop + 1;
	}
	return 6;
}

extern  char * version_str;
CMD_MAP g_cmdmap[]={
	FILL_CMD_WITH_PRI_IN 	(get_args),
	FILL_CMD_WITH_PRI_IN 	(set_args),

	FILL_CMD_WITHOUT_PRI_IN	(mole2_user_user_login_ex),
	FILL_CMD_WITH_PRI_IN	(mole2_user_user_update_signature),
	FILL_CMD_WITH_PRI_IN	(mole2_user_user_update_color),
	FILL_CMD_WITH_PRI_IN	(mole2_user_user_update_professtion),
	FILL_CMD_WITH_PRI_IN	(mole2_user_user_add_xiaomee),
	FILL_CMD_WITH_PRI_IN	(mole2_user_user_add_exp),
	FILL_CMD_WITH_PRI_IN	(mole2_user_user_add_base_attr),
	FILL_CMD_WITH_PRI_IN	(mole2_user_user_add_attr_addition),
	FILL_CMD_WITHOUT_PRI_IN	(mole2_user_get_day_list),
	FILL_CMD_WITHOUT_PRI_IN (mole2_user_get_nick),
	FILL_CMD_WITH_PRI_IN	(mole2_user_pet_add),
	FILL_CMD_WITH_PRI_IN	(mole2_user_pet_get_info),
	FILL_CMD_WITHOUT_PRI_IN	(mole2_user_pet_get_list),
	FILL_CMD_WITH_PRI_IN	(mole2_user_pet_update_nick),
	FILL_CMD_WITH_PRI_IN	(mole2_user_pet_cure),
	FILL_CMD_WITH_PRI_IN	(mole2_user_pet_get_base_attr_initial),
	FILL_CMD_WITH_PRI_IN	(mole2_user_pet_get_base_attr_param),
	FILL_CMD_WITH_PRI_IN	(mole2_user_pet_add_base_attr_initial),
	FILL_CMD_WITHOUT_PRI_IN	(mole2_user_user_login),
	FILL_CMD_WITH_PRI_IN	(mole2_user_pet_add_exp),
	FILL_CMD_WITH_PRI_IN	(mole2_user_pet_add_attr_addition),
	FILL_CMD_WITH_PRI_IN	(mole2_user_pet_get_four_attr),
	FILL_CMD_WITH_PRI_IN_GE	(mole2_user_user_cure),
	FILL_CMD_WITH_PRI_IN	(mole2_user_pet_set_hp_mp),
	FILL_CMD_WITH_PRI_IN	(mole2_user_pet_update_carry_flag),
	FILL_CMD_WITH_PRI_IN	(mole2_user_pet_injury_cure),
	FILL_CMD_WITH_PRI_IN	(mole2_user_pet_get_flag),
	FILL_CMD_WITH_PRI_IN	(mole2_user_user_add_friend),
	FILL_CMD_WITH_PRI_IN	(mole2_user_user_del_friend),
	FILL_CMD_WITH_PRI_IN	(mole2_user_user_add_blackuser),
	FILL_CMD_WITH_PRI_IN	(mole2_user_user_del_blackuser),
	FILL_CMD_WITH_PRI_IN_GE	(mole2_user_user_add_offline_msg),
	FILL_CMD_WITHOUT_PRI_IN	(mole2_user_user_get_offline_msg),
	FILL_CMD_WITH_PRI_IN	(mole2_user_pet_get_attr_add),
	FILL_CMD_WITH_PRI_IN	(mole2_user_pet_add_attr_add),
	FILL_CMD_WITHOUT_PRI_IN	(mole2_user_user_get_friend_list),
	FILL_CMD_WITHOUT_PRI_IN	(mole2_user_user_get_black_list),
	FILL_CMD_WITH_PRI_IN	(mole2_user_user_check_is_friend),
	FILL_CMD_WITH_PRI_IN	(mole2_user_attire_add),
	FILL_CMD_WITHOUT_PRI_IN	(mole2_user_attire_get_list),
	FILL_CMD_WITHOUT_PRI_IN	(mole2_user_attire_get_list_ex),
	//FILL_CMD_WITH_PRI_IN_GE	(mole2_user_attire_attr_add_list),
	FILL_CMD_WITH_PRI_IN	(mole2_user_attire_get),
	FILL_CMD_WITH_PRI_IN	(mole2_user_attire_update_gridid),
	FILL_CMD_WITH_PRI_IN_GE	(mole2_user_attire_update_gridid_list),
	FILL_CMD_WITH_PRI_IN	(mole2_user_attire_del),
	FILL_CMD_WITH_PRI_IN	(user_attire_attrib_set),
	FILL_CMD_WITH_PRI_IN	(mole2_user_item_add),
	FILL_CMD_WITHOUT_PRI_IN	(mole2_user_item_get_list),
	FILL_CMD_WITH_PRI_IN_GE	(mole2_user_item_add_list),
	FILL_CMD_WITH_PRI_IN	(mole2_user_item_buy),
	FILL_CMD_WITH_PRI_IN	(mole2_user_item_ranged_get_list),
	FILL_CMD_WITH_PRI_IN	(mole2_user_user_set_hp_mp),
	FILL_CMD_WITH_PRI_IN	(mole2_user_skill_add),
	FILL_CMD_WITHOUT_PRI_IN	(mole2_user_skill_get_list),
	FILL_CMD_WITH_PRI_IN	(mole2_user_skill_add_level),
	FILL_CMD_WITH_PRI_IN	(mole2_user_skill_del),
	FILL_CMD_WITH_PRI_IN	(mole2_user_skill_buy),
	FILL_CMD_WITH_PRI_IN	(mole2_user_skill_add_exp),
	FILL_CMD_WITH_PRI_IN	(mole2_user_pet_skill_get_list),
	FILL_CMD_WITH_PRI_IN	(mole2_user_pet_skill_buy),



	FILL_CMD_WITH_PRI_IN	(mole2_user_user_set_pos),
	FILL_CMD_WITH_PRI_IN	(user_user_injury_state_set),
	FILL_CMD_WITH_PRI_IN	(user_pet_injury_state_set),
	FILL_CMD_WITHOUT_PRI_IN	(user_user_injury_state_get),
	FILL_CMD_WITH_PRI_IN	(user_pet_injury_state_get),
//	FILL_CMD_WITH_PRI_IN_GE	(user_task_reward_set),
	FILL_CMD_WITH_PRI_IN_GE	(user_attire_info_get),
	FILL_CMD_WITH_PRI_IN_GE	(user_attire_sell_set),
	FILL_CMD_WITH_PRI_IN_GE (user_create_role_set),
	FILL_CMD_WITH_PRI_IN	(user_user_levelup_set),
	FILL_CMD_WITH_PRI_IN    (user_pet_levelup_set),
	FILL_CMD_WITH_PRI_IN    (user_user_skill_up_set),
	FILL_CMD_WITH_PRI_IN	(mole2_user_task_item_add),
	FILL_CMD_WITH_PRI_IN	(mole2_user_task_item_del),
	FILL_CMD_WITHOUT_PRI_IN	(mole2_user_task_item_get_list),
	FILL_CMD_WITH_PRI_IN_GE (user_user_skill_list_levelup_set),
	FILL_CMD_WITH_PRI_IN    (user_user_flag_set),
	FILL_CMD_WITH_PRI_IN    (rand_item_info_check),
	FILL_CMD_WITH_PRI_IN    (rand_item_info_set),
	FILL_CMD_WITH_PRI_IN    (user_pet_all_get),
	FILL_CMD_WITH_PRI_IN    (user_user_medicine_cure),
	FILL_CMD_WITH_PRI_IN    (user_pet_location_set),
	FILL_CMD_WITH_PRI_IN    (user_user_note_dream_set),
	FILL_CMD_WITH_PRI_IN_GE	(user_pet_catch_set),
	FILL_CMD_WITHOUT_PRI_IN (user_pet_list_all_get),
	FILL_CMD_WITH_PRI_IN    (activation_code_check),
	FILL_CMD_WITHOUT_PRI_IN (user_map_flight_list_get),
	FILL_CMD_WITH_PRI_IN    (mole2_user_map_flight_check),
	FILL_CMD_WITH_PRI_IN    (user_map_flight_set),
	FILL_CMD_WITH_PRI_IN	(user_logout_position_set),
	FILL_CMD_WITHOUT_PRI_IN (user_handbook_list_get),
	FILL_CMD_WITH_PRI_IN    (item_handbook_count_add),
	FILL_CMD_WITH_PRI_IN    (item_handbook_state_set),
	FILL_CMD_WITH_PRI_IN    (monster_handbook_state_set),
	FILL_CMD_WITH_PRI_IN_GE (monster_handbook_count_list_add),
	FILL_CMD_WITHOUT_PRI_IN	(user_title_list_get),
	FILL_CMD_WITH_PRI_IN	(user_honor_set),
	FILL_CMD_WITH_PRI_IN	(user_pet_follow_set),

	FILL_CMD_WITH_PRI_IN    (user_map_boss_position_set),
	FILL_CMD_WITH_PRI_IN    (user_task_optdate_get),
	FILL_CMD_WITH_PRI_IN	(tash_handbook_content_set),
	FILL_CMD_WITH_PRI_IN_GE (user_attire_list_duration_set),
	FILL_CMD_WITH_PRI_IN_GE (user_attire_list_mend_set),
	FILL_CMD_WITH_PRI_IN	(user_title_delete),
	FILL_CMD_WITH_PRI_IN	(pet_in_store_list_get),
	FILL_CMD_WITH_PRI_IN	(pet_in_store_location_set),
	FILL_CMD_WITH_PRI_IN	(user_item_storage_set),
	FILL_CMD_WITHOUT_PRI_IN	(user_item_in_storage_list_get),
	FILL_CMD_WITH_PRI_IN	(user_attire_in_storage_list_get),
	FILL_CMD_WITH_PRI_IN	(user_battle_info_set),
	FILL_CMD_WITH_PRI_IN	(home_info_get),
	FILL_CMD_WITH_PRI_IN    (item_storage_add),
	FILL_CMD_WITHOUT_PRI_IN (mole2_user_other_user_info),
	FILL_CMD_WITH_PRI_IN    (user_mail_send_set),

	FILL_CMD_WITHOUT_PRI_IN (user_mail_simple_list_get),
	FILL_CMD_WITH_PRI_IN    (user_exp_info_set),
	FILL_CMD_WITH_PRI_IN    (user_mail_content_get),
	FILL_CMD_WITH_PRI_IN_GE (user_mail_list_del),
	FILL_CMD_WITH_PRI_IN    (user_mail_flag_set),
	FILL_CMD_WITH_PRI_IN    (mole2_task_set),
	FILL_CMD_WITH_PRI_IN    (user_energy_set),
	FILL_CMD_WITH_PRI_IN    (mole2_task_info_get),
	FILL_CMD_WITHOUT_PRI_IN	(mole2_task_info_list_get),
	FILL_CMD_WITH_PRI_IN_GE	(user_task_reward_set_cp),
	FILL_CMD_WITH_PRI_IN    (mole2_task_ser_buf_set),
	FILL_CMD_WITH_PRI_IN    (rand_item_present_set),
	FILL_CMD_WITH_PRI_IN    (mole2_task_delete),
	FILL_CMD_WITH_PRI_IN    (rand_item_activation_set),
	FILL_CMD_WITH_PRI_IN	(mole2_home_flag_set),
	FILL_CMD_WITH_PRI_IN    (set_winbossid),
	FILL_CMD_WITH_PRI_IN    (set_ban_flag),
	FILL_CMD_WITHOUT_PRI_IN (user_get_ban_flag),
	FILL_CMD_WITH_PRI_IN_GE (user_swap_pets),
	FILL_CMD_WITHOUT_PRI_IN (mole2_user_get_invited),
	FILL_CMD_WITHOUT_PRI_IN (mole2_user_add_invited),
	FILL_CMD_WITH_PRI_IN    (set_title_and_job_level),
	FILL_CMD_WITHOUT_PRI_IN (proto_get_expbox),
	FILL_CMD_WITH_PRI_IN    (proto_set_expbox),

	FILL_CMD_WITHOUT_PRI_IN (mole2_user_pet_get_in_store_list),
	FILL_CMD_WITH_PRI_IN    (mole2_user_pet_set),
	FILL_CMD_WITH_PRI_IN    (mole2_user_pet_delete),
	FILL_CMD_WITH_PRI_IN    (item_handbook_state_get),
	FILL_CMD_WITH_PRI_IN    (day_add_ssid_count),
	FILL_CMD_WITH_PRI_IN    (day_get_ssid_info),
	FILL_CMD_WITH_PRI_IN    (day_sub_ssid_total),
	FILL_CMD_WITH_PRI_IN    (mail_get_post_items),
	FILL_CMD_WITH_PRI_IN    (monster_handbook_range_get),
	FILL_CMD_WITH_PRI_IN    (get_item_type_counter),
	FILL_CMD_WITH_PRI_IN    (add_item_type_counter),
};

#define DEALFUN_COUNT  (int( sizeof(g_cmdmap )/sizeof(g_cmdmap[0])))

#include "./proto/mole2_db.h"
stru_cmd_item_t g_new_cmd_map[]={
#include "./proto/mole2_db_bind.h"
};
#define NEW_DEALFUN_COUNT  (int(sizeof(g_new_cmd_map )/sizeof(g_new_cmd_map[0])))



/*
 *--------------------------------------------------------------------------------------
 *	   Class:  Croute_func
 *	  Method:  Croute_func
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
Croute_func::Croute_func (mysql_interface * db): Cfunc_route_base(db),
	item_change_log(db,"MOLE2_USER" ),
	args(db),
	user(db),
	pet(db),
	attire(db, &item_change_log),
	skill(db),
	item(db,&item_change_log ),
	pet_skill(db),
	rand_item(db),
	user_note(db),
	user_map(db),
	item_handbook(db),
	monster_handbook(db),
	user_title(db),
	task_handbook(db),
	battle(db),
	home(db),
	mail(db),
	task(db),
	present(db),
	activation(db),
	invited(db),
	max(db),
	day(db),
	cli_buff(db),
	user2(db),
	vip_buff(db),
	vip_item_log(db),
	survey(db),
	relation(db),
	hero_cup(db),
	pet_fight(db),
	val(db),
	rank(db)
{
	
	this->db=db;
	this->sync_user_data_flag=config_get_intval("SYNC_USER_DATA_FLAG",0);
	this->initlist(g_cmdmap,DEALFUN_COUNT );

	//初始化命令列表
	uint32_t malloc_size=sizeof(stru_cmd_item_t )*64*1024;
	this->p_cmd_list=(stru_cmd_item_t* ) malloc (malloc_size );
	memset(this->p_cmd_list,0, malloc_size);
	this->set_cmd_list(g_new_cmd_map,NEW_DEALFUN_COUNT );

	//this->cmdmaplist.show_item();
}  /* -----  end of method Croute_func::Croute_func  (constructor)  ----- */

int Croute_func::do_sync_data(uint32_t userid, uint16_t cmdid )
{
    return SUCC;
}
//==================SU====================

int Croute_func::su_switch_kick_player(DEAL_FUNC_ARG){
    return SUCC;
}
int Croute_func::su_switch_send_msg(DEAL_FUNC_ARG){
    return SUCC;
}
int Croute_func::su_switch_mail_notify(DEAL_FUNC_ARG){
    return SUCC;
}

int Croute_func::su_mole2_get_user(DEAL_FUNC_ARG){
    su_mole2_get_user_out* p_out=P_OUT;

	stru_mole2_user_user_info_ex ainfo={};
	ret = this->user.get_all_info(userid, &ainfo);
	if(ret != SUCC) return ret;
	memcpy(p_out->nick,ainfo.userinfo.nick,sizeof(p_out->nick));
	p_out->flag = ainfo.userinfo.flag;
	p_out->color = ainfo.userinfo.color;
	p_out->regist_time = ainfo.userinfo.register_time;
	p_out->race = ainfo.userinfo.race;
	p_out->profession= ainfo.userinfo.professtion;
	p_out->honor= ainfo.userinfo.honor;
	p_out->xiaomee= ainfo.userinfo.xiaomee;
	p_out->level= ainfo.userinfo.level;
	p_out->experience= ainfo.userinfo.experience;
	p_out->physique= ainfo.userinfo.physique;
	p_out->strength= ainfo.userinfo.strength;
	p_out->endurance= ainfo.userinfo.endurance;
	p_out->quick= ainfo.userinfo.quick;
	p_out->intelligence= ainfo.userinfo.intelligence;
	p_out->attr_addition= ainfo.userinfo.attr_addition;
	p_out->hp= ainfo.userinfo.hp;
	p_out->mp= ainfo.userinfo.mp;
	p_out->injury_state= ainfo.userinfo.injury_state;
	p_out->in_front= ainfo.userinfo.in_front;
	p_out->joblevel= ainfo.userinfo.joblevel;
	p_out->max_attire= ainfo.userinfo.max_attire;
	p_out->max_medicine= ainfo.userinfo.max_medicine;
	p_out->max_stuff= ainfo.userinfo.max_stuff;
	p_out->time= ainfo.time;
	p_out->axis_x= ainfo.axis_x;
	p_out->axis_y= ainfo.axis_y;
	p_out->mapid= ainfo.mapid;
	p_out->daytime = ainfo.daytime;
	p_out->fly_mapid= ainfo.fly_mapid;
	p_out->expbox= ainfo.expbox;
	p_out->energy= ainfo.energy;
	p_out->skill_expbox= ainfo.skill_expbox;
	p_out->flag_ex= ainfo.flag_ex;
	p_out->winbossid= ainfo.winbossid;
	p_out->parent= ainfo.parent;
	p_out->sign_count=ainfo.sign_count;
	ret = this->battle.get_online_login(userid, p_out->battle);

    return ret;
}
int Croute_func::su_mole2_get_tasks(DEAL_FUNC_ARG){
    su_mole2_get_tasks_out* p_out=P_OUT;

	ret = this->task.get_tasks_doing(userid,p_out->tasks_doing);
	if(ret != SUCC) return ret;
	ret = this->task.get_tasks_done(userid,p_out->tasks_done);

    return ret;
}

int Croute_func::su_mole2_del_task(DEAL_FUNC_ARG){
    su_mole2_del_task_in* p_in=P_IN;

	ret = this->task.task_del(userid,p_in->taskid);

    return ret;
}

int Croute_func::su_mole2_add_task(DEAL_FUNC_ARG){
    su_mole2_add_task_in* p_in=P_IN;

	ret = this->task.task_add(userid,p_in->taskid);

    return ret;
}

int Croute_func::su_mole2_get_day_items(DEAL_FUNC_ARG){
    su_mole2_get_day_items_out* p_out=P_OUT;

	ret = this->day.get_limits(userid,p_out->day_items,0,0xFFFFFFFF);

    return ret;
}
int Croute_func::su_mole2_get_items(DEAL_FUNC_ARG){
    su_mole2_get_items_out* p_out=P_OUT;

	ret = this->item.get_items(userid, p_out->items);

    return ret;
}
int Croute_func::su_mole2_add_item(DEAL_FUNC_ARG){
    su_mole2_add_item_in* p_in=P_IN;

	ret = this->item.add_item(userid, p_in->itemid, p_in->add_count);
	if(ret != SUCC) return ret;
	ret = this->item.add_item_in_storage(userid, p_in->itemid, p_in->add_storage);
	if(ret != SUCC) {
		STD_ROLLBACK();
	}

    return ret;
}
int Croute_func::su_mole2_get_skills(DEAL_FUNC_ARG){
    su_mole2_get_skills_out* p_out=P_OUT;

	ret = this->skill.get_skills(userid, p_out->skills);

    return ret;
}
int Croute_func::su_mole2_add_skill(DEAL_FUNC_ARG){
    su_mole2_add_skill_in* p_in=P_IN;

	ret = this->skill.add_skill(userid, p_in->skill.skillid, p_in->skill.level);

    return ret;
}
int Croute_func::su_mole2_del_skill(DEAL_FUNC_ARG){
    su_mole2_del_skill_in* p_in=P_IN;

	ret = this->skill.del_skill(userid, p_in->skillid);

    return ret;
}
int Croute_func::su_mole2_get_pets(DEAL_FUNC_ARG){
    su_mole2_get_pets_out* p_out=P_OUT;

	uint32_t i;
	ret = this->pet.get_pets(userid, p_out->pets_in_bag, 0, 10000);
	if(ret != SUCC) return ret;
	for(i = 0; i < p_out->pets_in_bag.size(); i++) {
		this->pet_skill.get_skills(userid,p_out->pets_in_bag[i].gettime,p_out->pets_in_bag[i].skills);
		if(ret != SUCC) return ret;
	}
	ret = this->pet.get_pets(userid, p_out->pets_in_home,10000-1,20000);
	for(i = 0; i < p_out->pets_in_home.size(); i++) {
		this->pet_skill.get_skills(userid,p_out->pets_in_home[i].gettime,p_out->pets_in_home[i].skills);
		if(ret != SUCC) return ret;
	}

    return ret;
}
int Croute_func::su_mole2_add_pet(DEAL_FUNC_ARG){
    su_mole2_add_pet_in* p_in=P_IN;
	
	stru_mole2_user_pet_info pet;
	pet.pettype = p_in->pet.pettype;
	pet.race    = p_in->pet.race;
	pet.flag    = p_in->pet.flag;
	memcpy(pet.nick,p_in->pet.nick,sizeof(pet.nick));
	pet.level   = p_in->pet.level;
	pet.exp     = p_in->pet.exp;
	pet.physiqueinit = p_in->pet.physiqueinit;
	pet.strengthinit = p_in->pet.strengthinit;
	pet.enduranceinit = p_in->pet.enduranceinit;
	pet.quickinit = p_in->pet.quickinit;
	pet.iqinit = p_in->pet.iqinit;
	pet.physiqueparam = p_in->pet.physiqueparam;
	pet.strengthparam = p_in->pet.strengthparam;
	pet.enduranceparam = p_in->pet.enduranceparam;
	pet.quickparam = p_in->pet.quickparam ;
	pet.iqparam = p_in->pet.iqparam;
	pet.physique_add = p_in->pet.physique_add;
	pet.strength_add = p_in->pet.strength_add;
	pet.endurance_add = p_in->pet.endurance_add;
	pet.quick_add = p_in->pet.quick_add;
	pet.iq_add = p_in->pet.iq_add;
	pet.attr_addition = p_in->pet.attr_addition;
	pet.hp = p_in->pet.hp;
	pet.mp = p_in->pet.mp;
	pet.earth = p_in->pet.earth;
	pet.water = p_in->pet.water;
	pet.fire = p_in->pet.fire;
	pet.wind = p_in->pet.wind;
	pet.injury_state = p_in->pet.injury_state;
	pet.location = p_in->pet.location;


	ret = this->pet.pet_add(userid, &pet,&p_in->pet.gettime);
	if(ret == SUCC) {
		for(uint32_t i = 0; i < p_in->pet.skills.size(); i++) {
			this->pet_skill.add_skill(userid,p_in->pet.gettime,p_in->pet.skills[i].skillid,p_in->pet.skills[i].level);
			if(ret != SUCC) {
				STD_ROLLBACK();
				return ret;
			}
		}
	}

    return ret;
}
int Croute_func::su_mole2_set_pet(DEAL_FUNC_ARG){
    su_mole2_set_pet_in* p_in=P_IN;

	stru_mole2_user_pet_info pet;
	pet.pettype = p_in->pet.pettype;
	pet.race    = p_in->pet.race;
	pet.flag    = p_in->pet.flag;
	memcpy(pet.nick,p_in->pet.nick,sizeof(pet.nick));
	pet.level   = p_in->pet.level;
	pet.exp     = p_in->pet.exp;
	pet.physiqueinit = p_in->pet.physiqueinit;
	pet.strengthinit = p_in->pet.strengthinit;
	pet.enduranceinit = p_in->pet.enduranceinit;
	pet.quickinit = p_in->pet.quickinit;
	pet.iqinit = p_in->pet.iqinit;
	pet.physiqueparam = p_in->pet.physiqueparam;
	pet.strengthparam = p_in->pet.strengthparam;
	pet.enduranceparam = p_in->pet.enduranceparam;
	pet.quickparam = p_in->pet.quickparam ;
	pet.iqparam = p_in->pet.iqparam;
	pet.physique_add = p_in->pet.physique_add;
	pet.strength_add = p_in->pet.strength_add;
	pet.endurance_add = p_in->pet.endurance_add;
	pet.quick_add = p_in->pet.quick_add;
	pet.iq_add = p_in->pet.iq_add;
	pet.attr_addition = p_in->pet.attr_addition;
	pet.hp = p_in->pet.hp;
	pet.mp = p_in->pet.mp;
	pet.earth = p_in->pet.earth;
	pet.water = p_in->pet.water;
	pet.fire = p_in->pet.fire;
	pet.wind = p_in->pet.wind;
	pet.injury_state = p_in->pet.injury_state;
	pet.location = p_in->pet.location;

	ret = this->pet.update(userid,p_in->pet.gettime,&pet);

    return ret;
}
int Croute_func::su_mole2_del_pet(DEAL_FUNC_ARG){
    su_mole2_del_pet_in* p_in=P_IN;

	ret = this->pet.del_pet(userid, p_in->petid);

    return ret;
}
int Croute_func::su_mole2_get_friends(DEAL_FUNC_ARG){
    su_mole2_get_friends_out* p_out=P_OUT;

	stru_id_list friends;
	ret = this->user.get_friend_list(userid, &friends);
	if(ret == SUCC) {
		for(uint32_t i = 0; i < friends.count;i++) {
			p_out->friends.push_back(friends.iditem[i]);
		}
	}

    return ret;
}
int Croute_func::su_mole2_add_friend(DEAL_FUNC_ARG){
    su_mole2_add_friend_in* p_in=P_IN;

	ret = this->user.add_friend(userid, p_in->friendid);

    return ret;
}
int Croute_func::su_mole2_del_friend(DEAL_FUNC_ARG){
    su_mole2_del_friend_in* p_in=P_IN;

	ret = this->user.del_friend(userid, p_in->friendid);

    return ret;
}
int Croute_func::su_mole2_get_blacks(DEAL_FUNC_ARG){
    su_mole2_get_blacks_out* p_out=P_OUT;

	stru_id_list black;
	ret = this->user.get_black_list(userid, &black);
	if(ret == SUCC) {
		for(size_t i = 0; i < black.count;i++) {
			p_out->blacks.push_back(black.iditem[i]);
		}
	}

    return ret;
}
int Croute_func::su_mole2_add_black(DEAL_FUNC_ARG){
    su_mole2_add_black_in* p_in=P_IN;

	ret = this->user.add_blackuser(userid, p_in->black_userid);

    return ret;
}
int Croute_func::su_mole2_del_black(DEAL_FUNC_ARG){
    su_mole2_del_black_in* p_in=P_IN;

	ret = this->user.del_blackuser(userid, p_in->black_userid);

    return ret;
}
int Croute_func::su_mole2_add_mail(DEAL_FUNC_ARG){
    su_mole2_add_mail_in* p_in=P_IN;

	uint32_t count = 0;
    user_mail_send_set_in mail;
	uint32_t now = (uint32_t)time(0);

	ret = this->mail.count_get(userid, &count);
	if (ret != SUCC) return ret;
	
	if (count > MAIL_COUNT_MAX && p_in->type < 3) {
		return MAIL_COUNT_MAX_ERR;
	}

	mail.flag = 0;
	mail.type = p_in->type;
	mail.themeid = p_in->themeid;
	mail.senderid = p_in->sender_id;
	memcpy(mail.sender_nick,p_in->sender_nick,sizeof(mail.sender_nick));
	memcpy(mail.title,p_in->title,sizeof(mail.title));
	memcpy(mail.message,p_in->message,sizeof(mail.message));
	memcpy(mail.items,p_in->items,sizeof(mail.items));
	memset(mail.ids,0,sizeof(mail.ids));

	for (int i = 0; i < 50; ++i,now++) {
    	ret = this->mail.insert(userid, now, &mail);
		if(ret == SUCC)  break;
	}

    return ret;
}
int Croute_func::su_mole2_get_vip_flag(DEAL_FUNC_ARG){
    su_mole2_get_vip_flag_out* p_out=P_OUT;

	ret = this->user.get_int_value(userid,this->user.str_flag,&p_out->flag);
	p_out->flag &= 0x101;

    return ret;
}

int Croute_func::su_mole2_add_cloth(DEAL_FUNC_ARG)
{
	su_mole2_add_cloth_in *p_in=P_IN;	

	stru_mole2_user_attire_info ai;
	memset(&ai,0,sizeof(ai));
	ai.attire_id=p_in->attire_id;	
	uint32_t now=time(NULL);
	ret=this->attire.attire_add(userid,&ai,&now,0);

	return ret;
}

int Croute_func::su_mole2_add_title(DEAL_FUNC_ARG)
{
	su_mole2_add_title_in *p_in=P_IN;	

	ret=this->user_title.insert(userid,p_in->titleid,p_in->time);

	return ret;
}

int Croute_func::su_mole2_delete_title(DEAL_FUNC_ARG)
{
	su_mole2_delete_title_in *p_in=P_IN;	

	ret=this->user_title.del(userid,p_in->titleid);

	return ret;
}


int Croute_func::su_mole2_get_equips(DEAL_FUNC_ARG)
{
	su_mole2_get_equips_out* p_out=P_OUT;

	ret = this->attire.get_cloths(userid, p_out->cloths, 0, 0xFFFFFFFF);

	return ret;
}

int Croute_func::su_mole2_get_expbox(DEAL_FUNC_ARG){
    su_mole2_get_expbox_out* p_out=P_OUT;

	ret = this->user.exp_box_get(userid,&p_out->expbox);

    return ret;
}
int Croute_func::su_mole2_get_ban_flag(DEAL_FUNC_ARG){
    su_mole2_get_ban_flag_out* p_out=P_OUT;

	ret = this->user.check_ban_flag(userid);
	switch(ret) {
		default:
			p_out->flag = 0;
			break;
		case BAN_FOREVER_ERR:
			p_out->flag = BAN_FOREVER;
			break;
		case BAN_14DAYS_ERR:
			p_out->flag = BAN_14DAYS;
			break;
		case BAN_7DAYS_ERR:
			p_out->flag = BAN_7DAYS;
			break;
		case BAN_24HOURS_ERR:
			p_out->flag = BAN_24HOURS;
			break;
	}

    return SUCC;
}
int Croute_func::su_mole2_set_ban_flag(DEAL_FUNC_ARG){
    su_mole2_set_ban_flag_in* p_in=P_IN;

	ret = this->user.set_ban_flag(userid, p_in->flag);

    return ret;
}
int Croute_func::su_mole2_set_expbox(DEAL_FUNC_ARG){
    su_mole2_set_expbox_in* p_in=P_IN;

	ret = this->user.exp_box_set(userid,p_in->expbox);

    return ret;
}
int Croute_func::su_mole2_set_nick(DEAL_FUNC_ARG){
    su_mole2_set_nick_in* p_in=P_IN;

	mole2_user_user_update_nick_in nick_in;
	memcpy(nick_in.nick,p_in->nick,sizeof(nick_in.nick));
	ret = this->user.update_nick(userid, &nick_in);

    return ret;
}
int Croute_func::su_mole2_set_profession(DEAL_FUNC_ARG){
    su_mole2_set_profession_in* p_in=P_IN;

	ret = this->user.set_int_value(userid, this->user.str_profess, p_in->profession);

    return ret;
}
int Croute_func::su_mole2_add_xiaomee(DEAL_FUNC_ARG){
    su_mole2_add_xiaomee_in* p_in=P_IN;

	ret = this->user.add_xiaomee(userid, p_in->xiaomee);

    return ret;
}
int Croute_func::su_mole2_delete_cloths(DEAL_FUNC_ARG)
{
	su_mole2_delete_cloths_in *p_in=P_IN;
	
	ret= this->attire.del_attire(userid,p_in->gettime,&p_in->clothid,false);
	
	return ret;
}
int Croute_func::su_mole2_set_base_attr(DEAL_FUNC_ARG){
    su_mole2_set_base_attr_in* p_in=P_IN;
	
	mole2_user_user_add_base_attr_in  attr;
	attr.physique      = p_in->physique;
	attr.strength      = p_in->strength;
	attr.endurance     = p_in->endurance;
	attr.quick         = p_in->quick;
	attr.intelligence  = p_in->intelligence;
	attr.attr_addition = p_in->addition;
	ret = this->user.set_base_attr(userid,&attr);

    return ret;
}
int Croute_func::su_mole2_set_position(DEAL_FUNC_ARG){
    su_mole2_set_position_in* p_in=P_IN;

	ret = this->user.set_pos(userid,p_in->position);

    return ret;
}

int Croute_func::su_mole2_set_field_value(DEAL_FUNC_ARG){
    su_mole2_set_field_value_in* p_in=P_IN;
	if(p_in->opt_type==1) {
		ret = this->user.set_field_value(userid,p_in);
	} else if (p_in->opt_type==2)  {
		ret = this->user2.set_field_value(userid,p_in);
		if (ret){
			mole2_user_set_vip_info_in item;
			this->user2.set_vip_info(userid, &item);

			ret = this->user2.set_field_value(userid,p_in);
		}
	} else if (p_in->opt_type==3)  {
		ret = this->pet.set_field_value(userid,p_in);
	} else if(p_in->opt_type==4){
		ret = this->attire.set_field_value(userid,p_in);
	}else if(p_in->opt_type==5){
		ret = this->day.set_field_value(userid,p_in);
	}else if(p_in->opt_type==6){
		ret = this->task.set_field_value(userid,p_in);
	}

    return ret;
}

int Croute_func::su_mole2_get_vip_info(DEAL_FUNC_ARG){
    su_mole2_get_vip_info_out* p_out=P_OUT;

	uint32_t vip_flag = 0;
	mole2_user_vip_info vipinfo = {0};
	this->user.get_int_value(userid,this->user.str_flag,&vip_flag);
	if(vip_flag & 0x101) {
		ret = this->user2.get_vip_info(userid,&vipinfo);
		if(ret == SUCC) {
			uint32_t day_value = 0;
			vipinfo.vip_ex_val += vipinfo.vip_base_val;
			if(time(NULL) > vipinfo.vip_end_time) {
				if((vip_flag & 0x01)) {
					DEBUG_LOG("error:vip end[%u]",userid);
					//vip_flag |= 0x100;
					//if(!vipinfo.vip_auto) vip_flag &=~ 0x01;
					//this->user.set_int_value(userid,this->user.str_flag,vip_flag);
				}

				day_value = (time(NULL) - vipinfo.vip_end_time) / (3600 * 24);
				day_value *= 5;
				if(vipinfo.vip_ex_val > day_value)
					vipinfo.vip_ex_val -= day_value;
				else
					vipinfo.vip_ex_val = 0;
			} else {
				day_value = (time(NULL) - vipinfo.vip_begin_time) / (3600 * 24);
				day_value *= 5;
				vipinfo.vip_ex_val += day_value;
			}
			if((vip_flag & 0x01) && vipinfo.vip_ex_val == 0) vipinfo.vip_ex_val = 1;
		} else if(ret == USER_ID_NOFIND_ERR) {
			//if(vip_flag & 0x101) {
				//vip_flag &=~ 0x101;
				//this->user.set_int_value(userid,this->user.str_flag,vip_flag);
			//}
		}
	}

	p_out->vip_value = vipinfo.vip_ex_val;
	p_out->vip_level = get_vip_level(p_out->vip_value);
	p_out->vip_auto_charge = vipinfo.vip_auto;
	p_out->vip_base_value = vipinfo.vip_base_val;
	p_out->vip_end_time = vipinfo.vip_end_time;
	p_out->vip_begin_time = vipinfo.vip_begin_time;

    return SUCC;
}

int Croute_func::su_mole2_set_task(DEAL_FUNC_ARG){
    su_mole2_set_task_in* p_in=P_IN;

	ret = this->task.set_state(userid,p_in->taskid,p_in->state);

    return ret;
}

//================END SU==================

int Croute_func::buy_item_in_mall(DEAL_FUNC_ARG){
    buy_item_in_mall_in* p_in=P_IN;

	ret = SUCC;
	for(uint32_t i = 0; i < p_in->items.size();i++) {
		if(p_in->items[i].count > 0x7FFFFFFF) {
			ret = VALUE_MORE_THAN_LIMIT_ERR;
			STD_ROLLBACK();
			break;
		}
		ret = this->item.add_item(userid,p_in->items[i].itemid,(int)p_in->items[i].count,NULL,true);
		if(ret != SUCC) {
			STD_ROLLBACK();
			break;
		}
	}

    return ret;
}

int Croute_func::buy_cloth_in_mall(DEAL_FUNC_ARG){
    buy_cloth_in_mall_in* p_in=P_IN;

	uint32_t gettime = 0;
	stru_mole2_user_attire_info attire_info = {};

	for(uint32_t i = 0; i < p_in->cloths.size(); i++) {
		attire_info.attire_id = p_in->cloths[i].clothid;
		attire_info.gridid    = p_in->cloths[i].gridid;
		attire_info.level     = p_in->cloths[i].level;
		attire_info.mduration = p_in->cloths[i].mduration;
		attire_info.duration  = p_in->cloths[i].duration;
		attire_info.hpmax     = p_in->cloths[i].hpmax;
		attire_info.mpmax     = p_in->cloths[i].mpmax;
		attire_info.atk       = p_in->cloths[i].atk;
		attire_info.matk      = p_in->cloths[i].matk;
		attire_info.def       = p_in->cloths[i].defense;
		attire_info.mdef      = p_in->cloths[i].mdef;
		attire_info.speed     = p_in->cloths[i].speed;
		attire_info.spirit    = p_in->cloths[i].spirit;
		attire_info.resume    = p_in->cloths[i].resume;
		attire_info.hit       = p_in->cloths[i].hit;
		attire_info.dodge     = p_in->cloths[i].dodge;
		attire_info.crit      = p_in->cloths[i].crit;
		attire_info.fightback = p_in->cloths[i].fightback;
		attire_info.rpoison   = p_in->cloths[i].rpoison;
		attire_info.rlithification = p_in->cloths[i].rlithification;
		attire_info.rlethargy = p_in->cloths[i].rlethargy;
		attire_info.rinebriation = p_in->cloths[i].rinebriation;
		attire_info.rconfusion = p_in->cloths[i].rconfusion;
		attire_info.roblivion = p_in->cloths[i].roblivion;
		attire_info.quality   = p_in->cloths[i].quality;
		attire_info.validday  = p_in->cloths[i].validday;
		attire_info.crystal_attr = p_in->cloths[i].crystal_attr;
		attire_info.bless_type   = p_in->cloths[i].bless_type;

		ret = this->attire.attire_add(userid,&attire_info,&gettime,true);
		if(ret != SUCC) {
			STD_ROLLBACK();
			break;
		}
	}

    return ret;
}

int Croute_func::mall_chk_cloths_limits(DEAL_FUNC_ARG){
    mall_chk_cloths_limits_in* p_in=P_IN;

	for(uint32_t i =0; i < p_in->cloths.size(); i++) {
		uint32_t count = 0;
		ret = this->attire.get_count_by_type(userid,p_in->cloths[i].clothid,count);
		if(ret != SUCC) return ret;
		if(p_in->cloths[i].count + count > p_in->cloths[i].limit) {
			return VALUE_MORE_THAN_LIMIT_ERR;
		}
	}

    return SUCC;
}

//================END mall==================

int Croute_func::get_server_info(DEAL_FUN_ARG)
{        
    get_server_info_out out={};
    ret=SUCC; 


	std::vector<std::string> file_name_list;
 	file_name_list.push_back("./libpubser.so" );
 	file_name_list.push_back("./pubser" );
 	strncpy(out.msg,get_version_str(version_str,file_name_list ).c_str(),sizeof (out.msg)  );
    out.msg[sizeof (out.msg)  - 1]= '\0';
    STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::get_args(DEAL_FUN_ARG)
{        
    get_args_in *p_in = PRI_IN_POS;
	get_args_out_header out_header = {};
	get_args_out_item *p_items;
	ret = this->args.select(RECVBUF_USERID,p_in->min,p_in->max,&p_items,&out_header.count);
    STD_RETURN_WITH_STRUCT_LIST(ret,out_header, p_items, out_header.count);
}

int Croute_func::set_args(DEAL_FUN_ARG)
{        
    set_args_in *p_in = PRI_IN_POS;
	ret = this->args.update(RECVBUF_USERID,p_in->key,p_in->value);
	STD_RETURN(ret);
}

int Croute_func::mole2_login(DEAL_FUNC_ARG)
{
	mole2_login_out *p_out=P_OUT;
	stru_mole2_user_user_info out={};
	ret=this->user.get_all_info(userid,&out);
	memcpy(p_out->nick,out.nick,NICK_LEN);
	memcpy(p_out->signature,out.signature,SIGN_LEN);
	p_out->color=out.color;
	p_out->endurance=out.endurance;
	p_out->experience=out.experience;
	p_out->flag=out.flag;
	p_out->honor=out.honor;
	p_out->hp=out.hp;
	p_out->in_front=out.in_front;
	p_out->injury_state=out.injury_state;
	p_out->intelligence=out.intelligence;
	p_out->joblevel=out.joblevel;
	p_out->level=out.level;
	p_out->max_attire=out.max_attire;
	p_out->max_medicine=out.max_medicine;
	p_out->max_stuff=out.max_stuff;
	p_out->mp=out.mp;
	p_out->physique=out.physique;
    p_out->professtion=out.professtion;
	p_out->quick=out.quick;
	p_out->race=out.race;
	p_out->register_time=out.register_time;
	p_out->strength=out.strength;
	p_out->xiaomee=out.xiaomee;
	p_out->attr_addition=out.attr_addition;
	return ret;
}
int Croute_func::mole2_user_user_login(DEAL_FUN_ARG)
{
	mole2_user_user_login_out_header out_header = {};
	mole2_user_user_login_out_item* p_idlist;
	
	ret=this->user.check_ban_flag(RECVBUF_USERID);
	if(ret == SUCC) {
		ret=this->user.get_login_info(RECVBUF_USERID, &out_header, &p_idlist);
		if(ret == SUCC) {
			this->user.get_int_value(RECVBUF_USERID,this->user.str_lastonline,&out_header.lastonline);
		}
	}

	STD_RETURN_WITH_STRUCT_LIST(ret,out_header, p_idlist, out_header.friend_cnt + out_header.black_cnt);
}


//恢复 1615
int Croute_func::get_item_type_counter(DEAL_FUN_ARG)
{
        get_item_type_counter_in *p_in = PRI_IN_POS;
        get_item_type_counter_out out = {};

        uint32_t day = 0;
        out.type = p_in->type;
        out.itemid = p_in->itemid;
        ret = this->max.get(RECVBUF_USERID,p_in->type,p_in->itemid,&out.count,&day);
        if(ret != SUCC) {
            out.count = 0;
        } else {
            uint32_t date = today();
            switch((p_in->type >> 28) & 0x7) {
                default:// day
                    if(day != date) {
                        out.count = 0;
                    }
                    break;
                case 3:
                    if(day + 7 * 24 * 3600 >= date) {
                        out.count = 0;
                    }
                    break;
                case 4: // month
                    if(day + 30 * 24 * 3600 >= date) {
                        out.count = 0;
                    }
                    break;
                case 5: // year
                    if(day + 365 * 24 * 3600 >= date) {
                        out.count = 0;
                    }
                    break;
                case 6://forever
                case 7://once
                    break;
            }
        }

        STD_RETURN_WITH_STRUCT(SUCC, out);
}

int Croute_func::add_item_type_counter(DEAL_FUN_ARG)
{
        add_item_type_counter_in *p_in = PRI_IN_POS;
        add_item_type_counter_out out = {};

        out.type = p_in->type;
        out.itemid = p_in->itemid;
        if(IS_TYPE_MAX_2(p_in->type)) {
            ret = this->max.add(RECVBUF_USERID,p_in->type,p_in->itemid,p_in->count,&out.count);
        } else {
            ret = VALUE_OUT_OF_RANGE_ERR;
        }

        STD_RETURN_WITH_STRUCT(ret, out);
}


int Croute_func::mole2_user_user_update_nick(DEAL_FUNC_ARG)
{
	mole2_user_user_update_nick_in *p_in=P_IN; 
	mole2_user_user_update_nick_out *p_out =P_OUT;
	ret=this->user.update_nick(userid, p_in);
	if (ret == SUCC)
		memcpy (p_out->nick, p_in->nick, NICK_LEN);
	return ret;
}

int Croute_func::mole2_user_user_update_signature(DEAL_FUN_ARG)
{
	mole2_user_user_update_signature_in *p_in=PRI_IN_POS; 
	mole2_user_user_update_signature_out out = {};
	ret=this->user.update_signature(RECVBUF_USERID, p_in);
	if (ret == SUCC)
		memcpy (out.signature, p_in->signature, SIGN_LEN);
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_user_user_update_color(DEAL_FUN_ARG)
{
	mole2_user_user_update_color_in *p_in=PRI_IN_POS; 
	mole2_user_user_update_color_out out = {};
	ret=this->user.update_color(RECVBUF_USERID, p_in);
	if (ret == SUCC)
		out.color = p_in->color;
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_user_user_update_professtion(DEAL_FUN_ARG)
{
	mole2_user_user_update_professtion_in *p_in=PRI_IN_POS; 
	mole2_user_user_update_professtion_out out = {};
	ret=this->user.update_professtion(RECVBUF_USERID, p_in);
	if (ret == SUCC)
		out.professtion = p_in->professtion;	
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_user_user_add_xiaomee(DEAL_FUN_ARG)
{
	mole2_user_user_add_xiaomee_in *p_in=PRI_IN_POS; 
	mole2_user_user_add_xiaomee_out out = {};
	ret=this->user.add_xiaomee(RECVBUF_USERID, p_in->xiaomee, &out);
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_user_user_add_exp(DEAL_FUN_ARG)
{
	mole2_user_user_add_exp_in *p_in=PRI_IN_POS; 
	mole2_user_user_add_exp_out out = {};
	ret=this->user.add_exp(RECVBUF_USERID, p_in->experience, &out);
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_user_user_add_base_attr(DEAL_FUN_ARG)
{
	mole2_user_user_add_base_attr_in *p_in=PRI_IN_POS; 
	mole2_user_user_add_base_attr_out out = {};
	
	
	ret = this->user.set_base_attr(RECVBUF_USERID, p_in);

	memcpy(&out, p_in, sizeof(out));
	
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_user_user_add_attr_addition(DEAL_FUN_ARG)
{
	mole2_user_user_add_attr_addition_in *p_in=PRI_IN_POS;
   	mole2_user_user_add_attr_addition_out out = {};	
	ret=this->user.add_attr_addition(RECVBUF_USERID, p_in, &out);
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_user_get_day_list(DEAL_FUN_ARG)
{
	char out[PROTO_MAX_SIZE]={0};

	mole2_user_get_day_list_out_header *out_header=(mole2_user_get_day_list_out_header*)out;
	mole2_user_get_day_list_out_item *p_item_day = NULL;

	int bytes = 0;
	ret=this->day.get_list(RECVBUF_USERID,&p_item_day,&out_header->count);
	if(ret == SUCC) {
		bytes = sizeof(mole2_user_get_day_list_out_header);
		memcpy(out + bytes, p_item_day, out_header->count * sizeof(mole2_user_get_day_list_out_item));
		bytes += out_header->count * sizeof(mole2_user_get_day_list_out_item);
	}
	free(p_item_day);

	STD_RETURN_WITH_BUF(ret, out, bytes);
}

int Croute_func::mole2_user_user_set_hp_mp(DEAL_FUN_ARG)
{
	mole2_user_user_set_hp_mp_in *p_in=PRI_IN_POS; 
	mole2_user_user_set_hp_mp_out out={};
	mole2_user_user_add_hp_mp_out temp={};

	ret = this->user.get_hp_mp(RECVBUF_USERID,&temp);
	if(ret == SUCC) {
		out  = *p_in;
		if(temp.hp + p_in->hp < 1) p_in->hp = 1-temp.hp; 
		if(temp.mp + p_in->mp < 0) p_in->mp = -temp.mp;
		ret=this->user.add_hp_mp(RECVBUF_USERID, p_in->hp, p_in->mp, p_in->injury_state);
	}

	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_pet_add(DEAL_FUN_ARG)
{
	mole2_user_pet_add_in *p_in=PRI_IN_POS; 
	mole2_user_pet_add_out out={};
	ret=this->pet.pet_add(RECVBUF_USERID, p_in, &out.gettime);
	memcpy(&out.pet_info, p_in, sizeof(*p_in));
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_user_pet_get_info(DEAL_FUN_ARG)
{
	mole2_user_pet_get_info_in *p_in=PRI_IN_POS;
	mole2_user_pet_get_info_out out={};
	ret = this->pet.get_pet_info(RECVBUF_USERID, p_in->petid, &out);
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_pet_get_list(DEAL_FUN_ARG)
{
	mole2_user_pet_get_list_out_header  out_header;
	mole2_user_pet_get_list_out_item *p_out_item;
	ret=this->pet.get_pet_list_on_body(RECVBUF_USERID, &p_out_item, &(out_header.count));
	STD_RETURN_WITH_STRUCT_LIST(ret,out_header,p_out_item,out_header.count);
}

int Croute_func::mole2_user_pet_update_nick(DEAL_FUN_ARG)
{
	mole2_user_pet_update_nick_in *p_in=PRI_IN_POS; 
	mole2_user_pet_update_nick_out out={};
	ret=this->pet.update_nick(RECVBUF_USERID, p_in, &out);
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_pet_get_base_attr_initial(DEAL_FUN_ARG)
{
	mole2_user_pet_get_base_attr_initial_in *p_in=PRI_IN_POS; 
	mole2_user_pet_get_base_attr_initial_out out={};
	ret=this->pet.get_base_attr_initial(RECVBUF_USERID, p_in, &out);
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_pet_get_base_attr_param(DEAL_FUN_ARG)
{
	mole2_user_pet_get_base_attr_param_in *p_in=PRI_IN_POS; 
	mole2_user_pet_get_base_attr_param_out out={};
	ret=this->pet.get_base_attr_param(RECVBUF_USERID, p_in, &out);
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_pet_add_exp(DEAL_FUN_ARG)
{
	mole2_user_pet_add_exp_in *p_in=PRI_IN_POS; 
	mole2_user_pet_add_exp_out out={};
	ret=this->pet.add_exp(RECVBUF_USERID, p_in, &out);
	STD_RETURN_WITH_STRUCT(ret,out);
}


int Croute_func::mole2_user_pet_add_attr_addition(DEAL_FUN_ARG)
{
	mole2_user_pet_add_attr_addition_in *p_in = PRI_IN_POS; 
	mole2_user_pet_add_attr_addition_out out = {};
	DEBUG_LOG("%u %u %u %u %u %u",p_in->petid,p_in->physique,p_in->endurance,p_in->iq,p_in->strength,p_in->attr_addition);
	ret = this->pet.set_attr_addition(RECVBUF_USERID, p_in);
	memcpy(&out, p_in, sizeof(*p_in));
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_pet_get_four_attr(DEAL_FUN_ARG)
{
	mole2_user_pet_get_four_attr_in *p_in=PRI_IN_POS; 
	mole2_user_pet_get_four_attr_out out={};
	ret=this->pet.get_four_attr(RECVBUF_USERID, p_in, &out);
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_pet_update_carry_flag(DEAL_FUN_ARG)
{
	mole2_user_pet_update_carry_flag_in *p_in=PRI_IN_POS; 
	mole2_user_pet_update_carry_flag_out out={};
	ret=this->pet.update_carry_flag(RECVBUF_USERID, p_in, &out);
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_pet_set_hp_mp(DEAL_FUN_ARG)
{
	mole2_user_pet_set_hp_mp_in *p_in = PRI_IN_POS; 
	mole2_user_pet_set_hp_mp_out out={};
	mole2_user_pet_add_hp_mp_out temp={};
	
	ret = this->pet.get_hp_mp(RECVBUF_USERID, p_in->petid, &temp);
	if(ret == SUCC) {
		if(temp.hp + p_in->hp < 1) p_in->hp = 1-temp.hp; 
		if(temp.mp + p_in->mp < 0) p_in->mp = -temp.mp;
		ret = this->pet.add_hp_mp(RECVBUF_USERID, p_in->petid, p_in->hp, p_in->mp, p_in->injury_state);
		if(ret == SUCC) {
			out.petid = p_in->petid;
			out.hp = temp.hp + p_in->hp;
			out.mp = temp.mp + p_in->mp;
			out.injury_state = p_in->injury_state;
		}
	}
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_user_create_role(DEAL_FUNC_ARG)
{
	mole2_user_user_create_role_in *p_in=P_IN; 
	ret=this->user.create_role(userid, p_in);
	if(ret == SUCC) {
		uint32_t vip_end_time = 0;
		ret = this->user2.get_int_value(userid,"vip_end_time",&vip_end_time);
		if(ret == SUCC) {
			if(vip_end_time > time(NULL))
				this->user.set_int_value(userid,this->user.str_flag,0x0041);
			else
				this->user.set_int_value(userid,this->user.str_flag,0x0140);
		}
		ret = SUCC;
	}
	return ret;
}

int Croute_func::mole2_user_pet_add_base_attr_initial(DEAL_FUN_ARG)
{
	mole2_user_pet_add_base_attr_initial_in *p_in=PRI_IN_POS; 
	mole2_user_pet_get_base_attr_initial_out out={};
	ret=this->pet.add_base_attr_initial(RECVBUF_USERID, p_in, &out);
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_pet_get_flag(DEAL_FUN_ARG)
{
	mole2_user_pet_get_flag_in *p_in=PRI_IN_POS; 
	mole2_user_pet_get_flag_out out={};
	ret=this->pet.get_flag(RECVBUF_USERID, p_in->petid, &out);
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_user_add_friend(DEAL_FUN_ARG)
{
	mole2_user_user_add_friend_in *p_in=PRI_IN_POS; 
	ret=this->user.add_friend(RECVBUF_USERID, p_in->userid);
	STD_RETURN(ret);
}

int Croute_func::mole2_user_user_del_friend(DEAL_FUN_ARG)
{
	mole2_user_user_del_friend_in *p_in=PRI_IN_POS; 
	ret=this->user.del_friend(RECVBUF_USERID , p_in->userid );
    if (ret==LIST_ID_NOFIND){
        return  MOLE2_FRIENDID_NOFIND_ERR;
    }

	STD_RETURN(ret);
}

int Croute_func::mole2_user_user_add_blackuser(DEAL_FUN_ARG)
{
	mole2_user_user_add_blackuser_in *p_in=PRI_IN_POS; 
	ret=this->user.add_blackuser(RECVBUF_USERID, p_in->userid);
	STD_RETURN(ret);
}

int Croute_func::mole2_user_user_del_blackuser(DEAL_FUN_ARG)
{
	mole2_user_user_del_blackuser_in *p_in=PRI_IN_POS; 
	ret=this->user.del_blackuser(RECVBUF_USERID, p_in->userid);
	STD_RETURN(ret);
}

int Croute_func::mole2_user_user_add_offline_msg(DEAL_FUN_ARG)
{
	mole2_user_user_add_offline_msg_in_header *p_in=PRI_IN_POS;
	
    CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->msglen);

	mole2_user_user_get_offline_msg_out msglist = {0,0,{0}};

	ret=this->user.get_offline_msg(RECVBUF_USERID, &msglist);
	if (ret != SUCC) return ret;

	if (p_in->msglen + sizeof(p_in->msglen) > MSG_LIST_BUF_SIZE || 
		msglist.msglen + p_in->msglen + sizeof(p_in->msglen) > MSG_LIST_BUF_SIZE)  {
		return  MOLE2_OFFLINE_MSG_MAX_ERR;
	}

	memcpy(msglist.msg + msglist.msglen, (char*)p_in, p_in->msglen + sizeof(p_in->msglen));
	msglist.count++;
	msglist.msglen += p_in->msglen + sizeof(p_in->msglen);
	ret = this->user.set_offline_msg(RECVBUF_USERID,&msglist);

	STD_RETURN(ret);
}

int Croute_func::mole2_user_user_get_offline_msg(DEAL_FUN_ARG)
{
	mole2_user_user_get_offline_msg_out out={0,0,{0}};
	mole2_user_user_get_offline_msg_out reset={0,0,{0}};
	ret=this->user.get_offline_msg(RECVBUF_USERID, &out);
	if(ret == SUCC) {
		ret = this->user.set_offline_msg(RECVBUF_USERID, &reset);
	}
	STD_RETURN_WITH_BUF(ret, (char*)&out, (sizeof(out) - MSG_LIST_BUF_SIZE) + out.msglen);
}

int Croute_func::mole2_user_user_get_friend_list(DEAL_FUN_ARG)
{
	mole2_user_user_get_friend_list_out out={};
	ret=this->user.get_friend_list(RECVBUF_USERID, &out);
	if (ret!=SUCC) return ret;

    if (out.count > ID_LIST_MAX) out.count = ID_LIST_MAX;

    STD_RETURN_WITH_BUF(ret,(char*) &out, sizeof(out)-(ID_LIST_MAX-out.count) * sizeof(out.iditem[0]));
}

int Croute_func::mole2_user_user_get_black_list(DEAL_FUN_ARG)
{
	mole2_user_user_get_black_list_out out={};
	ret=this->user.get_black_list(RECVBUF_USERID, &out);
	if (ret!=SUCC) return ret;

    if (out.count > ID_LIST_MAX) out.count = ID_LIST_MAX;

    STD_RETURN_WITH_BUF(ret,(char*) &out,
            sizeof(out)-(ID_LIST_MAX-out.count)*sizeof(out.iditem[0]));
}

int Croute_func::mole2_user_user_check_is_friend(DEAL_FUN_ARG)
{
	mole2_user_user_check_is_friend_in *p_in=PRI_IN_POS; 
	mole2_user_user_check_is_friend_out out={};
	ret=this->user.check_is_friend(RECVBUF_USERID, p_in, &out);
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_pet_get_attr_add(DEAL_FUN_ARG)
{
	mole2_user_pet_get_attr_add_in *p_in=PRI_IN_POS; 
	mole2_user_pet_get_attr_add_out out={};
	ret=this->pet.get_attr_add(RECVBUF_USERID, p_in, &out);
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_pet_add_attr_add(DEAL_FUN_ARG)
{
	mole2_user_pet_add_attr_add_in *p_in=PRI_IN_POS; 
	mole2_user_pet_add_attr_add_out out={};
//	ret=this->pet.add_attr_add(RECVBUF_USERID, p_in, &out);
	ret = 0;
	STD_RETURN_WITH_STRUCT(ret,out);

}

int Croute_func::mole2_user_attire_add(DEAL_FUN_ARG)
{
	mole2_user_attire_add_in *p_in = PRI_IN_POS;
	mole2_user_attire_add_out out = {};

	uint32_t gettime;
	ret = this->attire.attire_add(RECVBUF_USERID, &p_in->attire_info, &gettime);
	
	if (ret == SUCC && p_in->xiaomee != 0) {
	ret = this->user.add_xiaomee(RECVBUF_USERID, p_in->xiaomee);
	if (ret != SUCC) {
		STD_ROLLBACK();
		return ret;
	}
	}
	out.attire_info_ex.gettime = gettime;
	memcpy(&out.attire_info_ex.attire_info, &p_in->attire_info, sizeof(stru_mole2_user_attire_info));
	ret = this->user.xiaomee_get(RECVBUF_USERID, &out.xiaomee_all);
	out.xiaomee_change = p_in->xiaomee;
	
	STD_RETURN_WITH_STRUCT(ret, out);
}



int Croute_func::mole2_user_attire_get_list(DEAL_FUN_ARG)
{
	mole2_user_attire_get_list_out_header  out_header;
	mole2_user_attire_get_list_out_item *p_out_item;
	ret=this->attire.get_attire_list(RECVBUF_USERID, &p_out_item, &(out_header.count));
	STD_RETURN_WITH_STRUCT_LIST(ret,out_header,p_out_item,out_header.count);
}

int Croute_func::mole2_user_attire_get(DEAL_FUN_ARG)
{
	mole2_user_attire_get_in *p_in=PRI_IN_POS; 
	mole2_user_attire_get_out out={};
	ret=this->attire.get_attire(RECVBUF_USERID, p_in->gettime, &out);
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_attire_update_gridid(DEAL_FUN_ARG)
{
	mole2_user_attire_update_gridid_in *p_in=PRI_IN_POS; 
	mole2_user_attire_update_gridid_out out={};
	ret=this->attire.update_gridid(RECVBUF_USERID, p_in->gettime, p_in->gridid);
	if (ret != SUCC) {
		return ret;
	}
	ret = this->attire.get_attire(RECVBUF_USERID, p_in->gettime, &out);
	
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_user_attire_del(DEAL_FUN_ARG)
{
	mole2_user_attire_del_in *p_in=PRI_IN_POS;
	mole2_user_attire_del_out out = {};
	uint32_t attire_id;
	ret=this->attire.del_attire(RECVBUF_USERID, p_in->gettime ,&attire_id );
	out.gettime = p_in->gettime;
	if(ret==SUCC)
	{

		//记录日志
		stru_game_change_add log_item;
		log_item.userid= RECVBUF_USERID ; 
		log_item.opt_type=1001 ;//修改物品
		log_item.ex_v1= attire_id;
		log_item.ex_v2= -1;
		game_change_add(log_item );
	}


	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_user_attire_get_list_ex(DEAL_FUN_ARG)
{
	mole2_user_attire_get_list_ex_out_header  out_header;
	mole2_user_attire_get_list_ex_out_item *p_out_item;
	ret=this->attire.get_attire_list_ex(RECVBUF_USERID, &p_out_item, &(out_header.count));
	STD_RETURN_WITH_STRUCT_LIST(ret,out_header,p_out_item,out_header.count);
}

/*
int Croute_func::mole2_user_attire_attr_add_list(DEAL_FUN_ARG)
{
	
	mole2_user_attire_attr_add_list_in_header *p_in = PRI_IN_POS;
	CHECK_PRI_IN_LEN_WITHOUT_HEADER((p_in->count)*sizeof(mole2_user_attire_attr_add_list_in_item));
	mole2_user_attire_attr_add_list_in_item * p_in_item=
		(mole2_user_attire_attr_add_list_in_item*)(RCVBUF_PRIVATE_POS+sizeof(*p_in)) ;
	mole2_user_attire_attr_add_list_out_header  out_header;
	mole2_user_attire_attr_add_list_out_item *p_out_item;
	
	if((p_out_item = (mole2_user_attire_attr_add_list_out_item*)malloc(
				   sizeof(mole2_user_attire_attr_add_list_out_item) * p_in->count)) ==NULL)	
		return SYS_ERR;
	memset(p_out_item, 0, sizeof(mole2_user_attire_attr_add_list_out_item) * p_in->count);	
	ret=this->attire.attr_add_list(RECVBUF_USERID, p_in_item, p_in->count, p_out_item, &(out_header.count));
	
	
	STD_RETURN_WITH_STRUCT_LIST(ret,out_header,p_out_item,out_header.count);
}
*/

int Croute_func::mole2_user_attire_update_gridid_list(DEAL_FUN_ARG)
{
	mole2_user_attire_update_gridid_list_in_header *p_in=PRI_IN_POS;
	CHECK_PRI_IN_LEN_WITHOUT_HEADER((p_in->count)*sizeof(mole2_user_attire_update_gridid_list_in_item));
	mole2_user_attire_update_gridid_list_in_item * p_in_item=
		(mole2_user_attire_update_gridid_list_in_item*)(RCVBUF_PRIVATE_POS+sizeof(*p_in)) ;
	
	char out[PROTO_MAX_SIZE] = {};
	mole2_user_attire_update_gridid_list_out_header  *p_out_header = 
		(mole2_user_attire_update_gridid_list_out_header*)out;
	char* p_offset = out + sizeof(*p_out_header);
	int cpy_len = 0;
	mole2_user_attire_update_gridid_list_out_item_1 *p_out_body_attire;
	mole2_user_attire_update_gridid_list_out_item_2 *p_out_bag_attire;

	ret=this->attire.update_gridid_list(RECVBUF_USERID, p_in_item, p_in->count);
 	if(ret!=SUCC){
		return ret;
    }
	this->attire.get_attire_list(RECVBUF_USERID, &p_out_body_attire, &(p_out_header->attire_body_cnt));
	cpy_len = sizeof(mole2_user_attire_update_gridid_list_out_item_1) * p_out_header->attire_body_cnt;
	memcpy(p_offset, p_out_body_attire, cpy_len);
	free(p_out_body_attire);
	p_out_body_attire = NULL;
	p_offset += cpy_len;

	this->attire.get_attire_list_ex(RECVBUF_USERID, &p_out_bag_attire, &(p_out_header->attire_bag_cnt));
	cpy_len = sizeof(*p_out_bag_attire) * p_out_header->attire_bag_cnt;
	memcpy(p_offset, p_out_bag_attire, cpy_len);
	free(p_out_bag_attire);
	p_out_bag_attire = NULL;
	p_offset += cpy_len;
	
	STD_RETURN_WITH_BUF(ret,out, p_offset - out);
}

int Croute_func::mole2_user_user_login_ex(DEAL_FUN_ARG)
{
	char out[PROTO_MAX_SIZE]={};
	mole2_user_user_login_ex_out_header*	p_out = (mole2_user_user_login_ex_out_header *)out;
	mole2_user_user_login_ex_out_item_1*	p_skill_out_item;
	mole2_user_user_login_ex_out_item_2*	p_attire_on_body_out_item;
	mole2_user_user_login_ex_out_item_3*	p_pet_out_item;
	mole2_user_pet_skill_get_list_out_item*	p_pet_skill_out_item;

	
	mole2_user_user_login_ex_out_item_4*	p_attire_in_grid_out_item;
	mole2_user_user_login_ex_out_item_5*	p_item_out_item;
//	mole2_user_user_login_ex_out_item_6*	p_task_out_item;
	mole2_user_user_login_ex_out_item_7*	p_title_out_item;


	
	stru_mole2_user_user_info_ex	userinfo_ex 		= {};
	stru_mole2_user_battle_info		user_battle_info	= {};
	stru_id_list					user_friend_info	= {};

//get statistics info
	ret = this->user.get_friend_list(RECVBUF_USERID, &user_friend_info);
	if (ret != SUCC) {
		return ret;
	}
	if(user_friend_info.count <= ID_LIST_MAX)
		p_out->friends_cnt	= user_friend_info.count;
	else
		p_out->friends_cnt = ID_LIST_MAX;
	
	ret = this->pet.pet_cnt_get(RECVBUF_USERID, &p_out->all_pet_cnt,0);
	if (ret != SUCC) {
		return ret;
	}
//----------
//get header info	
	ret = this->user.get_all_info(RECVBUF_USERID, &userinfo_ex);
	if (ret != SUCC) {
		return ret;
	}
	
	mole2_get_relation_out relation_info;
	memset(&p_out->relation_info,0,sizeof(p_out->relation_info));
	this->relation.get_relation_info(RECVBUF_USERID, &relation_info);
	p_out->relation_info.flag = relation_info.flag;
	p_out->relation_info.exp = relation_info.exp;
	p_out->relation_info.graduation = relation_info.graduation;
	p_out->relation_info.total_val = relation_info.total_val;
	p_out->relation_info.count = relation_info.relations.size();
	for(uint32_t i = 0; i < relation_info.relations.size();i++) {
		p_out->relation_info.relations[i].uid = relation_info.relations[i].uid;
		p_out->relation_info.relations[i].day = relation_info.relations[i].day;
		p_out->relation_info.relations[i].val = relation_info.relations[i].val;
	}

	ret = this->user2.get_vip_info(RECVBUF_USERID, &userinfo_ex.vip_info);
	if (ret == SUCC) {
		if(userinfo_ex.vip_info.vip_end_time <= time(NULL)) {
			if(userinfo_ex.userinfo.flag & 0x1) {
				DEBUG_LOG("error:vip already end[%u]",RECVBUF_USERID);
				//userinfo_ex.userinfo.flag &= ~0x1;
				//userinfo_ex.userinfo.flag |= 0x100;
				//this->user.set_int_value(RECVBUF_USERID,this->user.str_flag,userinfo_ex.userinfo.flag);
			}
		} else {
			if(!(userinfo_ex.userinfo.flag & 0x1)) {
				DEBUG_LOG("error:vip not begin[%u]",RECVBUF_USERID);
				//userinfo_ex.userinfo.flag |= 0x1;
				//userinfo_ex.userinfo.flag &= ~0x3C00;
				//this->user.set_int_value(RECVBUF_USERID,this->user.str_flag,userinfo_ex.userinfo.flag);
			}
		}
	} else {
		//if(userinfo_ex.userinfo.flag & 0x101) {
			//userinfo_ex.userinfo.flag &= ~0x101;
			//if(ret == USER_ID_NOFIND_ERR) {
				//this->user.set_int_value(RECVBUF_USERID,this->user.str_flag,userinfo_ex.userinfo.flag);
			//}
		//}
		ret = SUCC;
	}

	memcpy(out, &userinfo_ex, sizeof(userinfo_ex));
	
	ret = this->battle.info_get(RECVBUF_USERID, &user_battle_info);
	if (ret != SUCC) {
		ret = 0;
	}
	memcpy(out + sizeof(stru_mole2_user_user_info_ex), &user_battle_info, sizeof(user_battle_info));
	
	uint32_t cpy_len = 0;
	char* p_offset = out + sizeof(*p_out);
// get skill list and skill count
	ret = this->skill.get_skill_list(RECVBUF_USERID, &p_skill_out_item, &(p_out->skill_count));
	if (ret != SUCC) {
		return ret;
	}
	cpy_len = p_out->skill_count * sizeof(mole2_user_skill_get_list_out_item);
	memcpy(p_offset, p_skill_out_item, cpy_len);
	free(p_skill_out_item);
	p_offset += cpy_len;
	
// get pet list on body 
	ret = this->pet.get_pet_list_on_body(RECVBUF_USERID, &p_pet_out_item, &(p_out->pet_count));
	if (ret != SUCC) {
		return ret;
	}
//get pet skill list	
	for (uint32_t i = 0;  i< p_out->pet_count; i++)
	{
		// for each pet, get skill list without petid
		ret = this->pet_skill.get_skill_list(RECVBUF_USERID, (p_pet_out_item+i)->gettime, 
				&p_pet_skill_out_item, &((p_pet_out_item + i)->skillcnt));
			//first copy the pet's info 
		cpy_len = sizeof (mole2_user_user_login_ex_out_item_3);
		memcpy(p_offset, p_pet_out_item + i, cpy_len);
		p_offset += cpy_len;
		//copy the skill info after pet info
		if (ret != SUCC) {
			return ret;
		}
		
		cpy_len = sizeof(mole2_user_skill_get_list_out_item) * p_pet_out_item[i].skillcnt;
		memcpy(p_offset, p_pet_skill_out_item, cpy_len);
		p_offset += cpy_len;
		free(p_pet_skill_out_item);
	}
	free(p_pet_out_item);
//--------------
// get attire list on body 
	ret = this->attire.get_attire_list(RECVBUF_USERID, &p_attire_on_body_out_item, &(p_out->attire_on_body_count));
	if(ret != SUCC) {
		return ret;
	}
	
	cpy_len = sizeof(mole2_user_attire_get_list_out_item) * p_out->attire_on_body_count;
	DEBUG_LOG("ATTIRE ON BODY %u", p_out->attire_on_body_count);
	memcpy(p_offset, p_attire_on_body_out_item,cpy_len);
	free(p_attire_on_body_out_item);
	p_attire_on_body_out_item = NULL;
	p_offset += cpy_len;
//-----------
//---get attire list in grid
	ret = this->attire.get_attire_list_ex(RECVBUF_USERID, &p_attire_in_grid_out_item, &p_out->attire_in_grid_count);
	if (ret != SUCC) {
		return ret;
	}
	cpy_len = p_out->attire_in_grid_count * sizeof(*p_attire_in_grid_out_item);
	memcpy(p_offset, p_attire_in_grid_out_item, cpy_len);
	free(p_attire_in_grid_out_item);
	p_attire_in_grid_out_item = NULL;
	p_offset += cpy_len;
//----
//---get item list
	ret = this->item.list_get(RECVBUF_USERID, &p_item_out_item, &p_out->item_count);
	if (ret != SUCC) {
		return ret;
	}
	cpy_len = p_out->item_count * sizeof(*p_item_out_item);
	memcpy(p_offset, p_item_out_item, cpy_len);
	free(p_item_out_item);
	p_item_out_item = NULL;
	p_offset += cpy_len;
	
//--get user title list 
	bool use_honor=false;
	ret = this->user_title.title_list_get(RECVBUF_USERID, &p_title_out_item, &p_out->title_count,userinfo_ex.userinfo.honor,  &use_honor);
	if (!use_honor ) {
		((stru_mole2_user_user_info_ex*) out)->userinfo.honor=0;
	}
	if (ret != SUCC) {
		return ret;
	}
	cpy_len = p_out->title_count * sizeof(*p_title_out_item);
	memcpy(p_offset, p_title_out_item, cpy_len);
	free(p_title_out_item);
	p_title_out_item= NULL;
	p_offset += cpy_len;
//--
//set login time
	int days = 0;
	uint32_t login  = 0;
	uint32_t now = time(0);
	uint32_t logout = userinfo_ex.time;

	this->user.get_int_value(RECVBUF_USERID, this->user.str_login, &login);

	if(login > logout) logout = login;

	days = time_day_diff(now,logout);
	if (days == 0) {
		ret = this->user.login_set(RECVBUF_USERID, now);
	} else {
		ret = this->user.login_set(RECVBUF_USERID, now, 0);
		stru_mole2_user_user_info_ex* p_uex = (stru_mole2_user_user_info_ex*)out;
		p_uex->daytime = 0;
		if(p_uex->userinfo.flag & (1 << 21)) {
			p_uex->userinfo.flag &= ~(1 << 21);
			this->user.flag_set(RECVBUF_USERID,p_uex->userinfo.flag,p_uex->flag_ex);
			if(days == 1) {//signed yesterday
				p_uex->sign_count++;
			} else {
				p_uex->sign_count = 1;//signed last login but not yesterday
			}
		} else {
			p_uex->sign_count = 1; // not signed last login
		}
		this->user.set_int_value(RECVBUF_USERID, this->user.str_sign_count, p_uex->sign_count);
	}

	if(userinfo_ex.userinfo.level < 9 && !(userinfo_ex.userinfo.flag & (1 << 6))) {
		userinfo_ex.userinfo.flag |= (1 << 6);
		this->user.flag_set(RECVBUF_USERID,userinfo_ex.userinfo.flag,userinfo_ex.flag_ex);
	}

	if (ret != SUCC) {
		return ret;
	}
	
	STD_RETURN_WITH_BUF(ret, out, p_offset - out);
}

int Croute_func::mole2_user_skill_add(DEAL_FUN_ARG)
{
	mole2_user_skill_add_in *p_in=PRI_IN_POS; 
	ret=this->skill.add_skill(RECVBUF_USERID, p_in->skillid, p_in->level);
	STD_RETURN(ret);
}

int Croute_func::mole2_user_skill_get_list(DEAL_FUN_ARG)
{
	mole2_user_skill_get_list_out_header out_header;
	mole2_user_skill_get_list_out_item *p_out_item;
	ret=this->skill.get_skill_list(RECVBUF_USERID, &p_out_item, &(out_header.count));
	STD_RETURN_WITH_STRUCT_LIST(ret,out_header,p_out_item,out_header.count);
}

int Croute_func::mole2_user_skill_add_level(DEAL_FUN_ARG)
{
	mole2_user_skill_add_level_in *p_in=PRI_IN_POS; 
	mole2_user_skill_add_level_out out={};
	ret=this->skill.add_level(RECVBUF_USERID, p_in, &out);
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_skill_del(DEAL_FUN_ARG)
{
	mole2_user_skill_del_in *p_in=PRI_IN_POS; 
	mole2_user_skill_del_out out = {};
	ret=this->skill.del_skill(RECVBUF_USERID, p_in->skillid);
	out.skillid = p_in->skillid;
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_user_skill_add_exp(DEAL_FUN_ARG)
{
	mole2_user_skill_add_exp_in *p_in=PRI_IN_POS; 
	mole2_user_skill_add_exp_out out={};
	ret=this->skill.add_exp(RECVBUF_USERID, p_in, &out);
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_item_add(DEAL_FUN_ARG)
{
	mole2_user_item_add_in *p_in=PRI_IN_POS; 
	mole2_user_item_add_out out={};

	/* 
	if(p_in->type && p_in->count > 0) {
		uint32_t day;
		uint32_t total;
		uint32_t count;

		ret = this->day.get(RECVBUF_USERID,p_in->itemid,&total,&count,&day);
		if(ret != SUCC) {
			ret = SUCC;
			this->day.insert(RECVBUF_USERID,p_in->itemid,p_in->count);
		} else {
			if(day == (uint32_t)today()) {
				count += p_in->count;
			} else {
				count = p_in->count;
			}
			this->day.set(RECVBUF_USERID,p_in->itemid,total + p_in->count,count);
		}
	}
 */

	out.type = p_in->type;
	out.itemid = p_in->itemid;
	ret=this->item.add_item(RECVBUF_USERID, p_in->itemid, p_in->count , (uint32_t*)&(out.count));
	//this->item.get_item(RECVBUF_USERID, p_in->itemid, (uint32_t*)&(out.count));
	if (ret!=SUCC ){
		STD_ROLLBACK();
	}

	this->vip_item_log.log(RECVBUF_USERID, p_in->itemid, p_in->count);

	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_item_get_list(DEAL_FUN_ARG)
{
	mole2_user_item_get_list_out_header  out_header;
	mole2_user_item_get_list_out_item *p_out_item;
	ret=this->item.get_item_list(RECVBUF_USERID, &p_out_item, &(out_header.count));
	STD_RETURN_WITH_STRUCT_LIST(ret,out_header,p_out_item,out_header.count);
}

int Croute_func::mole2_user_item_add_list(DEAL_FUN_ARG)
{
	char out[PROTO_MAX_SIZE] = {0};
	mole2_user_item_add_list_in_header *p_in=PRI_IN_POS;
	CHECK_PRI_IN_LEN_WITHOUT_HEADER((p_in->count)*sizeof(mole2_user_item_add_list_in_item));
	mole2_user_item_add_list_in_item * p_in_item=(mole2_user_item_add_list_in_item*)(RCVBUF_PRIVATE_POS+sizeof(*p_in)) ;
	mole2_user_item_add_list_out_header* p_out_header = (mole2_user_item_add_list_out_header *)out;

	for (uint32_t i = 0; i < p_in->count; i ++)
	{
		if(p_in_item[i].type && p_in_item[i].count > 0){
			uint32_t day;
			uint32_t total;
			uint32_t count;
		
			ret = this->day.get(RECVBUF_USERID,p_in_item[i].itemid,&total,&count,&day);
			if(ret != SUCC) {
				ret = SUCC;
				this->day.insert(RECVBUF_USERID,p_in_item[i].itemid,p_in_item[i].count);
			} else {
				if(day == (uint32_t)today()) {
					count += p_in_item[i].count;
				} else {
					count = p_in_item[i].count;
				}
				this->day.set(RECVBUF_USERID,p_in_item[i].itemid,total + p_in_item[i].count,count);
			}
		}
		ret = this->item.add_item(RECVBUF_USERID, p_in_item[i].itemid, p_in_item[i].count);
		if (ret != SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}
	}
	ret=this->user.add_xiaomee(RECVBUF_USERID, p_in->xiaomee, (mole2_user_user_add_xiaomee_out *)p_out_header);
	if (ret != SUCC) {
		STD_ROLLBACK();
	}

	p_out_header->count = p_in->count;
	int len = p_in->count * sizeof(mole2_user_item_add_list_out_item);
	memcpy(out + sizeof(mole2_user_item_add_list_out_header), p_in_item, len);

	if(ret == SUCC) {
		for (uint32_t i = 0; i < p_in->count; i++) {
			this->vip_item_log.log(RECVBUF_USERID, p_in_item[i].itemid, p_in_item[i].count);
		}
	}

	STD_RETURN_WITH_BUF(ret, out, len + sizeof(mole2_user_item_add_list_out_header));
}

int Croute_func::mole2_user_pet_skill_add(DEAL_FUNC_ARG)
{
	mole2_user_pet_skill_add_in *p_in=P_IN; 
	mole2_user_pet_skill_add_out *p_out=P_OUT;

	p_out->petid = p_in->petid;
	p_out->skillid = p_in->skillid;
	p_out->level = p_in->level;

	ret=this->pet_skill.add_skill(userid, p_in->petid, p_in->skillid, p_in->level);
	return ret;
}

int Croute_func::mole2_user_pet_skill_get_list(DEAL_FUN_ARG)
{
	mole2_user_pet_skill_get_list_in *p_in = PRI_IN_POS; 
	mole2_user_pet_skill_get_list_out_header  out_header;
	mole2_user_pet_skill_get_list_out_item *p_out_item;
	out_header.petid = p_in->petid;
	ret = this->pet_skill.get_skill_list(RECVBUF_USERID, p_in->petid, &p_out_item, &(out_header.count));
	STD_RETURN_WITH_STRUCT_LIST(ret,out_header,p_out_item,out_header.count);
}

/* 
int Croute_func::mole2_user_pet_skill_add_level(DEAL_FUN_ARG)
{
	mole2_user_pet_skill_add_level_in *p_in=PRI_IN_POS; 
	mole2_user_pet_skill_add_level_out out={};
	ret=this->pet_skill.add_level(RECVBUF_USERID, p_in, &out);
	STD_RETURN_WITH_STRUCT(ret,out);
}
*/

int Croute_func::mole2_user_pet_skill_del(DEAL_FUNC_ARG)
{
	mole2_user_pet_skill_del_in *p_in=P_IN;
	mole2_user_pet_skill_del_out *p_out=P_OUT; 

	ret=this->pet_skill.del_skill(userid, p_in);
	p_out->petid = p_in->petid;
	p_out->skillid = p_in->skillid;
	p_out->level = p_in->level;

	return ret;
}


int Croute_func::mole2_user_item_buy(DEAL_FUN_ARG)
{
	mole2_user_item_buy_in *p_in = PRI_IN_POS; 
	mole2_user_item_buy_out out = {};
	ret=this->item.add_item(RECVBUF_USERID, p_in->itemid, p_in->count);
	if (ret == SUCC)
	{
		ret = this->user.add_xiaomee(RECVBUF_USERID, p_in->xiaomee, &out.xiaomee_info);
		if (ret != SUCC) {
			STD_ROLLBACK();
		}
	}
	out.itemid = p_in->itemid;
	this->item.get_item(RECVBUF_USERID, p_in->itemid, &(out.count));

	if(ret == SUCC) {
		this->vip_item_log.log(RECVBUF_USERID, p_in->itemid, p_in->count);
	}

	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_user_item_ranged_get_list(DEAL_FUN_ARG)
{
	mole2_user_item_ranged_get_list_in *p_in = PRI_IN_POS; 
	mole2_user_item_ranged_get_list_out_header  out_header;
	mole2_user_item_ranged_get_list_out_item *p_out_item;
	out_header.count = 0;
	ret=this->item.get_item_list_ranged(RECVBUF_USERID, p_in, &p_out_item, &(out_header.count));
	STD_RETURN_WITH_STRUCT_LIST(ret,out_header,p_out_item,out_header.count);
}

int Croute_func::user_user_injury_state_set(DEAL_FUN_ARG)
{
	user_user_injury_state_set_in *p_in = PRI_IN_POS; 
	ret = this->user.injury_state_update(RECVBUF_USERID, p_in->injury_state);
	STD_RETURN(ret);
}

int Croute_func::user_pet_injury_state_set(DEAL_FUN_ARG)
{
	user_pet_injury_state_set_in *p_in = PRI_IN_POS; 
	ret = this->pet.injury_state_update(RECVBUF_USERID, p_in->petid, p_in->injury_state);
	STD_RETURN(ret);
}

int Croute_func::user_user_injury_state_get(DEAL_FUN_ARG)
{
	user_user_injury_state_get_out out = {};
	ret = this->user.injury_state_select(RECVBUF_USERID, &out.injury_state);
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::user_pet_injury_state_get(DEAL_FUN_ARG)
{
	user_pet_injury_state_get_in *p_in = PRI_IN_POS; 
	user_pet_injury_state_get_out out = {};
	ret = this->pet.injury_state_select(RECVBUF_USERID, p_in->petid, &out.injury_state);
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::user_attire_info_get(DEAL_FUN_ARG)
{
 	user_attire_info_get_in_header *p_in = PRI_IN_POS;
	CHECK_PRI_IN_LEN_WITHOUT_HEADER((p_in->count)*sizeof(user_attire_info_get_in_item));
	user_attire_info_get_in_item * p_in_item = (user_attire_info_get_in_item*)(RCVBUF_PRIVATE_POS+sizeof(*p_in)) ;
	user_attire_info_get_out_header  out_header;
	user_attire_info_get_out_item *p_out_item;

	out_header.count = p_in->count;	
	p_out_item = (user_attire_info_get_out_item*)malloc(p_in->count * sizeof(user_attire_info_get_out_item));
	for (uint32_t i = 0; i < p_in->count; i++) {
		ret = this->attire.get_attire_simple(RECVBUF_USERID,
				(p_in_item + i)->gettime,
				&((p_out_item + i)->attire_id),
				&((p_out_item + i)->level)
				);	
		(p_out_item + i)->gettime = (p_in_item + i)->gettime;
	}

	STD_RETURN_WITH_STRUCT_LIST(ret,out_header, p_out_item,out_header.count);
}

int Croute_func::user_attire_sell_set(DEAL_FUN_ARG)
{
 	user_attire_sell_set_in_header *p_in = PRI_IN_POS;
	char out[PROTO_MAX_SIZE];
 	user_attire_sell_set_out_header* p_out_header	= (user_attire_sell_set_out_header*)out;
	CHECK_PRI_IN_LEN_WITHOUT_HEADER((p_in->count)*sizeof(user_attire_sell_set_in_item));
	user_attire_sell_set_in_item* p_in_item 	= (user_attire_sell_set_in_item*)(RCVBUF_PRIVATE_POS+sizeof(*p_in)) ;
	char* p_offset = out + sizeof(*p_out_header);
	uint32_t attireid;
	for (uint32_t i = 0; i < p_in->count; i++) {
		ret = this->attire.del_attire(RECVBUF_USERID, (p_in_item + i)->gettime,&attireid);
		if (ret != DB_SUCC) {
			STD_ROLLBACK();
			break;
		}
	}
	if (ret == DB_SUCC) {
		ret = this->user.add_xiaomee(RECVBUF_USERID, p_in->xiaomee, &p_out_header->xiaomee_info);
		if (ret != DB_SUCC)
			STD_ROLLBACK();
	}
	p_out_header->count = p_in->count;
	
	memcpy(p_offset, p_in_item, p_in->count * sizeof(user_attire_sell_set_out_item));
	p_offset += p_in->count * sizeof(user_attire_sell_set_out_item);
	STD_RETURN_WITH_BUF(ret, out, p_offset - out);
}

int Croute_func::user_create_role_set(DEAL_FUN_ARG)
{
    user_create_role_set_in_header *p_in = PRI_IN_POS;
	user_create_role_set_in_item *p_in_item = (user_create_role_set_in_item*)(RCVBUF_PRIVATE_POS+sizeof(*p_in)) ;

	char out[PROTO_MAX_SIZE] = {0};
	user_create_role_set_out_header *p_out_h = (user_create_role_set_out_header*)out;

	CHECK_PRI_IN_LEN_WITHOUT_HEADER((p_in->count) * sizeof(user_create_role_set_in_item));

	ret = this->battle.insert(RECVBUF_USERID);
	if (ret == USER_ID_EXISTED_ERR) {
		stru_mole2_user_battle_info_1 battle_info = {};
		battle_info.binfo.all_score = 100;
		ret = this->battle.info_set_ex(RECVBUF_USERID, &battle_info);
		if (ret != SUCC) return ret;
	}

    ret = this->user.update_role(RECVBUF_USERID, p_in->nick, p_in->prof, p_in->color, p_in->attribute_value, p_in->hp, p_in->mp);
	if(ret == SUCC) {
 		ret = this->user.get_all_info(RECVBUF_USERID, &p_out_h->userinfo);
		for(p_out_h->count = 0; ret == SUCC && p_out_h->count < p_in->count; p_out_h->count++) {
			ret = this->attire.attire_add(RECVBUF_USERID, &p_in_item[p_out_h->count].attire_info,&p_in_item[p_out_h->count].gettime);
		}
	}

	if (ret == SUCC) {
		memcpy(out + sizeof(*p_out_h),p_in_item,p_in->count * sizeof(user_create_role_set_in_item));
	} else {
		STD_ROLLBACK();
	}
	STD_RETURN_WITH_BUF(ret, out, sizeof(*p_out_h) + p_out_h->count * sizeof(user_create_role_set_in_item));
}

int Croute_func::mole2_user_user_cure(DEAL_FUN_ARG)
{
	int bytes = 0;
	char out[PROTO_MAX_SIZE];
	mole2_user_user_cure_in_item 	*p_in_item;
	mole2_user_user_cure_in_header *p_in=PRI_IN_POS;
	mole2_user_user_cure_out_header *p_out=(mole2_user_user_cure_out_header*)out;

	CHECK_PRI_IN_LEN_WITHOUT_HEADER(p_in->count * sizeof(mole2_user_user_cure_in_item));
	p_in_item = (mole2_user_user_cure_in_item*)(RCVBUF_PRIVATE_POS+sizeof(*p_in));

	p_out->count = p_in->count;
	ret = this->user.add_xiaomee(RECVBUF_USERID,p_in->add_xiaomee,&p_out->xiaomee_info);
	for(uint32_t i = 0; ret == SUCC && i < p_in->count; i++,p_in_item++) {
		if(p_in_item->petid == 0) {
			ret=this->user.set_hp_mp_injury(RECVBUF_USERID, p_in_item->hp, p_in_item->mp, p_in_item->injury);
		} else {
			ret=this->pet.set_hp_mp_injury(RECVBUF_USERID, p_in_item->petid, p_in_item->hp, p_in_item->mp, p_in_item->injury);
		}
	}

	if(ret != SUCC) {
		STD_ROLLBACK();
	} else {
		bytes = sizeof(*p_in_item) * p_in->count;
		memcpy(out + sizeof(*p_out),(char*)p_in + sizeof(*p_in),bytes);
		bytes += sizeof(*p_out);
	}

	STD_RETURN_WITH_BUF(ret,out,bytes);
}

int Croute_func::mole2_user_pet_cure(DEAL_FUN_ARG)
{
	mole2_user_pet_cure_in *p_in=PRI_IN_POS; 
	mole2_user_pet_cure_out out={};
	ret=this->pet.set_hp_mp(RECVBUF_USERID, p_in->petid, p_in->hp, p_in->mp);
	if (ret == DB_SUCC)
	{
		out.petid = p_in->petid;
		out.hp = p_in->hp;
		out.mp = p_in->mp;
		ret = this->user.add_xiaomee(RECVBUF_USERID, p_in->xiaomee, &out.xiaomee_info);
	}
	STD_RETURN_WITH_STRUCT(ret,out);
}
int Croute_func::mole2_user_user_set_pos(DEAL_FUN_ARG)
{
	mole2_user_user_set_pos_in *p_in=PRI_IN_POS; 
	mole2_user_user_set_pos_out out={};
	ret=this->user.set_pos(RECVBUF_USERID, p_in->pos);
	if (ret == DB_SUCC)
		out.pos = p_in->pos;
	STD_RETURN_WITH_STRUCT(ret,out);
}
/*
int Croute_func::mole2_user_user_injury_cure(DEAL_FUN_ARG)
{
	mole2_user_user_injury_cure_in *p_in=PRI_IN_POS; 
	mole2_user_user_injury_cure_out out={};
	ret=this->user.injury_state_update(RECVBUF_USERID, 0);
	if (ret ==SUCC)
	{
		ret = this->user.add_xiaomee(RECVBUF_USERID, p_in->xiaomee, &out);
		if (ret != SUCC)
			STD_ROLLBACK();
	}
	STD_RETURN_WITH_STRUCT(ret,out);
}
*/
int Croute_func::mole2_user_pet_injury_cure(DEAL_FUN_ARG)
{
	mole2_user_pet_injury_cure_in *p_in=PRI_IN_POS; 
	mole2_user_pet_injury_cure_out out={};
	ret=this->pet.injury_state_update(RECVBUF_USERID, p_in->petid, 0);
	if (ret ==SUCC)
	{
		ret = this->user.add_xiaomee(RECVBUF_USERID, p_in->xiaomee, &out.xiaomee_info);
		if (ret != SUCC)
			STD_ROLLBACK();
	}
	out.petid = p_in->petid;
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_skill_buy(DEAL_FUN_ARG)
{
	mole2_user_skill_buy_in *p_in=PRI_IN_POS; 
	mole2_user_skill_buy_out out={};
	ret=this->skill.add_skill(RECVBUF_USERID, p_in->skill_id, p_in->skill_lv);
	if (ret == SUCC)
	{
		ret = this->user.add_xiaomee(RECVBUF_USERID, p_in->xiaomee, &(out.xiaomee));
		if (ret != SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}
		out.skill.skill_id = p_in->skill_id;
		out.skill.skill_lv = p_in->skill_lv;
		out.skill.skill_exp = 0;
	}
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_pet_skill_buy(DEAL_FUN_ARG)
{
	mole2_user_pet_skill_buy_in *p_in=PRI_IN_POS; 
	mole2_user_pet_skill_buy_out out={};
	ret=this->pet_skill.add_skill(RECVBUF_USERID, p_in->petid, p_in->skillid, p_in->level);
	if (ret == SUCC)
	{
		ret = this->user.add_xiaomee(RECVBUF_USERID, p_in->xiaomee, &(out.xiaomee));
		if (ret != SUCC)
			STD_ROLLBACK();
		out.petid = p_in->petid;
		out.skillid = p_in->skillid;
		out.level = p_in->level;
	}
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::user_user_levelup_set(DEAL_FUN_ARG)
{
	user_user_levelup_set_in *p_in = PRI_IN_POS; 
	ret = this->user.levelup_update(RECVBUF_USERID, p_in);
	STD_RETURN(ret);
}

int Croute_func::user_pet_levelup_set(DEAL_FUN_ARG)
{
    user_pet_levelup_set_in *p_in = PRI_IN_POS;
    ret = this->pet.levelup_update(RECVBUF_USERID, p_in);
    STD_RETURN(ret);
}

int Croute_func::user_user_skill_up_set(DEAL_FUN_ARG)
{
    user_user_skill_up_set_in *p_in = PRI_IN_POS;
    ret=this->skill.skill_up_update(RECVBUF_USERID, p_in);
    STD_RETURN(ret);
}

int Croute_func::mole2_user_task_item_add(DEAL_FUN_ARG)
{
	mole2_user_task_item_add_in *p_in=PRI_IN_POS; 
	mole2_user_task_item_add_out out={};
	//ret=this->task_item.add_item(RECVBUF_USERID, p_in->taskid, p_in->itemid, p_in->itemtype, p_in->count, p_in->expiretime);
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::mole2_user_task_item_del(DEAL_FUN_ARG)
{
	mole2_user_task_item_del_in *p_in=PRI_IN_POS; 
	//ret=this->task_item.del_item(RECVBUF_USERID, p_in->taskid);
	STD_RETURN(ret);
}

int Croute_func::mole2_user_task_item_get_list(DEAL_FUN_ARG)
{
	//mole2_user_task_item_get_list_out_header  out_header;
	//mole2_user_task_item_get_list_out_item *p_out_item;
	//ret=this->task_item.get_item_list(RECVBUF_USERID, &p_out_item, &out_header.count);
	//STD_RETURN_WITH_STRUCT_LIST(ret,out_header,p_out_item,out_header.count);
	return 0;
}

int Croute_func::user_user_skill_list_levelup_set(DEAL_FUN_ARG)
{
    user_user_skill_list_levelup_set_in_header *p_in=PRI_IN_POS;
    CHECK_PRI_IN_LEN_WITHOUT_HEADER((p_in->count) * sizeof(user_user_skill_list_levelup_set_in_item));
    user_user_skill_list_levelup_set_in_item* p_in_item = 
		(user_user_skill_list_levelup_set_in_item*)(RCVBUF_PRIVATE_POS+sizeof(*p_in));
	for (uint32_t i = 0; i < p_in->count; i++) {
		ret = this->skill.skill_up_set(RECVBUF_USERID, p_in_item[i].skillid, p_in_item[i].level, p_in_item[i].exp);
	}	
    STD_RETURN(SUCC);
}

int Croute_func::user_user_flag_set(DEAL_FUN_ARG)
{
    user_user_flag_set_in *p_in=PRI_IN_POS;
	user_user_flag_set_out out = {};
    ret = this->user.flag_set(RECVBUF_USERID, p_in->flag, p_in->flag_ex);
	out.flag = p_in->flag;
	out.flag_ex	= p_in->flag_ex;
    STD_RETURN_WITH_STRUCT(ret, out);
}   

int Croute_func::rand_item_info_check(DEAL_FUN_ARG)
{
    rand_item_info_check_in *p_in = PRI_IN_POS;
	rand_item_info_check_out out = {};
	uint32_t	db_time = 0;
	uint32_t	db_count = 0;

	time_t	now_time	= time(0);
	
    ret = this->rand_item.get_rand_info(RECVBUF_USERID, p_in->randid, &db_time, &db_count);
	if (ret == MOLE2_RAND_INFO_NOFIND_ERR) {
		ret = this->rand_item.insert(RECVBUF_USERID, p_in->randid, now_time, 0);
		out.randid = p_in->randid;
		out.time = now_time;
		out.count = 0;
		STD_RETURN_WITH_STRUCT(ret, out);
	}
	if (ret != SUCC) {
		return ret;
	}

	//set out
	out.randid	= p_in->randid;
	out.time	= db_time;
	out.count	= db_count;

	struct tm		now_tm;
	struct tm		db_tm;


	time_t t_db = db_time;
	localtime_r(&t_db, &db_tm);
	localtime_r(&now_time, &now_tm);

	if (p_in->date_type == 1 && db_tm.tm_yday != now_tm.tm_yday) {
		ret = this->rand_item.set_rand_info(RECVBUF_USERID, p_in->randid, now_time, 0);
	}

/*
	if (p_in->date_type == 1) { // day circle
		if (db_tm.tm_year == now_tm.tm_year && now_tm.tm_yday == now_tm.tm_yday && db_count >= p_in->max_count) {
			return 	RAND_ITEM_COUNT_FULL_ERR;
		} else if (db_tm.tm_year != now_tm.tm_year || (db_tm.tm_year == now_tm.tm_year && db_tm.tm_yday != now_tm.tm_yday)){
			ret = this->rand_item.set_rand_info(RECVBUF_USERID, p_in->randid, now_time, 0);
		} else {
			return SUCC;
		}
	} else if (p_in->date_type == 2) { //week circle
		if (db_count >= p_in->max_count) {
			return RAND_ITEM_COUNT_FULL_ERR;
		} else {
			return SUCC;
		}
	}
*/



	
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::rand_item_info_set(DEAL_FUN_ARG)
{
    rand_item_info_set_in *p_in=PRI_IN_POS;
	
    ret = this->rand_item.set_rand_info(RECVBUF_USERID, p_in->randid, time(0));
		
    STD_RETURN(ret);
}

int Croute_func::user_pet_all_get(DEAL_FUN_ARG)
{
    user_pet_all_get_in *p_in = PRI_IN_POS;
    user_pet_all_get_out_header  out_header = {};
    user_pet_all_get_out_item *p_out_item;

	//get pet info from t_pet
    ret = this->pet.get_pet_info(RECVBUF_USERID, p_in->gettime, &out_header.pet_info_ex);
	if (ret != SUCC) {
		return ret;
	}
	//get pet skill from t_pet_skill
	ret = this->pet_skill.get_skill_list(RECVBUF_USERID, p_in->gettime, &p_out_item, &out_header.count);
    STD_RETURN_WITH_STRUCT_LIST(ret,out_header,p_out_item,out_header.count);
}

int Croute_func::user_user_medicine_cure(DEAL_FUN_ARG)
{
    user_user_medicine_cure_in *p_in=PRI_IN_POS; 
    user_user_medicine_cure_out out={};
    ret = this->user.set_hp_mp(RECVBUF_USERID, p_in->hp, p_in->mp);
	if (ret != SUCC) {
		return ret;
	}
	ret = this->item.add_item(RECVBUF_USERID, p_in->itemid, -1);
	memcpy(&out, p_in, sizeof(user_user_medicine_cure_in));

	if(ret == SUCC) {
		this->vip_item_log.log(RECVBUF_USERID, p_in->itemid, -1);
	}

    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::user_pet_location_set(DEAL_FUN_ARG)
{
    user_pet_location_set_in *p_in=PRI_IN_POS;
    user_pet_location_set_out_header	out_header = {};
	user_pet_location_set_out_item*		p_out_item;

	ret = this->pet.get_pet_info(RECVBUF_USERID, p_in->petid, &out_header.pet_info_ex);
	if (ret != SUCC) {
		return ret;
	}
	if (out_header.pet_info_ex.pet_info.location >= 20000) {
		return MOLE2_PETID_NOFIND_ERR;
	}

	ret = this->pet_skill.get_skill_list(RECVBUF_USERID, p_in->petid, &p_out_item, &out_header.count);
	if (ret != SUCC) {
		return ret;
	}

	out_header.pet_info_ex.pet_info.location = p_in->location;
	ret = this->pet.location_set(RECVBUF_USERID, p_in->petid, p_in->location);	

    STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

int Croute_func::user_user_note_dream_set(DEAL_FUN_ARG)
{
    user_user_note_dream_set_in *p_in = PRI_IN_POS;
	user_user_note_dream_set_out out = {};
	out.profession = p_in->profession;
	
	ret = SUCC;
	if (p_in->profession != 0) {
		ret = this->user.profession_set(RECVBUF_USERID, p_in->profession);
	}
	
    STD_RETURN_WITH_STRUCT(ret, out);
}  

int Croute_func::user_pet_catch_set(DEAL_FUN_ARG)
{
    user_pet_catch_set_in_header *p_in = PRI_IN_POS;
    CHECK_PRI_IN_LEN_WITHOUT_HEADER((p_in->skillcnt) * sizeof(user_pet_catch_set_in_item));
    user_pet_catch_set_in_item * p_in_item=(user_pet_catch_set_in_item*)(RCVBUF_PRIVATE_POS+sizeof(*p_in)) ;

	char out[PROTO_MAX_SIZE] = {};

	
	// add pet info in t_pet_00
    ret = this->pet.pet_add(RECVBUF_USERID, &p_in->pet_info, &p_in->gettime);
	if (ret != SUCC) {
		return ret;
	}
	// add pet skill info in t_pet_skill_00
	for (uint32_t i = 0; i < p_in->skillcnt; i++) {
		ret = this->pet_skill.add_skill(RECVBUF_USERID, p_in->gettime, (p_in_item + i)->skillid, p_in_item->level);
		if (ret != SUCC) {
			STD_ROLLBACK();
			return ret;
		}
	}
	//decrease item in t_item_00
//	ret = this->item.add_item(RECVBUF_USERID, p_in->itemid, -1);

	char*	p_offset	= out;
	int		pkglen		= sizeof(*p_in);
	memcpy(p_offset, p_in, pkglen);
	p_offset	+= pkglen;
	pkglen	= p_in->skillcnt * sizeof(*p_in_item);
	memcpy(p_offset, p_in_item, pkglen);
	p_offset	+= pkglen;
	STD_RETURN_WITH_BUF(ret, out, p_offset - out);
}   

int Croute_func::user_pet_list_all_get(DEAL_FUN_ARG)
{
	char out[PROTO_MAX_SIZE];
    user_pet_list_all_get_out_header*  p_out_header = (user_pet_list_all_get_out_header*)out;
    user_pet_list_all_get_out_item_1*	p_out_item_1;	// pet info
	user_pet_list_all_get_out_item_2*	p_out_item_2;	// pet skill
    

	// get pet list on body 
	ret = this->pet.get_pet_list_on_body(RECVBUF_USERID, &p_out_item_1, &(p_out_header->count));
	if (ret != SUCC) {
		return ret;
	}

	int cpy_len = 0;
	char* p_offset = out;
	p_offset += sizeof(*p_out_header);
//get pet skill list	
	for (uint32_t i = 0;  i< p_out_header->count; i++)
	{
		// for each pet, get skill list without petid
		ret = this->pet_skill.get_skill_list(RECVBUF_USERID, (p_out_item_1 + i)->gettime, 
				&p_out_item_2, &((p_out_item_1 + i)->skillcnt));
		//first copy the pet's info 
		cpy_len = sizeof (*p_out_item_1);
		memcpy(p_offset, p_out_item_1 + i, cpy_len);
		p_offset += cpy_len;
		//copy the skill info after pet info
		if (ret != SUCC) {
			return ret;
		}
		
		cpy_len = sizeof(*p_out_item_2) * p_out_item_1[i].skillcnt;
		memcpy(p_offset, p_out_item_2, cpy_len);
		p_offset += cpy_len;
		free(p_out_item_2);
			
	}
	free(p_out_item_1);
    STD_RETURN_WITH_BUF(ret, out, p_offset - out);
}   

int Croute_func::activation_code_check(DEAL_FUN_ARG)
{
	/*  
    activation_code_check_in *p_in=PRI_IN_POS;
    ret = this->activation_code.check(RECVBUF_USERID, p_in->acode);
	*/
	ret = 0;
    STD_RETURN(ret);
}

int Croute_func::user_map_flight_list_get(DEAL_FUN_ARG)
{
    user_map_flight_list_get_out_header  out_header;
    user_map_flight_list_get_out_item *p_out_item;
    ret = this->user_map.flight_list_get(RECVBUF_USERID, &p_out_item, &out_header.count);
    STD_RETURN_WITH_STRUCT_LIST(ret,out_header,p_out_item,out_header.count);
}

int Croute_func::mole2_user_map_flight_check(DEAL_FUN_ARG)
{
    mole2_user_map_flight_check_in *p_in = PRI_IN_POS;
    mole2_user_map_flight_check_out out = {};

	uint32_t db_flight;
    ret = this->user_map.flight_get(RECVBUF_USERID, p_in->mapid, &db_flight);
	if (ret == USER_MAP_MAPID_NOFIND_ERR) {
		return USER_MAP_FLIGHT_PATH_ERR;
	}
	if (ret != SUCC) {
		return ret;
	}
	if ((db_flight & MAP_FLIGHT_PATH_ON) == 0) {
		return USER_MAP_FLIGHT_PATH_ERR;
	}
	out.mapid = p_in->mapid;
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::user_map_flight_set(DEAL_FUN_ARG)
{   
    user_map_flight_set_in *p_in=PRI_IN_POS;
    ret = this->user_map.insert(RECVBUF_USERID, p_in->mapid, MAP_FLIGHT_PATH_ON);
	if (ret == USER_MAP_MAPID_EXISTED_ERR) {
		ret = this->user_map.flight_set(RECVBUF_USERID, p_in->mapid, MAP_FLIGHT_PATH_ON);
	}
    STD_RETURN(ret);
}

int Croute_func::user_logout_position_set(DEAL_FUN_ARG)
{
    user_logout_position_set_in *p_in=PRI_IN_POS;
	uint32_t login;
	uint32_t daytime;
	uint32_t login_yday;
	uint32_t now_yday;
	
	time_t now = time(0);
	ret = this->user.login_get(RECVBUF_USERID, &login, &daytime);
	if (ret != SUCC) {
		return ret;
	}
	
	struct tm tm_cur = {};
	
	time_t temp = login;
	localtime_r(&temp, &tm_cur);
	login_yday = tm_cur.tm_yday;

	localtime_r(&now, &tm_cur);
	now_yday = tm_cur.tm_yday;

	if (login_yday == now_yday) {
		daytime += now - login;
	} else {
//		daytime = tm_cur.tm_hour * 3600 + tm_cur.tm_min * 60 + tm_cur.tm_sec;
		daytime = 0;
	}

    ret = this->user.logout_position_set(RECVBUF_USERID, p_in->time, p_in->axis_x, p_in->axis_y, p_in->mapid, p_in->maptype, daytime, p_in->fly_mapid, p_in->last_online);
	this->user.update_total_time(RECVBUF_USERID,now - login);

    STD_RETURN(ret);
}

int Croute_func::user_handbook_list_get(DEAL_FUN_ARG)
{
    user_handbook_list_get_out_header  out_header;
    user_handbook_list_get_out_item_1 *p_out_item_1;	//item_handbook
    user_handbook_list_get_out_item_2 *p_out_item_2;	//monster_handbook

	char out[PROTO_MAX_SIZE];
    ret = this->item_handbook.list_get(RECVBUF_USERID, &p_out_item_1, &out_header.item_cnt);
	if (ret != SUCC) {
		return ret;
	}
	ret = this->monster_handbook.list_get(RECVBUF_USERID, &p_out_item_2, &out_header.monster_cnt);
	if (ret != SUCC) {
		return ret;
	}
	
	char* p_offset = out;
	int tmplen;
//copy header
	tmplen = sizeof(out_header);
	memcpy(p_offset, &out_header, tmplen);
	p_offset += tmplen;
//copy item handbook
	tmplen = out_header.item_cnt * sizeof(*p_out_item_1);
	memcpy(p_offset, p_out_item_1, tmplen);
	p_offset += tmplen;
	free(p_out_item_1);
//cpoy monster handbook
	tmplen = out_header.monster_cnt * sizeof(*p_out_item_2);
	memcpy(p_offset, p_out_item_2, tmplen);
	p_offset += tmplen;
	free(p_out_item_2);
	
	
    STD_RETURN_WITH_BUF(ret, out, p_offset - out);
}

int Croute_func::monster_handbook_range_get(DEAL_FUN_ARG)
{
        monster_handbook_range_get_in *p_in = PRI_IN_POS; 
        monster_handbook_range_get_out_header  out_header;
        monster_handbook_range_get_out_item *p_out_item;
        ret = this->monster_handbook.list_range_get(RECVBUF_USERID, p_in->min,p_in->max,&p_out_item, &out_header.count);
        STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

int Croute_func::item_handbook_state_get(DEAL_FUN_ARG)
{
        item_handbook_state_get_in *p_in = PRI_IN_POS; 
        item_handbook_state_get_out out = {};
		out.itemid=p_in->itemid;
        ret = this->item_handbook.state_get(RECVBUF_USERID, p_in->itemid, &out.state);
        STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::item_handbook_state_set(DEAL_FUN_ARG)
{
    item_handbook_state_set_in *p_in = PRI_IN_POS;
	item_handbook_state_set_out out = {};
	uint32_t db_state;
    ret = this->item_handbook.state_get(RECVBUF_USERID, p_in->handbookid, &db_state);
	if (ret == ITEM_HANDBOOK_ID_NOFIND_ERR) {
		ret = this->item_handbook.insert(RECVBUF_USERID, p_in->handbookid, ITEM_HANDBOOK_STATE_ON, 0);
	} else if (ret == SUCC && db_state != ITEM_HANDBOOK_STATE_ON) {
		ret = this->item_handbook.state_set(RECVBUF_USERID, p_in->handbookid, ITEM_HANDBOOK_STATE_ON);
	} else if (ret == SUCC && db_state == ITEM_HANDBOOK_STATE_ON) {
		return ITEM_HANDBOOK_STATE_IS_ON_ERR;
	}
	if (ret != SUCC) {
		return ret;
	}
	ret = this->item.add_item(RECVBUF_USERID, p_in->itemid, p_in->count);
	if (ret != SUCC) {
		STD_ROLLBACK();
	}
	out.itemid = p_in->itemid;
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::monster_handbook_state_set(DEAL_FUN_ARG)
{
	monster_handbook_state_set_in *p_in = PRI_IN_POS;
	monster_handbook_state_set_out out = {};
	uint32_t db_state;
    ret = this->monster_handbook.state_get(RECVBUF_USERID, p_in->handbookid, &db_state);
	if (ret == MONSTER_HANDBOOK_ID_NOFIND_ERR) {
		ret = this->monster_handbook.insert(RECVBUF_USERID, p_in->handbookid, p_in->state, 0);
	} else if (ret == SUCC) {
		if((p_in->state & 0xFFFF) && (db_state & p_in->state & 0xFFFF)) {
			return MONSTER_HANDBOOK_STATE_IS_ON_ERR;
		} else if(!(p_in->state & 0xFFFF) && (p_in->state >> 16) == (db_state >> 16)) {
			return MONSTER_HANDBOOK_STATE_IS_ON_ERR;
		}

		db_state &= 0xFFFF;
		db_state |= p_in->state;
		ret = this->monster_handbook.state_set(RECVBUF_USERID, p_in->handbookid,db_state);
	}

	if (ret != SUCC) {
		return ret;
	}

	ret = this->item.add_item(RECVBUF_USERID, p_in->itemid, p_in->count);
	if (ret != SUCC) {
		STD_ROLLBACK();
	}

	out.itemid	= p_in->itemid;
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::item_handbook_count_add(DEAL_FUN_ARG)
{   
    item_handbook_count_add_in *p_in = PRI_IN_POS;
    ret = this->item_handbook.count_add(RECVBUF_USERID, p_in->itemid, p_in->count);
	if (ret == ITEM_HANDBOOK_ID_NOFIND_ERR) {
		ret = this->item_handbook.insert(RECVBUF_USERID, p_in->itemid, 0, p_in->count);
	}
    STD_RETURN(ret);
}

int Croute_func::monster_handbook_count_list_add(DEAL_FUN_ARG)
{
    monster_handbook_count_list_add_in_header *p_in=PRI_IN_POS;
    CHECK_PRI_IN_LEN_WITHOUT_HEADER((p_in->count) * sizeof(monster_handbook_count_list_add_in_item));
    monster_handbook_count_list_add_in_item * p_in_item=(monster_handbook_count_list_add_in_item*)(RCVBUF_PRIVATE_POS+sizeof(*p_in)) ;
	for(uint32_t i = 0; i < p_in->count; ++i) {
		ret = this->monster_handbook.count_add(RECVBUF_USERID, (p_in_item + i)->monsterid, (p_in_item + i)->count);
		if (ret == MONSTER_HANDBOOK_ID_NOFIND_ERR) {
			ret = this->monster_handbook.insert(RECVBUF_USERID, (p_in_item + i)->monsterid, 0, (p_in_item + i)->count);
		}
		if (ret != SUCC) {
			STD_ROLLBACK();
			return ret;
		}
	}
    STD_RETURN(ret);
}

int Croute_func::user_title_list_get(DEAL_FUN_ARG)
{
	user_title_list_get_out_header  out_header;
	user_title_list_get_out_item *p_out_item;
	bool flag; 
	ret = this->user_title.title_list_get(RECVBUF_USERID, &p_out_item, &out_header.count,0,&flag);
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

int Croute_func::user_honor_set(DEAL_FUN_ARG)
{
	user_honor_set_in *p_in = PRI_IN_POS; 
	user_honor_set_out out = {};
	ret = this->user.honor_set(RECVBUF_USERID, p_in->titleid);
	out.titleid	= p_in->titleid;
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::user_pet_follow_set(DEAL_FUN_ARG)
{
	user_pet_follow_set_in *p_in = PRI_IN_POS; 
	user_pet_follow_set_out out = {};
	ret = this->pet.follow_pet_clear(RECVBUF_USERID);

	if (ret != DB_SUCC) {
		return ret;
	}
	
	if (p_in->petid != 0) {
		ret = this->pet.follow_pet_set(RECVBUF_USERID, p_in->petid);
	}
	out.petid = p_in->petid;
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::user_map_boss_position_set(DEAL_FUN_ARG)
{
    user_map_boss_position_set_in *p_in = PRI_IN_POS;
	uint32_t flag;
	ret = this->user_map.flight_get(RECVBUF_USERID, p_in->mapid, &flag);
	if (ret == USER_MAP_MAPID_NOFIND_ERR) {
		ret = this->user_map.insert(RECVBUF_USERID, p_in->mapid, MAP_BOSS_POSITION_ON);
		STD_RETURN(ret);
	}
	if (ret != SUCC) {
		return ret;
	}
	if ((flag & MAP_BOSS_POSITION_ON) != 0) {
		return UER_MAP_BOSS_POSITION_ON_ERR;
	}
    STD_RETURN(ret);
}

int Croute_func::user_task_optdate_get(DEAL_FUN_ARG)
{
    user_task_optdate_get_in *p_in = PRI_IN_POS; 
    user_task_optdate_get_out out = {};
	ret = this->task.fin_time_get(RECVBUF_USERID, p_in->taskid, &out.optdate);
	if (ret != SUCC) {
		return ret;
	}
	ret = this->task_handbook.content_get(RECVBUF_USERID, p_in->taskid, out.content);
	if (ret == MOLE2_TASK_NOFIND_ERR) {
		ret = SUCC;
		//memcpy(&out.content, "", sizeof(out.content));
	}
	out.taskid = p_in->taskid;
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::tash_handbook_content_set(DEAL_FUN_ARG)
{
	tash_handbook_content_set_in *p_in = PRI_IN_POS; 
	tash_handbook_content_set_out out = {0};
	ret = this->task_handbook.content_set(RECVBUF_USERID, p_in->taskid, p_in->content);
	if (ret == MOLE2_TASK_NOFIND_ERR) {
		ret = this->task_handbook.insert(RECVBUF_USERID, p_in->taskid, p_in->content);
	}
	out.taskid = p_in->taskid;
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::user_attire_list_duration_set(DEAL_FUN_ARG)
{
    user_attire_list_duration_set_in_header *p_in = PRI_IN_POS;
    CHECK_PRI_IN_LEN_WITHOUT_HEADER((p_in->count) * sizeof(user_attire_list_duration_set_in_item));
    user_attire_list_duration_set_in_item * p_in_item = (user_attire_list_duration_set_in_item*)(RCVBUF_PRIVATE_POS+sizeof(*p_in)) ;
	for (uint32_t i = 0; i < p_in->count; ++i) {
		ret = this->attire.duration_set(RECVBUF_USERID, (p_in_item + i)->attireid, (p_in_item + i)->duration);
		if (ret != SUCC) {
			STD_ROLLBACK();
			return ret;
		}
	}
    
    STD_RETURN(ret);
}

int Croute_func::user_attire_list_mend_set(DEAL_FUN_ARG)
{
    user_attire_list_mend_set_in_header *p_in = PRI_IN_POS;
    CHECK_PRI_IN_LEN_WITHOUT_HEADER((p_in->count) * sizeof(user_attire_list_mend_set_in_item));
    user_attire_list_mend_set_in_item * p_in_item = (user_attire_list_mend_set_in_item*)(RCVBUF_PRIVATE_POS+sizeof(*p_in)) ;
    char out[PROTO_MAX_SIZE] = {};
	for (uint32_t i = 0; i < p_in->count; ++i) {
		ret = this->attire.mending_set(RECVBUF_USERID, (p_in_item + i)->attireid, (p_in_item + i)->mduration, (p_in_item + i)->duration);
		if (ret != SUCC) {
			STD_ROLLBACK();
			return ret;
		}
	}
	ret = this->user.add_xiaomee(RECVBUF_USERID, p_in->xiaomee);
	uint32_t pkglen = sizeof(*p_in) + p_in->count * sizeof(user_attire_list_mend_set_in_item);
	memcpy(&out, p_in, pkglen);
    STD_RETURN_WITH_BUF(ret, out, pkglen);
}

int Croute_func::user_title_delete(DEAL_FUN_ARG)
{
	user_title_delete_in *p_in = PRI_IN_POS; 
	ret = this->user_title.del(RECVBUF_USERID, p_in->titleid);
	STD_RETURN(ret);
}

int Croute_func::pet_in_store_list_get(DEAL_FUN_ARG)
{
	pet_in_store_list_get_in *p_in = PRI_IN_POS;

	pet_in_store_list_get_out_header  out_header;
	pet_in_store_list_get_out_item *p_out_item;

	out_header.start = p_in->start;
	this->pet.pet_cnt_get(RECVBUF_USERID,&out_header.total,10000);
	ret = this->pet.get_pet_list_in_store(RECVBUF_USERID, &p_out_item, &out_header.count,p_in->start,p_in->count);
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

int Croute_func::pet_in_store_location_set(DEAL_FUN_ARG)
{
	pet_in_store_location_set_in *p_in = PRI_IN_POS; 
	
	ret = this->pet.location_set(RECVBUF_USERID, p_in->petid_1,p_in->location_1);
	if (ret != SUCC) {
		return ret;
	}
	if (p_in->petid_2 != 0) {
		ret = this->pet.location_set(RECVBUF_USERID, p_in->petid_2, p_in->location_2);
	}
	STD_RETURN(ret);
}

int Croute_func::user_item_storage_set(DEAL_FUN_ARG)
{
	user_item_storage_set_in *p_in = PRI_IN_POS; 
	user_item_storage_set_out out = {};

	uint32_t db_cnt;
	uint32_t db_storage_cnt;
	ret = this->item.get_all_count(RECVBUF_USERID, p_in->itemid, &db_cnt, &db_storage_cnt);
	if (ret != SUCC) {
		return ret;
	}
	int cnt = (int32_t)db_cnt - p_in->count;
	int storage_cnt = (int32_t)db_storage_cnt + p_in->count;
	if ( cnt < 0 || storage_cnt < 0) {
		STD_RETURN(ITEM_STORAGE_COUNT_ERR);
	}
	ret = this->item.storage_cnt_set(RECVBUF_USERID, p_in->itemid, cnt, storage_cnt);

	memcpy(&out, p_in, sizeof(*p_in));
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::user_item_in_storage_list_get(DEAL_FUN_ARG)
{
	user_item_in_storage_list_get_out_header  out_header;
	user_item_in_storage_list_get_out_item *p_out_item;
	ret = this->item.storage_list_get(RECVBUF_USERID, &p_out_item, &out_header.count);
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

int Croute_func::user_attire_in_storage_list_get(DEAL_FUN_ARG)
{
	user_attire_in_storage_list_get_in *p_in = PRI_IN_POS;

	user_attire_in_storage_list_get_out_header  out_header;
	user_attire_in_storage_list_get_out_item *p_out_item;

	ret = this->attire.storage_list_get(RECVBUF_USERID, p_in->start, p_in->count, &p_out_item, &out_header);
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

int Croute_func::user_battle_info_set(DEAL_FUN_ARG)
{
	user_battle_info_set_in *p_in = PRI_IN_POS;
	ret = this->battle.info_set(RECVBUF_USERID, p_in);
	if (ret != SUCC) {
		ret = this->battle.insert(RECVBUF_USERID, p_in);
	}
	STD_RETURN(ret);
}

int Croute_func::home_info_get(DEAL_FUN_ARG)
{
	home_info_get_in *p_in	= PRI_IN_POS;
	home_info_get_out out = {};
	ret = this->home.info_get(p_in->homeid, &out.home_info);
	if (ret == USER_ID_NOFIND_ERR) {
		ret = this->home.insert(p_in->homeid);
		memset(&out, 0, sizeof(out));
		out.home_info.userid = p_in->homeid;
	}
	if (ret != SUCC) {
		return ret;
	}
	ret = this->user.for_home_get(RECVBUF_USERID, out.home_info.name, &out.expbox,&out.joblevel);
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::item_storage_add(DEAL_FUN_ARG)
{
    item_storage_add_in *p_in = PRI_IN_POS;
    ret = this->item.add_item_in_storage(RECVBUF_USERID, p_in->itemid, p_in->count);

	if(ret == SUCC) {
		this->vip_item_log.log(RECVBUF_USERID, p_in->itemid, p_in->count);
	}

    STD_RETURN(ret);
}

int Croute_func::mole2_user_other_user_info(DEAL_FUN_ARG)
{
    mole2_user_other_user_info_out_header  out_header = {};
    mole2_user_other_user_info_out_item *p_out_item;
    ret = this->user.get_all_info(RECVBUF_USERID, &out_header.user_info);
	if (ret != SUCC) {
		return ret;
	}
	ret = this->battle.get_int_value(RECVBUF_USERID, "all_score", &out_header.all_score);
	if(ret != SUCC) {
		out_header.all_score = 0;
	}
	ret = this->attire.get_attire_list(RECVBUF_USERID, &p_out_item, &out_header.attire_cnt);
	if(ret == SUCC && (out_header.user_info.flag & 0x101)) {
		this->user2.get_vip_info(RECVBUF_USERID,&out_header.vipinfo);
	}
    STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.attire_cnt);
}

int Croute_func::user_mail_send_set(DEAL_FUN_ARG)
{
    user_mail_send_set_in *p_in = PRI_IN_POS;
	
	uint32_t	count = 0;
	item_list_t *itm_list = (item_list_t*)p_in->items;
	if(itm_list->count > 10) {
		KDEBUG_LOG(RECVBUF_USERID,"mail items.count error");
		return VALUE_MORE_THAN_LIMIT_ERR;
	}
	DEBUG_LOG("XXX ----%u ",p_in->themeid );
	if(p_in->themeid == 10004) {
		ret = this->mail.get_theme_count(RECVBUF_USERID,10004,&count);
		if (ret == SUCC && count > 0) {
    		STD_RETURN(ret);
		}
	}

	ret = this->mail.count_get(RECVBUF_USERID, &count);
	if (ret != SUCC) {
		return ret;
	}
	DEBUG_LOG("XXX ----%u ",p_in->themeid );

	if (count > MAIL_COUNT_MAX && p_in->type < 3) {
		return MAIL_COUNT_MAX_ERR;
	}

	DEBUG_LOG("XXX ----count %u ", count);
	uint32_t now = (uint32_t)time(0);
	for (int i = 0; i < 50; ++i) {
		DEBUG_LOG("XXX ---- i %u ", i);
		now++;
    	ret = this->mail.insert(RECVBUF_USERID, now, p_in);
		if (ret == SUCC)  break;
	}
    STD_RETURN(ret);
}

int Croute_func::user_mail_simple_list_get(DEAL_FUN_ARG)
{
    user_mail_simple_list_get_out_header  out_header;
    user_mail_simple_list_get_out_item *p_out_item;
    ret = this->mail.simple_list_get(RECVBUF_USERID, &p_out_item, &out_header.count);
    STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

int Croute_func::user_exp_info_set(DEAL_FUN_ARG)
{
    user_exp_info_set_in *p_in = PRI_IN_POS;
    user_exp_info_set_out out = {};
	if (p_in->pet_levelup_info.petid) {
		ret = this->user.exp_box_set(RECVBUF_USERID, p_in->expbox);
		if (ret != SUCC) {
			return ret;
		}
		ret = this->pet.levelup_update(RECVBUF_USERID, &p_in->pet_levelup_info);
		if (ret != SUCC) {
			STD_ROLLBACK();
			return ret;
		}
	} else {
		ret = this->user.exp_info_set(RECVBUF_USERID, p_in->pet_levelup_info.experience, p_in->pet_levelup_info.level,
			p_in->pet_levelup_info.addition, p_in->pet_levelup_info.hp, p_in->pet_levelup_info.mp, p_in->expbox);
	}
	memcpy(&out, p_in, sizeof(*p_in));
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::user_mail_content_get(DEAL_FUN_ARG)
{
    user_mail_content_get_in *p_in = PRI_IN_POS;
    user_mail_content_get_out out = {};
	uint32_t flag;
    ret = this->mail.content_get(RECVBUF_USERID, p_in->mailid, out.message, out.item_list, &flag, out.id_list);
	if (ret != SUCC) {
		return ret;
	}
	if ((flag & 2) == 0) {// 2  is has read
		ret = this->mail.flag_set(RECVBUF_USERID, p_in->mailid, 2);
	}
	out.mailid	= p_in->mailid;
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::user_mail_list_del(DEAL_FUN_ARG)
{
    user_mail_list_del_in_header *p_in = PRI_IN_POS;
    CHECK_PRI_IN_LEN_WITHOUT_HEADER((p_in->count) * sizeof(user_mail_list_del_in_item));
    user_mail_list_del_in_item * p_in_item = (user_mail_list_del_in_item*)(RCVBUF_PRIVATE_POS + sizeof(*p_in)) ;
    //user_mail_list_del_out_header  out_header;
    //user_mail_list_del_out_item *p_out_item;
	char out[PROTO_MAX_SIZE];
	if (p_in->count) {
		for (uint32_t i = 0; i < p_in->count; ++i) {
    		ret = this->mail.del(RECVBUF_USERID, (p_in_item + i)->mailid);
			if (ret != SUCC) {
				STD_ROLLBACK();
				return ret;
			}
		}
	} else {
		ret = this->mail.del_all(RECVBUF_USERID);
	}
	int pkglen = 4 + p_in->count * 4;
	memcpy(out, RCVBUF_PRIVATE_POS, pkglen);
	
    STD_RETURN_WITH_BUF(ret, out, pkglen);
}

int Croute_func::user_mail_flag_set(DEAL_FUN_ARG)
{
    user_mail_flag_set_in *p_in = PRI_IN_POS;
    user_mail_flag_set_out out = {};
    ret = this->mail.flag_set(RECVBUF_USERID, p_in->mailid, p_in->flag);

	memcpy(&out, p_in, sizeof(*p_in));
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_get_val(DEAL_FUNC_ARG)
{
	mole2_get_val_out *p_out=P_OUT;

	ret=this->val.get_val(p_out->vals);
	
	return ret;
}

int Croute_func::mole2_get_val_by_id(DEAL_FUNC_ARG)
{
	mole2_get_val_by_id_in *p_in=P_IN;
	mole2_get_val_by_id_out *p_out=P_OUT;
	
	for(uint32_t loop=0 ; loop< p_in->list.size(); loop++){
        stru_id_cnt tmp;
		tmp.count=0;	
		ret=this->val.get_val(p_in->list[loop].itemid, tmp.count);
		tmp.id=p_in->list[loop].eid;
		p_out->vals.push_back(tmp);
	}
	
	return 0;
}


int Croute_func::mole2_add_val(DEAL_FUNC_ARG)
{
    mole2_add_val_in *p_in=P_IN;
    mole2_add_val_out *p_out=P_OUT;

    ret=this->val.update_val(p_in->userid, p_in->limit, p_out->val);
    p_out->userid=p_in->userid;

    return ret;
}

int Croute_func::mole2_task_set(DEAL_FUN_ARG)
{
    mole2_task_set_in *p_in = PRI_IN_POS;
    mole2_task_set_out out = {};
	ret = this->task.task_set(RECVBUF_USERID, p_in);
	if (ret == MOLE2_TASK_NOFIND_ERR) {
		ret = this->task.insert(RECVBUF_USERID, p_in);
	}
	memcpy(&out, p_in, sizeof(stru_mole2_task_info));
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::user_energy_set(DEAL_FUN_ARG)
{
    user_energy_set_in *p_in = PRI_IN_POS;
    user_energy_set_out out = {};
    ret = this->user.energy_set(RECVBUF_USERID, p_in->energy);
	out.energy = p_in->energy;
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_task_info_get(DEAL_FUN_ARG)
{
    mole2_task_info_get_in *p_in = PRI_IN_POS;
    mole2_task_info_get_out out = {};
    ret = this->task.task_get(RECVBUF_USERID, p_in->taskid, &out);
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_task_info_list_get(DEAL_FUN_ARG)
{
	char out[PROTO_MAX_SIZE] = {};
	mole2_task_info_list_get_out_header*  p_out = (mole2_task_info_list_get_out_header*)out;
	mole2_task_info_list_get_out_item_1*	p_out_1;
	mole2_task_info_list_get_out_item_2*	p_out_2;

	char* p_offset	= out + sizeof(mole2_task_info_list_get_out_header);
	int	tmplen		= 0;
	ret = this->task.on_list_get(RECVBUF_USERID, &p_out_1, &p_out->on_cnt);
	if (ret != SUCC) {
		return ret;
	}
	tmplen = p_out->on_cnt * sizeof(mole2_task_info_list_get_out_item_1);
	memcpy(p_offset, p_out_1, tmplen);
	free(p_out_1);
	p_out_1 = NULL;
	p_offset += tmplen;

	ret = this->task.other_list_get(RECVBUF_USERID, &p_out_2, &p_out->other_cnt);
	if (ret != SUCC) {
		return ret;
	}
	tmplen = p_out->other_cnt * sizeof(mole2_task_info_list_get_out_item_2);
	memcpy(p_offset, p_out_2, tmplen);
	free(p_out_2);
	p_out_2 = NULL;
	p_offset += tmplen;
	STD_RETURN_WITH_BUF(ret, out, p_offset - out);
}

int Croute_func::user_task_reward_set_cp(DEAL_FUN_ARG)
{
	user_task_reward_set_in_header *p_in = PRI_IN_POS;
	user_task_reward_set_in_item_1 * p_in_item_1  //attire in grid
		= (user_task_reward_set_in_item_1*)(RCVBUF_PRIVATE_POS+sizeof(*p_in));
	
	user_task_reward_set_in_item_2* p_in_item_2  //item
		= (user_task_reward_set_in_item_2*)(p_in_item_1 + p_in->clothes_count);
	
	user_task_reward_set_in_item_3 * p_in_item_3  //pet
		= (user_task_reward_set_in_item_3*)(p_in_item_2 + p_in->item_count);

	stru_mole2_user_title_update* p_in_item_5	//title
		=	(stru_mole2_user_title_update*)(p_in_item_3 + p_in->pet_count);
	stru_mole2_user_skill_info*		p_in_item_6  //user skill
		=   (stru_mole2_user_skill_info*)(p_in_item_5 + p_in->title_count);

	user_task_reward_set_in_item_6 * p_in_item_7  //pet
		= (user_task_reward_set_in_item_6*)(p_in_item_6 + p_in->user_skill_cnt);

	char	out[PROTO_MAX_SIZE]={0};
	
	uint32_t attire_gettime[p_in->clothes_count];


	user_task_reward_set_out_header*	p_out_header = (user_task_reward_set_out_header*)out;

	user_task_reward_set_out_item_1*	p_out_item_1;	//attire added

	CHECK_PRI_IN_LEN_WITHOUT_HEADER((p_in->clothes_count) * sizeof(user_task_reward_set_in_item_1) +
			(p_in->item_count) * sizeof(user_task_reward_set_in_item_2) +
			(p_in->pet_count) * sizeof(user_task_reward_set_in_item_3) + 
			(p_in->title_count) * sizeof(stru_mole2_user_title_update) +
			(p_in->user_skill_cnt) * sizeof(stru_mole2_user_skill_info));
//delete pet according to petid and pettype
	for (uint32_t i = 0; i < p_in->petcnt; i ++) {
		ret = this->pet.chk_pet(RECVBUF_USERID,  p_in_item_7[i].petid, p_in_item_7[i].pettype);
		if(ret != SUCC) 
			continue;
		else
			ret = this->pet.del_pet(RECVBUF_USERID, p_in_item_7[i].petid);

		if(ret != SUCC){
			STD_ROLLBACK();
			return ret;
		}
	}

//add xiaomee, exp, level, attr_addition, hp, mp
	ret = this->user.reward_set_ex(RECVBUF_USERID, p_in->xiaomee, p_in->exp, p_in->level, p_in->attr_addition,
				p_in->hp, p_in->mp, p_in->expbox, p_in->skill_expbox, p_in->flag_ex, p_in->joblevel);
//add attire
	for (uint32_t i = 0; i < p_in->clothes_count; i++) {
		ret = this->attire.attire_add(RECVBUF_USERID, (p_in_item_1 + i), &attire_gettime[i]);
		if (ret != SUCC) {
			STD_ROLLBACK();
			return ret;
		}
	}
//add item
	for (uint32_t i = 0; i < p_in->item_count; i++) {
		ret = this->item.add_item(RECVBUF_USERID, (p_in_item_2 + i)->itemid, (p_in_item_2 + i)->count);
		if (ret != SUCC) {
			STD_ROLLBACK();
			return ret;
		}
	}
//add pet
	for (uint32_t i = 0; i < p_in->pet_count; i++) {
		ret = this->pet.pet_add(RECVBUF_USERID, &(p_in_item_3 + i)->pet_info, &(p_in_item_3 + i)->gettime);	
		if (ret != SUCC) {
			STD_ROLLBACK();
			return ret;
		}
	// add pet skill
		for(uint32_t j = 0; j < (p_in_item_3 + i)->skillcnt; j++) {
			ret = this->pet_skill.add_skill(RECVBUF_USERID,p_in_item_3[i].gettime,p_in_item_3[i].pet_skill[j].skillid,p_in_item_3[i].pet_skill[j].level);
			if (ret != SUCC) {
				STD_ROLLBACK();
				return ret;
			}
		}
	}
//add title
	for (uint32_t i = 0; i < p_in->title_count; i++) {
		if (p_in_item_5->titleid_old == 0) {
			ret = this->user_title.insert(RECVBUF_USERID, p_in_item_5->titleid_new, p_in_item_5->time);
		} else if(p_in_item_5->titleid_old != 0) {
			ret = this->user_title.title_set(RECVBUF_USERID, p_in_item_5->titleid_old, p_in_item_5->titleid_new);
		}
		if (ret != SUCC) {
			STD_ROLLBACK();
			return ret;
		}
	}
/*! add user skill */
	for (uint32_t i = 0; i < p_in->user_skill_cnt; ++i) {
		ret = this->skill.add_skill(RECVBUF_USERID, p_in_item_6[i].skill_id, p_in_item_6[i].skill_lv);
		if (ret != SUCC) {
			STD_ROLLBACK();
			return ret;
		}
	}

	ret = this->task.task_set(RECVBUF_USERID, &p_in->task_in);
	if (ret != SUCC) {
		STD_ROLLBACK();
		return ret;
	}

//copy out header
	char* p_offset = out;
	
	int tmplen = sizeof(user_task_reward_set_out_header);
	memcpy(p_offset, p_in, tmplen);
	p_offset += tmplen;


//copy out attire added
	p_out_item_1 = (user_task_reward_set_out_item_1*)p_offset;
	for (uint32_t i = 0; i < p_out_header->clothes_count; i++) {		
		(p_out_item_1 + i)->gettime		= attire_gettime[i];		
		memcpy(&(p_out_item_1 + i)->attire_info, p_in_item_1 + i, sizeof(*p_in_item_1));
	}
	p_offset += p_out_header->clothes_count * sizeof(*p_out_item_1);
	
//--		
//copy out item
	tmplen = p_in->item_count * sizeof(*p_in_item_2);
	memcpy(p_offset, p_in_item_2, tmplen);
	p_offset +=  tmplen;
//--
//copy out pet
	tmplen = p_out_header->pet_count * sizeof(*p_in_item_3);
	memcpy(p_offset, p_in_item_3, tmplen);
	p_offset += tmplen;
//copy out title
	tmplen = p_out_header->title_count * sizeof(*p_in_item_5);
	memcpy(p_offset, p_in_item_5, tmplen);
	p_offset += tmplen;
//--
//copy out user skill
	tmplen = p_out_header->user_skill_cnt * sizeof(*p_in_item_6);
	memcpy(p_offset, p_in_item_6, tmplen);
	p_offset += tmplen;
//--
	STD_RETURN_WITH_BUF(ret, out, p_offset - out);
}

int Croute_func::mole2_task_ser_buf_set(DEAL_FUN_ARG)
{
    mole2_task_ser_buf_set_in *p_in = PRI_IN_POS;
    mole2_task_ser_buf_set_out out = {};
    ret = this->task.svr_buf_set(RECVBUF_USERID, p_in->taskid, p_in->ser_buf);

	memcpy(&out, p_in, sizeof(mole2_task_ser_buf_set_in));
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::rand_item_present_set(DEAL_FUN_ARG)
{
    rand_item_present_set_in *p_in = PRI_IN_POS;
    rand_item_present_set_out out = {};
	memcpy(out.strcode, p_in->strcode, sizeof(p_in->strcode));

	userid_t 	uid;
	uint32_t	count = 0;
	ret	= this->present.uid_count_get(RECVBUF_USERID, &count);
	if (count != 0) {
		return MOLE2_PRESENT_UID_EXISTS_ERR;
	}
	ret = this->present.strcode_get(p_in->strcode, &uid);
	if (ret != SUCC) {
		return ret;
	}
	if (uid != 0 ) {
		return MOLE2_PRESENT_ID_USED_ERR;
	}
	ret = this->present.update(p_in->strcode, RECVBUF_USERID);

    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_task_delete(DEAL_FUN_ARG)
{
    mole2_task_delete_in *p_in = PRI_IN_POS;
    mole2_task_delete_out out = {};
    ret = this->task.task_del(RECVBUF_USERID, p_in->taskid);

	out.taskid = p_in->taskid;
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::rand_item_activation_set(DEAL_FUN_ARG)
{
    rand_item_activation_set_in *p_in = PRI_IN_POS;
    rand_item_activation_set_out out = {};

	memcpy(out.sz_code, p_in->sz_code, sizeof(p_in->sz_code));

	userid_t 	uid;
	uint32_t	db_time	= 0;
	uint32_t	count	= 0;

	ret = this->rand_item.get_rand_info(RECVBUF_USERID, uint32_t(-1), &db_time, &count);
	if (count >= 1000) {
		return MOLE2_ACTIVATION_TRY_ERR;
	}
	
	ret = this->rand_item.insert(RECVBUF_USERID, uint32_t(-1), (uint32_t)time(0), 1);	
	if (ret == MOLE2_RAND_INFO_EXISTED_ERR) {
		ret = this->rand_item.set_rand_info(RECVBUF_USERID, uint32_t(-1), (uint32_t)time(0), count + 1);
	}	
	if (ret != SUCC) {
		return ret;
	}

	count = 0;
	ret	= this->activation.uid_count_get(RECVBUF_USERID, &count);
	if (count == 0) {
		ret = this->activation.strcode_get(p_in->sz_code, &uid);
		if (ret == SUCC) {
			if (uid == 0) {
				ret = this->activation.update(p_in->sz_code, RECVBUF_USERID);
			} else {
				ret = MOLE2_ACTIVATION_ID_USED_ERR;
			}
		}
	}
	
    STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_home_flag_set(DEAL_FUN_ARG)
{
	mole2_home_flag_set_in *p_in = PRI_IN_POS; 
	mole2_home_flag_set_out out = {};

	if(p_in->index < 31) {
		if(p_in->flag) out.flag = (1 << p_in->index);
		ret = this->home.flag_set(RECVBUF_USERID, p_in->index,out.flag);
	} else {
		ret = VALUE_OUT_OF_RANGE_ERR;
	}
	STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::set_winbossid(DEAL_FUN_ARG)
{
        set_winbossid_in *p_in = PRI_IN_POS; 
        ret = this->user.set_win_bossid(RECVBUF_USERID,p_in->bossid);
        STD_RETURN(ret);
}

int Croute_func::set_ban_flag(DEAL_FUN_ARG)
{
        set_ban_flag_in *p_in = PRI_IN_POS; 
        ret = this->user.set_ban_flag(RECVBUF_USERID,p_in->ban_flag);
        STD_RETURN(ret);
}

int Croute_func::user_swap_pets(DEAL_FUN_ARG)
{
	char out[PROTO_MAX_SIZE]={};
        user_swap_pets_in_header *p_in = PRI_IN_POS;

	int del_bytes = p_in->del_count * sizeof(user_swap_pets_in_item_1);
	int add_bytes = p_in->add_count * sizeof(user_swap_pets_in_item_2);

	CHECK_PRI_IN_LEN_WITHOUT_HEADER((del_bytes + add_bytes));

	user_swap_pets_in_item_1 *p_del = (user_swap_pets_in_item_1*)(RCVBUF_PRIVATE_POS+sizeof(*p_in)) ;
	user_swap_pets_in_item_2 *p_add = (user_swap_pets_in_item_2*)(RCVBUF_PRIVATE_POS+sizeof(*p_in) + del_bytes);

	memcpy(out,p_in,sizeof(*p_in) + del_bytes + add_bytes);
	user_swap_pets_out_item_2 *p_out_add = (user_swap_pets_out_item_2*)(out + sizeof(*p_in) + del_bytes);

	ret = SUCC;
	uint32_t i = 0;
	for(i = 0; ret == SUCC && i < p_in->del_count; i++) {
		ret = this->pet.chk_pet(RECVBUF_USERID, p_del[i].petid,p_del[i].pettype);
		if(ret != SUCC) break;
		if(p_del[i].location < STORAGE_SPLIT_END) {
			ret = this->pet.del_pet(RECVBUF_USERID, p_del[i].petid);
		} else {
        	ret = this->pet.location_set(RECVBUF_USERID, p_del[i].petid, p_del[i].location);
		}
	}

	for(i = 0; ret == SUCC && i < p_in->add_count; i++) {
		uint32_t n = 0;
		p_out_add[i].pet_info = p_add[i].pet_info;
		ret = this->pet.pet_add(RECVBUF_USERID, &p_add[i].pet_info, &p_out_add[i].petid);
		for(n = 0; ret == SUCC && n < p_add[i].skill_cnt && n < 10; n++) {
			ret = this->pet_skill.add_skill(RECVBUF_USERID, p_out_add[i].petid,
				p_add[i].skill[n].skillid, p_add[i].skill[n].level);
		}
	}

	if(ret!=SUCC) {
		STD_ROLLBACK();
	}

	STD_RETURN_WITH_BUF(ret, out, sizeof(*p_in) + del_bytes + add_bytes);
}

int Croute_func::mole2_user_get_invited(DEAL_FUN_ARG)
{
	uint32_t total = 0;
        mole2_user_get_invited_out out = {0,0};
        this->invited.get_invited(RECVBUF_USERID,&out.invited_count,&out.already_times);
	total = out.invited_count / 5;
	if(total > 0) {
		if(total > 3) total = 3;
		out.already_times = total - out.already_times;
	}
        STD_RETURN_WITH_STRUCT(SUCC, out);
}

int Croute_func::mole2_user_add_invited(DEAL_FUN_ARG)
{
        mole2_user_add_invited_out out = {0,0};
        ret = this->invited.add_invited(RECVBUF_USERID);
        STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::set_title_and_job_level(DEAL_FUN_ARG)
{
    set_title_and_job_level_in *p_in = PRI_IN_POS;
	if(p_in->title_old == 0) {
		ret = this->user_title.insert(RECVBUF_USERID,p_in->title_new,p_in->time);
	} else {
		ret = this->user_title.title_set(RECVBUF_USERID,p_in->title_old,p_in->title_new);
	}

	if(ret==SUCC && p_in->job_level) {
		ret = this->user.set_joblevel(RECVBUF_USERID,p_in->job_level);
	}
	
        STD_RETURN(ret);
}

int Croute_func::proto_get_expbox(DEAL_FUN_ARG)
{
        proto_get_expbox_out out = {};
        ret = this->user.exp_box_get(RECVBUF_USERID,&out.expbox);
        STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::proto_set_expbox(DEAL_FUN_ARG)
{
        proto_set_expbox_in *p_in = PRI_IN_POS; 
        ret = this->user.exp_box_set(RECVBUF_USERID,p_in->expbox);
        STD_RETURN(ret);
}

int Croute_func::user_attire_attrib_set(DEAL_FUN_ARG)
{
        user_attire_attrib_set_in *p_in = PRI_IN_POS; 
        ret = this->attire.set_attr(RECVBUF_USERID,p_in->gettime,p_in);
        STD_RETURN(ret);
}

int Croute_func::mole2_user_pet_get_in_store_list(DEAL_FUN_ARG)
{
        mole2_user_pet_get_in_store_list_out_header  out_header;
        mole2_user_pet_get_in_store_list_out_item *p_out_item;
        ret = this->pet.get_pet_list_in_store(RECVBUF_USERID, &p_out_item, &out_header.count,0,256);
        STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_item, out_header.count);
}

int Croute_func::mole2_user_pet_set(DEAL_FUN_ARG)
{
        mole2_user_pet_set_in *p_in = PRI_IN_POS; 
        ret = this->pet.update(RECVBUF_USERID, p_in->gettime,&p_in->pet_info);
        STD_RETURN(ret);
}

int Croute_func::mole2_user_pet_delete(DEAL_FUN_ARG)
{
        mole2_user_pet_delete_in *p_in = PRI_IN_POS; 
        ret = this->pet.del_pet(RECVBUF_USERID, p_in->petid);
        STD_RETURN(ret);
}

int Croute_func::user_get_ban_flag(DEAL_FUN_ARG)
{
        user_get_ban_flag_out out = {};
        ret = this->user.check_ban_flag(RECVBUF_USERID);
		switch(ret) {
			default:
				out.flag = 0;
				break;
			case BAN_FOREVER_ERR:
				out.flag = BAN_FOREVER;
				break;
			case BAN_14DAYS_ERR:
				out.flag = BAN_14DAYS;
				break;
			case BAN_7DAYS_ERR:
				out.flag = BAN_7DAYS;
				break;
			case BAN_24HOURS_ERR:
				out.flag = BAN_24HOURS;
				break;
		}
		ret = SUCC;
        STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_day_set_ssid_count(DEAL_FUNC_ARG)
{
    mole2_day_set_ssid_count_in *p_in=P_IN;
    uint32_t total;
    uint32_t count;
    uint32_t day;
    ret = this->day.get(userid,p_in->ssid,&total,&count,&day);
    if(ret != SUCC) {
        ret = this->day.insert(userid,p_in->ssid,p_in->count);
    } else {
        ret = this->day.set(userid,p_in->ssid, p_in->totalcnt, p_in->count);
    }
    return ret;
}

int Croute_func::day_add_ssid_count(DEAL_FUN_ARG)
{
		uint32_t total;
		uint32_t count;
		uint32_t day;

        day_add_ssid_count_in *p_in = PRI_IN_POS;
		if(p_in->limit && p_in->count > p_in->limit) {
			return VALUE_MORE_THAN_LIMIT_ERR;
		}
		ret = this->day.get(RECVBUF_USERID,p_in->ssid,&total,&count,&day);
		if(ret != SUCC) {
			ret = this->day.insert(RECVBUF_USERID,p_in->ssid,p_in->count);
		} else {
			if(day == (uint32_t)today()) {
				if(p_in->limit && p_in->count + count > p_in->limit) {
					return VALUE_MORE_THAN_LIMIT_ERR;
				} else {
					count += p_in->count;
				}
			} else {
				count = p_in->count;
			}
			ret = this->day.set(RECVBUF_USERID,p_in->ssid,total + p_in->count,count);
		}

        STD_RETURN(ret);
}

int Croute_func::day_get_ssid_info(DEAL_FUN_ARG)
{
		uint32_t day = 0;
        day_get_ssid_info_in *p_in = PRI_IN_POS; 
        day_get_ssid_info_out out = {};
		
        ret = this->day.get(RECVBUF_USERID,p_in->ssid,&out.total,&out.count,&day);
		if(ret != SUCC || day != (uint32_t)today()) {
			out.count = 0;
			ret = SUCC;
		}
        STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::day_sub_ssid_total(DEAL_FUN_ARG)
{
		uint32_t total;

        day_sub_ssid_total_in *p_in = PRI_IN_POS; 
        ret = this->day.get_int_value(RECVBUF_USERID,p_in->ssid,"total",&total);
		if(ret == SUCC) {
			if(total < p_in->count) return VALUE_NOT_ENOUGH_ERR;
			total -= p_in->count;
			ret = this->day.set_int_value(RECVBUF_USERID,p_in->ssid,"total",total);
		}

        STD_RETURN(ret);
}

int Croute_func::day_sub_ssid_count(DEAL_FUNC_ARG)
{
	uint32_t count;
	day_sub_ssid_count_in *p_in = P_IN; 
	ret = this->day.get_int_value(userid,p_in->ssid,"count",&count);
	if(ret == SUCC) {
		if(count < p_in->count) return VALUE_NOT_ENOUGH_ERR;
		count -= p_in->count;
		ret = this->day.set_int_value(userid,p_in->ssid,"count",count);
	}
	return ret;
}

int Croute_func::mail_get_post_items(DEAL_FUN_ARG)
{
        mail_get_post_items_in *p_in = PRI_IN_POS;

		uint32_t count = 0;
		char out[MAIL_ITEM_LIST_MAX] = {0};
		char buff[MAIL_ITEM_LIST_MAX] = {0};
		
		item_list_t *p_list = (item_list_t *)buff;
        ret = this->mail.get_mail_item_list(RECVBUF_USERID,p_in->mailid,p_list);
		if(ret == SUCC) {
			memcpy(out,buff,MAIL_ITEM_LIST_MAX);
			if(p_in->isrecv) {
				for(uint32_t i = 0; ret == SUCC && i < p_list->count; i++) {
					count += p_list->item[i].count;
					if(p_list->item[i].count != 0) {
						if(p_list->item[i].itmid == 50001) {
							ret = this->user.add_xiaomee(RECVBUF_USERID,p_list->item[i].count);
						} else {
							ret = this->item.add_item(RECVBUF_USERID,p_list->item[i].itmid,p_list->item[i].count);
						}
						p_list->item[i].count = 0;
					} else {
						ret = SUCC;
					}
				}

				if(ret == SUCC) {
					if(count) {
						ret = this->mail.set_mail_item_list(RECVBUF_USERID,p_in->mailid,p_list);
					}
				}

				if(ret != SUCC) {
					STD_ROLLBACK();
				}
			}
		}

		STD_RETURN_WITH_BUF(ret,out,MAIL_ITEM_LIST_MAX);
}

int Croute_func::mole2_user_get_nick(DEAL_FUN_ARG)
{
        mole2_user_get_nick_out out = {};
        ret = this->user.nick_get(RECVBUF_USERID,out.nick);
        STD_RETURN_WITH_STRUCT(ret, out);
}

int Croute_func::mole2_get_user_cli_buff(DEAL_FUNC_ARG){
	mole2_get_user_cli_buff_in* p_in=P_IN;
	mole2_get_user_cli_buff_out* p_out=P_OUT;
	ret= this->cli_buff.get_range(userid,p_in->min_id,p_in->max_id, p_out);
	return ret;
}

int Croute_func::mole2_set_user_cli_buff(DEAL_FUNC_ARG){
        mole2_set_user_cli_buff_in* p_in=P_IN;
        ret = this->cli_buff.set(userid,p_in);
        return ret;
}

#define VIP_TIME_ID_5 1000000005
#define VIP_TIME_ID_6 1000000006
#define VIP_TIME_ID VIP_TIME_ID_5 

int Croute_func::mole2_user_set_vip_info(DEAL_FUNC_ARG){
	mole2_user_set_vip_info_in* p_in=P_IN;
	uint32_t vip_activity=0; 
	ret = this->user2.set_vip_info(userid,p_in  ,&vip_activity);
	if(ret!=SUCC) return ret;
	DEBUG_LOG("vip_activity:%u",vip_activity );

	//得到天数
	vip_activity/=3600*24;
	//设置可以得到VIP物品的参数
	DEBUG_LOG("xxxxx");
	ret=this->item.add_item(userid, VIP_TIME_ID, vip_activity,NULL,false );//设置能充值vip的累计天数
	KDEBUG_LOG(userid,"2222xxxxx: ret=%d",ret);
	if(ret!=SUCC) return ret;
	ret=this->item.add_item(userid, VIP_TIME_ID_6, vip_activity,NULL,false );//设置能充值vip的天数 added in 2012-5-24
	KDEBUG_LOG(userid,"3333xxxxx: ret=%d",ret);
	if(ret!=SUCC) return ret;

	//设置vip_flag
	uint32_t vip_flag = 0;
	this->user.get_int_value(userid,this->user.str_flag,&vip_flag);
	if(p_in->vip_end_time > time(NULL)) {
		if(!(vip_flag & 0x01)) {
			vip_flag &= ~0x3C00;
			vip_flag |= 0x01;
			if (p_in->last_charge_chnl_id != 18)
				vip_flag |= 1 << 27;
			this->user.set_int_value(userid,this->user.str_flag,vip_flag);
		} else {
			vip_flag |= 0x100;
			if (p_in->last_charge_chnl_id != 18)
				vip_flag |= 1 << 27;
			this->user.set_int_value(userid,this->user.str_flag,vip_flag);
		}
	} else {
		if(vip_flag & 0x01) {
			vip_flag &= ~0x01;
			vip_flag |= 0x100;
			this->user.set_int_value(userid,this->user.str_flag,vip_flag);
		}
	}

	return ret;
}

int Croute_func::mole2_user_set_vip_level(DEAL_FUNC_ARG){
    mole2_user_set_vip_level_in* p_in=P_IN;
	ret = this->user2.set_vip_level(userid,p_in->vip_level);
    return ret;
}

int Croute_func::mole2_user_add_vip_ex_val(DEAL_FUNC_ARG){
    mole2_user_add_vip_ex_val_in* p_in=P_IN;
	ret = this->user2.add_vip_ex_val(userid,p_in->vip_ex_val);
    return ret;
}

int Croute_func::mole2_user_set_vip_base_val(DEAL_FUNC_ARG){
    mole2_user_set_vip_base_val_in* p_in=P_IN;
	ret = this->user2.set_vip_base_val(userid,p_in->growth_base);
    return ret;
}


/*得到程序的版本信息*/
int Croute_func::comm_get_dbser_version(DEAL_FUNC_ARG){
	comm_get_dbser_version_out* p_out=P_OUT;
	std::vector<std::string> file_name_list;
 	file_name_list.push_back("./libpubser.so" );
 	file_name_list.push_back("./pubser" );
 	strncpy(p_out->msg,
			get_version_str(version_str,file_name_list ).c_str(),
			sizeof (p_out->msg)  
			);
	ret=SUCC;
	return ret;
}

int Croute_func::mole2_get_user_sql(DEAL_FUNC_ARG){
	mole2_get_user_sql_in* p_in=P_IN;
	mole2_get_user_sql_out* p_out=P_OUT;

    std::string sql_str="";
    std::string tmp_sql_str="";
	
	ret=this->user.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->pet.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->attire.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->skill.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->item.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->pet_skill.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->rand_item.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->user_note.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->user_map.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->item_handbook.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->monster_handbook.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->user_title.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->task_handbook.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->battle.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->home.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->mail.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->task.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->max.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->day.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->cli_buff.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	ret=this->user2.get_insert_sql_by_userid(userid,tmp_sql_str ,"userid",p_in->obj_userid);
	sql_str+=tmp_sql_str;
	
	p_out->_sql_str_len=sql_str.size();
	memcpy(p_out->sql_str, sql_str.c_str(),p_out->_sql_str_len  );
	return ret;
}

int Croute_func::mole2_get_vip_flag(DEAL_FUNC_ARG){
    mole2_get_vip_flag_out* p_out=P_OUT;
	ret = this->user.get_int_value(userid,this->user.str_flag,&p_out->vip_flag);
	p_out->vip_flag &= 0x101;
    return ret;
}



int Croute_func::mole2_get_vip_buff(DEAL_FUNC_ARG){
    mole2_get_vip_buff_in* p_in=P_IN;
    mole2_get_vip_buff_out* p_out=P_OUT;

	ret = this->vip_buff.get_vip_buff_list(userid, p_in, p_out);

    return ret;
}

int Croute_func::mole2_set_vip_activity(DEAL_FUNC_ARG){
    mole2_set_vip_activity_in* p_in=P_IN;
	uint32_t rt;
	ret=this->user2.change_last_activity(userid, p_in->val, &rt);
    return ret;
}

int Croute_func::mole2_set_vip_activity3(DEAL_FUNC_ARG){
    mole2_set_vip_activity3_in* p_in=P_IN;
	uint32_t rt;
	ret=this->user2.change_last_activity3(userid, p_in->val, &rt);
    return ret;
}

int Croute_func::mole2_set_vip_buff(DEAL_FUNC_ARG){
    mole2_set_vip_buff_in* p_in=P_IN;

	ret = SUCC;
	size_t i = p_in->petlist.size();

	while (ret == SUCC && i--) {
		ret = this->vip_buff.set_vip_buff(userid, &p_in->petlist[i]);
	}

	if(ret != SUCC) {
		STD_ROLLBACK();
	}

    return ret;
}

int Croute_func::mole2_del_vip_buff(DEAL_FUNC_ARG){
    mole2_del_vip_buff_in* p_in=P_IN;

	ret = SUCC;
	size_t i = p_in->petid.size();

	while (ret == SUCC && i--) {
		ret = this->vip_buff.del_vip_buff(userid,p_in->petid[i]);
	}

	if(ret != SUCC) {
		STD_ROLLBACK();
	}

    return ret;
}

int Croute_func::log_vip_item(DEAL_FUNC_ARG){
    log_vip_item_in* p_in=P_IN;

	if(p_in->logtype == 0x44455355 && p_in->count > 0) {
		KERROR_LOG(userid,"USE VIP ITEM:%u %d",p_in->itemid,p_in->count);
		return VALUE_OUT_OF_RANGE_ERR;
	}
	ret = this->vip_item_log.add(p_in->logtype,p_in->itemid,p_in->count);

    return ret;
}

int Croute_func::get_rand_info_range(DEAL_FUNC_ARG){
    get_rand_info_range_in* p_in=P_IN;
    get_rand_info_range_out* p_out=P_OUT;

	ret = this->rand_item.get_rand_info_range(userid,p_in->minid,p_in->maxid,p_out);

    return ret;
}

int Croute_func::get_all_pet_type(DEAL_FUNC_ARG){
    get_all_pet_type_out* p_out=P_OUT;

	ret = this->pet.get_all_pet_type(userid,p_out);

    return ret;
}


int Croute_func::mole2_online_login(DEAL_FUNC_ARG){
	uint32_t i;
    mole2_online_login_out* p_out=P_OUT;

	ret = this->user.get_online_login(userid, p_out);
	if(ret != SUCC) return ret;	
	ret = this->battle.get_online_login(userid, p_out->battle);
	if(ret != SUCC) return ret;
	ret = this->skill.get_skills(userid, p_out->skills);
	if(ret != SUCC) return ret;
	ret = this->pet.pet_cnt_get(userid, &p_out->all_pet_cnt,0);
	if(ret != SUCC) return ret;
	ret = this->pet.get_pets(userid, p_out->pets,0,10000);
	if(ret != SUCC) return ret;
	for(i = 0; i < p_out->pets.size(); i++) {
		this->pet_skill.get_skills(userid,p_out->pets[i].gettime,p_out->pets[i].skills);
		if(ret != SUCC) return ret;
	}
	ret = this->attire.get_cloths(userid,p_out->cloths,10000,20000);
	if(ret != SUCC) return ret;
	ret = this->attire.get_cloths(userid,p_out->cloths_in_bag,0,10000);
	if(ret != SUCC) return ret;
	ret = this->item.get_items(userid,p_out->items);
	if(ret != SUCC) return ret;
	ret = this->user_title.get_titles(userid,p_out->titles);
	if(ret != SUCC) return ret;
	ret = this->task.get_tasks_done(userid,p_out->task_done);
	if(ret != SUCC) return ret;
	ret = this->task.get_tasks_doing(userid,p_out->task_doing);
	if(ret != SUCC) return ret;
	ret = this->user_map.get_mapstates(userid,p_out->mapstates);
	if(ret != SUCC) return ret;
	ret = this->monster_handbook.get_beasts(userid,p_out->beastbook,0,200000);
	if(ret != SUCC) return ret;
	ret = this->day.get_limits(userid,p_out->daylimits,0,200000);
	if(ret != SUCC) return ret;
	ret = this->vip_buff.get_buffs(userid,p_out->vipbuffs);
	if(ret != SUCC) return ret;

	ret = this->user2.get_online_login(userid, p_out->vipargs);
	if (ret == SUCC) {
		if(p_out->vipargs.end_time <= time(NULL)) {
			if(p_out->flag & 0x1) {
				DEBUG_LOG("error:vip already end[%u]",userid);
				//p_out->flag &= ~0x1;
				//p_out->flag |= 0x100;
				//this->user.set_int_value(userid,this->user.str_flag,p_out->flag);
			}
		} else {
			if(!(p_out->flag & 0x1)) {
				DEBUG_LOG("error:vip not begin[%u]",userid);
				//p_out->flag |= 0x1;
				//p_out->flag &= ~0x3C00;
				//this->user.set_int_value(userid,this->user.str_flag,p_out->flag);
			}
		}
	} else if(ret == USER_ID_NOFIND_ERR) {
		//if(p_out->flag & 0x101) {
			//p_out->flag &= ~0x101;
			//this->user.set_int_value(userid,this->user.str_flag,p_out->flag);
		//}
		ret = SUCC;
		p_out->vipargs.init();
	}

	mole2_user_user_get_offline_msg_out *msg = (mole2_user_user_get_offline_msg_out*)p_out->msglist;
	p_out->_msglist_len = msg->msglen + 8;

	if(ret == SUCC && msg->msglen) {
		mole2_user_user_get_offline_msg_out empty = {0,0,{0}};
		ret = this->user.set_offline_msg(userid,&empty);
	}

	return ret;
}

int Croute_func::mole2_reply_question(DEAL_FUNC_ARG){
    mole2_reply_question_in* p_in=P_IN;
	ret = SUCC;
	for(uint32_t i =0; ret == SUCC && i < p_in->replys.size();i++) {
		ret = this->survey.update(p_in->surveyid,p_in->replys[i]);
	}
	if(ret != SUCC) {
		STD_ROLLBACK();
	}
    return ret;
}

int Croute_func::mole2_get_survey(DEAL_FUNC_ARG){
    mole2_get_survey_in* p_in=P_IN;
	mole2_get_survey_out* p_out=P_OUT;

	ret=this->survey.get_survey(p_in->surveyid, p_out->replys);

    return ret;
}

int Croute_func::mole2_get_type_pets(DEAL_FUNC_ARG){
    mole2_get_type_pets_in* p_in=P_IN;
    mole2_get_type_pets_out* p_out=P_OUT;

	ret = this->pet.get_type_pet_count(userid,&p_out->total,p_in->pettype);
	if(ret == SUCC) {
		ret = this->pet.get_type_pets(userid, p_out->pets,p_in->start,p_in->limit,p_in->pettype);
	}
	
    return ret;
}

int Croute_func::mole2_set_levelup_time(DEAL_FUNC_ARG){
    mole2_set_levelup_time_in* p_in=P_IN;
	ret = this->user.set_int_value(userid,this->user.str_levelup_time,p_in->time);
    return ret;
}

int Croute_func::mole2_set_skill_def_level(DEAL_FUNC_ARG){
    mole2_set_skill_def_level_in* p_in=P_IN;

	if(p_in->petid) {
		ret = this->pet_skill.set_skill_use_level(userid,p_in->petid,p_in->skillid,p_in->def_level);
	} else {
		ret = this->skill.set_skill_use_level(userid,p_in->skillid,p_in->def_level);
	}
    return ret;
}

int Croute_func::mole2_user_update_rank(DEAL_FUNC_ARG)
{
	mole2_user_update_rank_in *p_in=P_IN;
	ret = this->rank.update_rank(userid,p_in);
	return ret;
}

int Croute_func::mole2_get_task_rank(DEAL_FUNC_ARG)
{
	mole2_get_task_rank_out *p_out=P_OUT;
	ret = this->rank.get_top_ten(p_out->uids);
	return ret;
}

int Croute_func::mole2_set_pet_flag(DEAL_FUNC_ARG)
{
	mole2_set_pet_flag_in * p_in=P_IN;
	ret= this->pet.set_int_value_bit(userid,p_in->petid,"flag",p_in->bit+1,p_in->state);
	return ret;
}

int Croute_func::mole2_set_flag_bits(DEAL_FUNC_ARG){
    mole2_set_flag_bits_in* p_in=P_IN;
	mole2_set_flag_bits_out* p_out=P_OUT;
	p_out->pos_id=p_in->pos_id;
	uint32_t opt_flag_id=p_in->pos_id/32;

	uint32_t opt_pos_id=p_in->pos_id%32+1;
	switch ( opt_flag_id ){
		case 0 :
			ret = this->user.set_int_value_bit(userid,this->user.str_flag,opt_pos_id , p_in->is_set_true );
			if(ret!=SUCC){ return ret; }
			ret = this->user.get_int_value(userid,this->user.str_flag,&(p_out->flag) );

			break;
		case 1 :
			DEBUG_LOG("opt_pos_id:%u,p_in->is_set_true=%u",opt_pos_id,p_in->is_set_true  );
			ret= this->item.set_int_value_bit(userid,FLAG_TYPE,this->item.str_cnt, opt_pos_id , p_in->is_set_true  );
			if(ret==USER_ID_NOFIND_ERR){
				if(p_in->is_set_true ) {
					ret= this->item.insert(userid,FLAG_TYPE,0, 1<< (opt_pos_id-1) ,0 );
				}
			}else{
				if(ret!=SUCC){ return ret; }
			}
			ret = this->item.get_int_value(userid,FLAG_TYPE ,this->item.str_cnt,&(p_out->flag) );
			break;
		default :
			break;
	}

    return ret;
}

int Croute_func::mole2_get_relation(DEAL_FUNC_ARG){
    mole2_get_relation_out* p_out=P_OUT;

	this->relation.get_relation_info(userid,p_out);

    return SUCC;
}


int Croute_func::mole2_add_relation_uid(DEAL_FUNC_ARG){
    mole2_add_relation_uid_in* p_in=P_IN;
    mole2_add_relation_uid_out* p_out=P_OUT;

	stru_relation_item item;
	mole2_get_relation_out info;

	this->relation.get_relation_info(userid,&info);
	if (info.flag && p_in->flag != info.flag) {
		return USER_RELATION_FLAG_ERR;
	}

	if ((p_in->flag == 1 && info.relations.size() == 3) ||
		(p_in->flag == 2 && info.relations.size() == 1) ) {
		return USER_RELATION_LIMIT_ERR;
	}

	uint32_t level = 0;
	this->user.get_int_value(userid,this->user.str_level,&level);
	if ((p_in->flag == 1 && (level < 10 || level > 30)) ||
		(p_in->flag == 2 && level < 30) ) {
		return USER_RELATION_LEVEL_ERR;
	}

	std::vector<stru_relation_item>::iterator it;
	for(it = info.relations.begin();it != info.relations.end();it++) {
		if(it->uid == p_in->uid) return MOLE2_ID_EXISTED_ERR;
	}

	info.flag = p_in->flag;
	item.val = 0;
	item.uid = p_in->uid;
	item.day = (uint32_t)today();
	info.relations.push_back(item);

	ret = this->relation.set_relation_info(userid,&info);

	this->user.add_friend(userid,p_in->uid);

	p_out->flag = p_in->flag;
	p_out->uid  = p_in->uid;

    return ret;
}
int Croute_func::mole2_del_relation_uid(DEAL_FUNC_ARG){
    mole2_del_relation_uid_in* p_in=P_IN;
    mole2_del_relation_uid_out* p_out=P_OUT;

	mole2_get_relation_out info;
	this->relation.get_relation_info(userid,&info);
	if (info.flag && p_in->flag != info.flag) {
		return USER_RELATION_FLAG_ERR;
	}

	std::vector<stru_relation_item>::iterator it;
	for(it = info.relations.begin();it != info.relations.end();it++) {
		if(it->uid == p_in->uid) {
			if(info.flag == 2 && p_in->op_uid == p_in->uid) {
				info.exp += it->val / 10;
				info.total_val += it->val;
			}
			info.relations.erase(it);
			ret = this->relation.set_relation_info(userid,&info);
			p_out->flag 	  = info.flag;
			p_out->exp		  = info.exp;
			p_out->graduation = info.graduation;
			p_out->total_val  = info.total_val;
			for(uint32_t i=0;i < info.relations.size();i++) {
				p_out->relations.push_back(info.relations[i]);
			}
			return ret;
		}
	}

    return MOLE2_ID_NOT_EXISTED_ERR;
}
int Croute_func::mole2_add_relation_exp(DEAL_FUNC_ARG){
    mole2_add_relation_exp_in* p_in=P_IN;

	mole2_get_relation_out info;
	this->relation.get_relation_info(userid,&info);
	if(info.flag != 2) return USER_NOT_TEACHER_ERR;


	if((int)info.exp + p_in->exp < 0) 
		return VALUE_OUT_OF_RANGE_ERR;

	info.exp = info.exp + p_in->exp;
	ret = this->relation.set_relation_info(userid,&info);

    return ret;
}
int Croute_func::mole2_add_graduation_uid(DEAL_FUNC_ARG){
    mole2_add_graduation_uid_in* p_in=P_IN;

	mole2_get_relation_out info;
	this->relation.get_relation_info(userid,&info);
	if(info.flag != 2) return USER_NOT_TEACHER_ERR;

	ret = SUCC;
	std::vector<stru_relation_item>::iterator it;
	for(it = info.relations.begin();it != info.relations.end();it++) {
		if(it->uid == p_in->uid) {
			info.graduation++;
			info.exp += it->val / 10;
			info.total_val += it->val;
			info.relations.erase(it);
			ret = this->relation.set_relation_info(userid,&info);
			break;
		}
	}

    return ret;
}
int Croute_func::mole2_reset_relation_flag(DEAL_FUNC_ARG){
	mole2_get_relation_out info;

	info.flag = 0;
	info.exp = 0;
	info.graduation = 0;
	info.total_val = 0;
	info.relations.clear();
	ret = this->relation.set_relation_info(userid,&info);

    return ret;
}

int Croute_func::mole2_add_relation_val(DEAL_FUNC_ARG){
	mole2_add_relation_val_in *p_in = P_IN;
	mole2_add_relation_val_out *p_out = P_OUT;

	mole2_get_relation_out info;

	this->relation.get_relation_info(userid,&info);
	if(info.flag != 2) return USER_NOT_TEACHER_ERR;

	if(p_in->uid == 0) {
		p_out->uid = p_in->uid;
		info.total_val += p_in->addval;
		p_out->value = info.total_val;
		ret = this->relation.set_relation_info(userid,&info);
	} else {
		ret = SUCC;
		if(p_in->addval < 0) return VALUE_OUT_OF_RANGE_ERR;
		std::vector<stru_relation_item>::iterator it;
		for(it = info.relations.begin();it != info.relations.end();it++) {
			if(it->uid == p_in->uid) {
				it->val += p_in->addval;
				p_out->value = it->val;
				p_out->uid = p_in->uid;
				ret = this->relation.set_relation_info(userid,&info);
				break;
			}
		}
	}

    return ret;
}

int Croute_func::mole2_add_skill_exp(DEAL_FUNC_ARG){
	mole2_add_skill_exp_in* p_in=P_IN;
	mole2_add_skill_exp_out* p_out=P_OUT;

	ret = this->user.change_int_value(userid,this->user.str_skill_expbox,p_in->add_exp,0x7FFFFFFF,&p_out->cur_exp);

	return ret;
}

int Croute_func::mole2_set_hero_team(DEAL_FUNC_ARG){
    mole2_set_hero_team_in* p_in=P_IN;

	std::vector < stru_team_rank_t > teams;
	
	if(p_in->teamid >= 10 || p_in->teamid == 0)
		return VALUE_OUT_OF_RANGE_ERR;

	ret = this->hero_cup.get_teams_users(teams);
	if(ret == SUCC) {
		if(teams.size()) {
			uint32_t myteam_count = 0;
			uint32_t other_min_count = 0xFFFFFFFF;

			for(uint32_t i=0;i < teams.size();i++) {
				if(teams[i].teamid == p_in->teamid) {
					myteam_count = teams[i].count;
				} else {
					if(teams[i].count < other_min_count) 
						other_min_count = teams[i].count;
				}
			}
			if(teams.size() < 4) other_min_count = 0;
			if(myteam_count > other_min_count + 1000)
				return TEAM_BALANCE_ERR;
		}
		ret = this->hero_cup.set_teamid(userid,p_in->teamid);
		if(ret == SUCC) {
			ret = this->hero_cup.add_team_users(p_in->teamid);
		}
	}

    return ret;
}

int Croute_func::mole2_add_medals(DEAL_FUNC_ARG){
    mole2_add_medals_in* p_in=P_IN;
    mole2_add_medals_out* p_out=P_OUT;

	uint32_t teamid = 10;
	if(userid <= 50000) {
		ret = this->hero_cup.get(userid,teamid,p_out->medals);
		if(ret == SUCC) {
			p_out->medals += p_in->medals;
			ret = this->hero_cup.add_medals(userid,p_in->medals);
		}
		return ret;
	}

	uint32_t i = 0;
	uint32_t mul[4] = {1,2,2,4};
	uint32_t add[4] = {1,53357,53455,55453};
	std::vector < stru_team_rank_t > ranks;

	ret = this->hero_cup.get(userid,teamid,p_out->medals);
	if(ret == SUCC) {
//		p_out->medals += p_in->medals;
//		ret = this->hero_cup.add_medals(userid,p_in->medals);
//		if(ret == SUCC) {
		if(this->hero_cup.get_team_rank(ranks) == SUCC) {
			for(i = 0; i < ranks.size();i++) {
				if(ranks[i].teamid == teamid && i > 0) {
					uint32_t x = (ranks[i-1].medals - ranks[i].medals) * 10 / add[i];
					if(x < 10) x = 10;
					if(x > mul[i] * 10) x = mul[i] * 10;
					p_in->medals = (p_in->medals * x) / 10;
					break;
				}
			}
		}
		ret = this->hero_cup.add_medals(teamid,p_in->medals);
		if(ret != SUCC) {
			STD_ROLLBACK();
		}
//		}
	}

    return ret;
}

int Croute_func::mole2_get_team_rank(DEAL_FUNC_ARG){
    mole2_get_team_rank_out* p_out=P_OUT;

	ret = this->hero_cup.get_team_rank(p_out->ranks);

    return ret;
}

int Croute_func::mole2_get_user_rank(DEAL_FUNC_ARG){
    mole2_get_user_rank_out* p_out=P_OUT;

	ret = this->hero_cup.get_user_rank(p_out->ranks);

    return ret;
}

int Croute_func::mole2_get_hero_team(DEAL_FUNC_ARG){
    mole2_get_hero_team_out* p_out=P_OUT;

	ret = this->hero_cup.get(userid,p_out->teamid,p_out->medals);
	if(ret == MOLE2_KEY_NOT_EXIST_ERR) {
		p_out->teamid = 0;
		p_out->medals = 0;
	}

    return SUCC;
}


int Croute_func::mole2_petfight_get_rank(DEAL_FUNC_ARG){
    mole2_petfight_get_rank_out* p_out=P_OUT;

    ret = this->pet_fight.get_ranks(p_out->ranks);

    return ret;
}

int Croute_func::mole2_petfight_get_user_rank(DEAL_FUNC_ARG){
    mole2_petfight_get_user_rank_out* p_out=P_OUT;

    ret = this->pet_fight.get_user_rank(userid, p_out->last_rank, p_out->rank);

    return ret;
}

int Croute_func::mole2_petfight_set_scores(DEAL_FUNC_ARG){
    mole2_petfight_set_scores_in* p_in=P_IN;

    ret = this->pet_fight.add_scores(userid, p_in->scores);

    return ret;
}

int Croute_func::mole2_user_del_title(DEAL_FUNC_ARG){
    mole2_user_del_title_in* p_in=P_IN;
	ret = this->user_title.delete_title(userid,p_in->titleid);
    return ret;
}


int Croute_func::mole2_get_day_count(DEAL_FUNC_ARG){
    mole2_get_day_count_in* p_in=P_IN;
    mole2_get_day_count_out* p_out=P_OUT;

	uint32_t day = 0;

	ret = this->day.get(userid,p_in->itemid,&p_out->total,&p_out->count,&day);
	if(ret != SUCC || day != (uint32_t)today()) {
		p_out->count = 0;
		ret = SUCC;
	}

    return ret;
}

int Croute_func::mole2_add_day_count(DEAL_FUNC_ARG){
    mole2_add_day_count_in* p_in=P_IN;

	uint32_t total = 0;
	uint32_t count = 0;
	uint32_t day = 0;

	ret = this->day.get(userid,p_in->itemid,&total,&count,&day);
	if(ret != SUCC) {
		ret = this->day.insert(userid,p_in->itemid,p_in->count);
	} else {
		if(day == (uint32_t)today()) {
			count += p_in->count;
		} else {
			count = p_in->count;
		}
		ret = this->day.set(userid,p_in->itemid,total + p_in->count,count);
	}

    return ret;
}

int Croute_func::mole2_set_pet_attributes(DEAL_FUNC_ARG) {
	mole2_set_pet_attributes_in *p_in = P_IN;
	mole2_set_pet_attributes_out *p_out = P_OUT;

	p_out->petid = p_in->petid;
	p_out->level = p_in->level;
	p_out->exp   = p_in->exp;
	p_out->physiqueinit = p_in->physiqueinit;
	p_out->strengthinit = p_in->strengthinit;
	p_out->enduranceinit = p_in->enduranceinit;
	p_out->quickinit = p_in->quickinit;
	p_out->iqinit = p_in->iqinit;
	p_out->physiqueparam = p_in->physiqueparam;
	p_out->strengthparam = p_in->strengthparam;
	p_out->enduranceparam = p_in->enduranceparam;
	p_out->quickparam = p_in->quickparam;
	p_out->iqparam = p_in->iqparam;
	p_out->physiqueadd = p_in->physiqueadd;
	p_out->strengthadd = p_in->strengthadd;
	p_out->enduranceadd = p_in->enduranceadd;
	p_out->quickadd = p_in->quickadd;
	p_out->iqadd = p_in->iqadd;
	p_out->attradd = p_in->attradd;
	p_out->hp = p_in->hp;
	p_out->mp = p_in->mp;

	ret = this->pet.set_attributes(userid, p_in);

	return ret;
}

int Croute_func::mole2_set_system_time(DEAL_FUNC_ARG) {
	mole2_set_system_time_in *p_in = P_IN;

	char str[100] = {0};
	sprintf(str,"sudo date -s \"%s\" &",p_in->datetime);
	DEBUG_LOG("TIME\t[%s]", str);
	system(str);

	return SUCC;
}

int Croute_func::mole2_user_set_shop_name(DEAL_FUNC_ARG) {
	mole2_user_set_shop_name_in *p_in = P_IN;
	ret=user.set_shop_name(userid, p_in->nick);
	return ret;
}

int Croute_func::mole2_user_get_shop_name(DEAL_FUNC_ARG) {
	mole2_user_get_shop_name_out* p_out = P_OUT;
	ret=user.get_shop_name(userid, p_out->nick);
	return ret;
}

int Croute_func::mole2_init_feed_count(DEAL_FUNC_ARG)
{
	mole2_init_feed_count_in *p_in = P_IN;
	return max.del(userid, p_in->type, p_in->itemid);
}

int Croute_func::mole2_get_all_info(DEAL_FUNC_ARG)
{
	mole2_get_all_info_out* p_out=P_OUT;
	this->user2.get_info(userid,&(p_out->user2_info));

	this->attire.get_cloths(userid,p_out->cloths , 0,0xFFFFFFFF);
	this->user_title.get_titles_ex(userid,p_out->user_title_list);
	return 0;
}

/*简单操作*/
int Croute_func::su_mole2_simple_opt(DEAL_FUNC_ARG){
	su_mole2_simple_opt_in* p_in=P_IN;
	switch ( p_in->table_type ){
		case 6: //user_title
			switch ( p_in->opt_type ){
				case 1 ://add
					this->user_title.insert( userid, p_in->v1,p_in->v2);
					break;
				case 2 ://delete
					this->user_title.del( userid, p_in->v1);
					break;
				default :
					break;
			}
			break;
		default :
			break;
	}


	return ret;
}

/*得到最近的获得的宠物列表*/
int Croute_func::su_get_last_pet_list(DEAL_FUNC_ARG){
	su_get_last_pet_list_out* p_out=P_OUT;
	ret=this->pet.get_last_pets(userid, p_out->pet_list );
	return ret;
}

// ./proto/mole2_db_db_src.cpp
//EOF
