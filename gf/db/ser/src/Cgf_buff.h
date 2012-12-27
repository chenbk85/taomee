#ifndef  GF_CGF_BUFF_H
#define  GF_CGF_BUFF_H
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_buff :public CtableRoute{
	public:
 		Cgf_buff(mysql_interface * db);
		int add_buff(userid_t userid, uint32_t role_time, uint32_t buff_type, 
			uint32_t duration, uint32_t mutex_type, uint32_t start_tm);
		int del_all_btl_buff(userid_t userid, uint32_t role_time);
		int clear_role_buff(userid_t userid, uint32_t role_time);
		int daily_del_buff(userid_t userid, uint32_t role_time);
		int del_buff_when_login(userid_t userid, uint32_t role_time);
		int update_buff(userid_t userid, uint32_t role_time, uint32_t buff_type, uint32_t duration);

		int get_buff_list(userid_t userid, userid_t role_regtime, uint32_t* p_count,
			gf_get_db_buff_list_out_item** pp_list);

		int del_one_buff_on_player(userid_t userid, uint32_t role_time, uint32_t buff_type);

		int del_invitee_buff(userid_t userid, uint32_t role_time);
	public:
};


#endif   /* ----- #ifndef GF_CGF_BUFF_H  ----- */

