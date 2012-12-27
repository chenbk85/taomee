#ifndef  GF_CGONGFU_H
#define  GF_CGONGFU_H

//#include <serverbench/benchapi.h>

#include "CtableRoute100x10.h"
#include "proto.h"

#pragma pack(1)

struct stru_xiaomee_info {
    uint32_t xiaomee;
    uint32_t xiaomee_max;
    uint32_t xiaomee_changetime;
};

#pragma pack()

class Cgongfu :public CtableRoute100x10 {
	
	private:
	public:
		Cgongfu(mysql_interface * db ); 
		int init(userid_t userid,   gf_reg_in  *p_in );
		//int test(userid_t userid);
		int get_user_info(userid_t userid, uint32_t role_regtime, stru_gf_info_out *p_out, uint32_t* flag, stru_msg_list* p_msg);
		int update_nick(userid_t userid, char* nick);

//		int add_friend(userid_t userid , userid_t id );

//		int get_friend_black_list(userid_t userid, uint32_t* p_flag, 
//				struct stru_id_list* p_friendlist,
//				struct stru_id_list* p_blacklist);

		int update_idlist(userid_t userid, const char* id_flag_str, struct stru_id_list* p_idlist);
		int del_id(userid_t userid, const char* id_flag_str, userid_t id);
		int get_idlist(userid_t userid, const char* id_flag_str, struct stru_id_list* p_list);

		int get_vip_type(userid_t userid,uint32_t* p_vip_type);

		int update_list_nickname(uint32_t count, gf_get_friendlist_out_item* p_list);

		int get_nick_name(userid_t userid, char* nick);

		int add_offline_msg(userid_t userid ,  stru_msg_item *item );

		int update_msglist(userid_t userid ,  stru_msg_list * msglist );

		int get_msglist(userid_t userid ,  stru_msg_list * msglist );

		int get_user_base_info(userid_t userid, gf_get_user_base_info_out_header *p_out);
		int update_user_base_info(userid_t userid, gf_set_user_base_info_in * p_in);

		int is_user_existed(userid_t userid,bool* is_existed, uint32_t* regtime);

		int is_user_existed_for_boss(userid_t userid,bool* is_existed);

		int add_user(userid_t userid, uint32_t parentid);

		int add_user_for_boss(userid_t userid);

		int update_onlinetime(userid_t userid, uint32_t onlinetime);

        int get_amb_status(userid_t userid, uint8_t* amb_status);
        int get_user_gfcoin(userid_t userid, uint32_t* gfcoin);
        int get_amb_achieve_count(userid_t userid, uint32_t* cnt);
        int update_user_count(userid_t userid, const char* row_name);
        int update_amb_status(userid_t userid, uint8_t status);
        int update_amb_reward_flag(userid_t userid, uint8_t buf[40]);
        int get_vip_info(userid_t userid, gf_get_user_vip_out* out);
        int get_necessary_info_for_login(userid_t userid, gf_user_self_define* out);
        int set_vip(userid_t userid, gf_set_vip_in* p_in);
        int set_int_add_value(userid_t userid, uint32_t add_value, const char* row_name);
        int set_int_value(userid_t userid, uint32_t value, const char* row_name);
        int set_vip_exvalue(userid_t userid, gf_set_vip_exinfo_in* p_in);
        int update_account_forbid(userid_t userid, uint32_t flag, uint32_t limit_tm);
        int get_act_record(userid_t userid, uint8_t *buf);
        int set_act_record(userid_t userid, uint8_t buf[40]);
        int get_int_value(userid_t userid, const char* row_name, uint32_t* value);
		int get_parent_id(userid_t userid, userid_t* p_id);
};


#endif   /* ----- #ifndef CUSER_INC  ----- */

