#ifndef  GF_CGF_ROLE_H
#define    GF_CGF_ROLE_H
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"
enum ROLE_STATUS{
	ROLE_STATUS_NOR=0,
	ROLE_STATUS_BLOCK,
	ROLE_STATUS_DEL,
	ROLE_STATUS_NOR_BLOCK,
	ROLE_STATUS_ALL
};


enum{
	TYPE_ROLE_XIAOMEE=1,
	TYPE_ROLE_EXP,
	TYPE_ROLE_SKILLPOINT,
    TYPE_ROLE_FUMOPOINT,
    TYPE_ROLE_PVP_FIGHT,
    TYPE_ROLE_DOUBLE_TM,
    TYPE_ROLE_HONOR,
    TYPE_ROLE_YAOSHI_EXP,
	TYPE_ROLE_ALLOCATOR_EXP,
	TYPE_ROLE_SWORD_VALUE,
	TYPE_ROLE_VIP_QUALIFY,
	TYPE_ROLE_GF_COUPONS = 101,
};



extern const char* ROLE_XIAOMEE_COLUMN;
extern const char* ROLE_EXPLOIT_COLUMN;
extern const char* ROLE_EXP_COLUMN;
extern const char* ROLE_SKILL_POINT_COLUMN;
extern const char* ROLE_OL_COUNT;
extern const char* ROLE_FUMO_POINTS_COLUMN;
extern const char* ROLE_HONOR_COLUMN;
extern const char* ROLE_ALLOCATOR_EXP_COLUMN;
extern const char* ROLE_GF_COUPONS_COLUMN;


#define ROLE_REDUCE_VALUE(type,userid,role_regtime,err_id,value,p_left_value) do{\
			ret = this->gf_role.reduce_int_value(userid, role_regtime,err_id,ROLE_##type##_COLUMN, value, p_left_value);\
			if (ret!=SUCC)\
			{\
				return ret;\
			}\
		}while(0)\

class Cgf_role:public CtableRoute{
	public:
		Cgf_role(mysql_interface * db ); 

		int get_role_info(userid_t userid,uint32_t role_regtime, stru_role_info* p_out, stru_msg_list* p_msg);
		int get_role_partial_info(userid_t userid, uint32_t role_regtime, get_user_partial_userpart* p_info); 
		int get_role_power_user_flag(userid_t userid, uint32_t role_regtime, uint32_t* power_user);
		int get_role_info_for_special_set(userid_t userid, uint32_t role_regtime, uint32_t* p_lv, uint32_t* p_exp, uint32_t* p_upd_flg) ;
		int get_role_detail_info(userid_t userid, uint32_t role_regtime, gf_get_role_detail_info_out* p_info); 

		int add_offline_msg(userid_t userid , uint32_t role_regtime,  stru_msg_item *item );
	
		int update_home_active_point(userid_t userid , uint32_t role_regtime, uint32_t point, uint32_t last_tm);

		int update_msglist(userid_t userid , uint32_t role_regtime,  stru_msg_list * msglist );
		
		int get_msglist(userid_t userid , uint32_t role_regtime,  stru_msg_list * msglist );

		int update_nick(userid_t userid , uint32_t role_regtime, char* nick);
		int set_client_buf(userid_t userid , uint32_t role_regtime, uint8_t* buf);
		int get_client_buf(userid_t userid , uint32_t role_regtime, uint8_t* buf);
		int get_last_login_nick(userid_t userid, char* nick);

		int set_role_phy_del(userid_t userid);
		int get_role_list(userid_t userid, uint32_t* p_count, gf_get_role_list_out_item_1** pp_list);

		int get_logic_del_role_list(userid_t userid, uint32_t* p_count, gf_get_logic_del_role_list** pp_list);
		
		int get_all_role_list(userid_t userid, uint32_t* p_count, gf_get_role_list_ex_out_item_1** pp_list);
	
		int add_role(userid_t userid,uint32_t role_regtime,gf_add_role_in* p_in);

		int del_role(userid_t userid,uint32_t role_regtime);

		int get_role_status(userid_t userid,uint32_t role_regtime,uint32_t* p_status);

		int is_role_status_nor(userid_t userid,uint32_t role_regtime);

		int get_int_value(userid_t userid , uint32_t role_regtime, 
			const char * field_type ,  uint32_t * p_value);

		int set_int_value(userid_t userid, uint32_t role_regtime, const char * field_type , uint32_t  value);
		int set_int_add_value(userid_t userid, uint32_t role_regtime, uint32_t value,const char* field_type);

		int get_xiaomee_exp(userid_t userid , uint32_t role_regtime,
			uint32_t* p_xiaomee, uint32_t* p_exp);

		int get_xiaomee_exp_skpt(userid_t userid , uint32_t role_regtime,
			uint32_t* p_xiaomee, uint32_t* p_exp, uint32_t * p_allocator_exp,  uint32_t* p_skill_pt, uint32_t* fumo_pt, uint32_t* honor);

		int set_role_base_info(userid_t userid, uint32_t role_regtime, 
			gf_set_role_base_info_in_header* p_in);

        int update_unique_itembit(userid_t userid, uint32_t role_regtime, uint8_t* bit);
		int set_role_base_info_2(userid_t userid, uint32_t role_regtime, 
				gf_set_role_base_info_2_in_header* p_in);
        int set_role_pvp_info(userid_t userid, uint32_t role_regtime, 
				gf_set_role_pvp_info_in* p_in);

