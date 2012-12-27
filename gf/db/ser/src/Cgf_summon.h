#ifndef  __GF_CGF_SUMMON_H__
#define  __GF_CGF_SUMMON_H__
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

struct gf_get_summon_nick_list_out_item {
	char nick[NICK_LEN];
};


class Cgf_summon:public CtableRoute{
	public:
		Cgf_summon(mysql_interface * db ); 

        int get_summon_list(userid_t userid, uint32_t usertm, uint32_t* p_count, gf_get_summon_list_out_item** pplist);

		int add_summon(userid_t userid,uint32_t usertm, uint32_t mon_tm, uint32_t mon_type, uint32_t status, char* mon_nick);

		int del_summon(userid_t userid,uint32_t usertm, uint32_t mon_tm);

		int update_summon_nick(userid_t userid,uint32_t usertm, uint32_t mon_tm, char* nick);

        int get_int_value(userid_t userid, uint32_t usertm, uint32_t mon_tm, const char* column_type, uint32_t* db_num);

		int get_role_summon_cnt(userid_t userid, uint32_t usertm, gf_get_player_community_info_out* p_out);

		int get_summon_cnt(userid_t userid, uint32_t usertm, const char* str_where, uint32_t* db_num);
		
        int set_int_value(userid_t userid, uint32_t usertm, uint32_t mon_tm, const char* column_type, uint32_t value);

        int increase_int_value(userid_t userid, uint32_t usertm, uint32_t mon_tm,const char* column_type, uint32_t value, uint32_t max_val,uint32_t* p_left);

        int reduce_int_value(userid_t userid, uint32_t usertm, uint32_t mon_tm,uint32_t err_id,const char* column_type, uint32_t value, uint32_t* p_left);
        
        int get_fight_summon(userid_t userid, uint32_t usertm, uint32_t* mon_tm);

        int set_summon_status(userid_t userid, uint32_t usertm, uint32_t mon_tm, uint32_t status);

        int callback_fight_summon(userid_t userid, uint32_t usertm);
        int set_summon_property(userid_t userid, uint32_t usertm, gf_set_summon_property_in* p_in);

		int clear_role_summon(userid_t userid,uint32_t usertm);
        int set_summon_exp_lv(userid_t userid,uint32_t usertm, uint32_t mon_tm,uint32_t add_exp, uint32_t lv);

		int fresh_summon_attr_add(userid_t userid, uint32_t usertm, uint32_t mon_tm, 
				uint32_t mon_type, int attr_per);

        int fresh_summon_attr_add_ex(userid_t userid, uint32_t usertm, uint32_t mon_tm,
				uint32_t mon_lv, uint32_t mon_exp, int attr_per); 
		int reset_off_summon(userid_t userid, uint32_t usertm) ;

		int get_summon_nick_list(userid_t userid, uint32_t usertm, uint32_t* p_count, gf_get_summon_nick_list_out_item** pp_list);

		int get_last_summon_nick(userid_t userid, uint32_t usertm, char* nick);

		int clear_role_info(userid_t userid,uint32_t role_regtime);

		int get_offline_summon(userid_t userid, uint32_t usertm, uint32_t * mon_tm);
	private:

};
#endif   /* ----- #ifndef __GF_CGF_SUMMON_H__  ----- */
