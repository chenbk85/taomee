
/*
 * =====================================================================================
 * 
 *       Filename:  Cuser.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  g++
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER2_INC
#define  CUSER2_INC
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser2:public CtableRoute100x10
{
private:
	int update(userid_t userid,mole2_user_vip_info *info);
public:
	Cuser2 (mysql_interface * db);

	int set_vip_level(userid_t userid,uint32_t vip_level);
	int add_vip_ex_val(userid_t userid, uint32_t vip_ex_val);
	int set_vip_base_val(userid_t userid, uint32_t vip_base_val);
	int change_last_activity(userid_t userid, int32_t change_val, uint32_t *rt_val);
	int change_last_activity3(userid_t userid, int32_t change_val, uint32_t *rt_val);
	int get_vip_info(userid_t userid,mole2_user_vip_info *info);
	int get_info(userid_t userid, user2_info_t*p_out);
	int set_field_value(userid_t userid, su_mole2_set_field_value_in *p_in);

	int set_vip_info(userid_t userid,mole2_user_set_vip_info_in *p_in, uint32_t * p_vip_activity=NULL );
	int get_online_login(userid_t userid,stru_vip_args &p_out);

};

#endif /* ----- #ifndef CUSER_INC  ----- */