        int set_role_god_guard_info(userid_t userid, uint32_t role_regtime, gf_group_t *group);
		int is_role_num_max(userid_t userid);
		
		int get_role_num(userid_t userid,uint32_t status,uint32_t* p_num);

		int get_all_active_role(userid_t userid, uint32_t * p_num);

		int set_role_delflg(userid_t userid,uint32_t role_regtime, uint32_t status = 2);

		int set_role_vitality_point(userid_t userid,uint32_t role_regtime, uint32_t point);

		int gray_del_role(userid_t userid,uint32_t role_regtime);

		int logic_del_role(userid_t userid,uint32_t role_regtime);

		int resume_gray_role(userid_t userid,uint32_t role_regtime);

		int get_skill_point(userid_t userid,uint32_t role_regtime,uint32_t* p_num);

		int reduce_skill_point(userid_t userid, uint32_t role_regtime, uint32_t num, uint32_t* p_left);

		int increase_int_value(userid_t userid,uint32_t role_regtime,const char* column_type, uint32_t value);

		int increase_int_value_with_ret(userid_t userid,uint32_t role_regtime,const char* column_type, 
			uint32_t value, uint32_t* p_left);
		
		int reduce_int_value(userid_t userid,uint32_t role_regtime,uint32_t err_id,const char* column_type, 
			uint32_t value, uint32_t* p_left);

		int update_itembind(userid_t userid,uint32_t role_regtime,gf_set_role_itembind_in* p_in);

		int update_skillbind(userid_t userid, uint32_t role_regtime, const char* buf, uint32_t len);
		
		int get_itembind(userid_t userid,uint32_t role_regtime,gf_set_role_itembind_out* p_in);

		int get_skillbind(userid_t userid, uint32_t role_regtime, char* buf);
		
		int get_coin_exp(userid_t userid, uint32_t role_regtime, uint32_t* p_coins, uint32_t* p_exp);

		int get_coins(userid_t userid, uint32_t role_regtime, uint32_t* p_coins);

		int set_coins(userid_t userid, uint32_t roel_regtime, uint32_t xiaomee);

		int logout(userid_t userid,uint32_t role_regtime, gf_logout_in* p_in);
        int get_role_pvp_info(userid_t userid,uint32_t role_regtime, struct pvp_header *p_pvp);
        int get_role_Ol_last(userid_t userid,uint32_t role_regtime, uint32_t* Ol_last);
		int set_role_Ol_last(userid_t userid, uint32_t role_regtime, uint32_t Ol_last= 0);
		int del_daily_loop(userid_t userid, userid_t role_regtime);
//		int get_itembind(userid_t userid,uint32_t role_regtime,gf_get_role_itembind_out* p_out);
        int get_user_max_level(userid_t userid, uint32_t* lv);
        int get_role_double_exp_data(userid_t userid, userid_t role_regtime, uint32_t* flag,
            uint32_t *dexp_tm);
        int set_role_double_exp_data(userid_t userid, userid_t role_regtime, uint32_t flag,
            uint32_t dexp_tm);
		
		int set_role_show_state(userid_t userid, userid_t role_regtime, uint32_t state);
		
		int set_role_open_box_times(userid_t userid, userid_t role_regtime, uint32_t total_times);

		int get_role_base_info_for_boss(userid_t userid, userid_t* role_regtime, gf_get_role_base_info_for_boss_out_header* p_out);

		int get_community_info(userid_t userid, userid_t role_regtime, gf_get_player_community_info_out* p_out);
/*
		int reduce_coins(userid_t userid,uint32_t role_regtime,uint32_t use_coins,uint32_t* left_coins)
		{
			int in_ret = this->reduce_int_value(userid, role_regtime,ROLE_XIAOMEE_COLUMN, use_coins, left_coins);
			if (in_ret!=SUCC)
			{
				if (in_ret==GF_NUM_NOENOUGH_ERR)
					return GF_XIAOMEE_NOENOUGH_ERR;
				return in_ret;
			}
			return in_ret;
		}
		int reduce_exp(userid_t userid,uint32_t role_regtime,uint32_t use_coins,uint32_t* left_coins)
		{
			int in_ret = this->reduce_int_value(userid, role_regtime,ROLE_EXP_COLUMN, use_coins, left_coins);
			if (in_ret!=SUCC)
			{
				if (in_ret==GF_NUM_NOENOUGH_ERR)
					return GF_EXP_NOENOUGH_ERR;
				return in_ret;
			}
			return in_ret;
		}*/

		int get_role_offline_info(userid_t userid, userid_t role_regteime, gf_get_offline_info_out * out);
		int set_role_offline_info(userid_t userid, userid_t role_regtetime, gf_set_offline_info_in * in); 

		int get_simple_role_info(userid_t userid, uint32_t * cnt, gf_list_simple_role_info_out_item ** out);


		int get_role_need_physics_del(uint32_t userid, uint32_t* p_userid, uint32_t* p_role_tm);

		int reset_pvp_fight(userid_t userid , uint32_t role_regtime);
	private:
		uint32_t role_max_num;
		uint32_t init_xiaomee;
//		uint32_t init_hp;
//		uint32_t init_mp;
//		uint32_t init_skill_point;
//		uint32_t init_level;
//		uint32_t init_exp;

};
#endif   /* ----- #ifndef GF_CGF_ROLE_H  ----- */


