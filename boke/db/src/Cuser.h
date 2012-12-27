/*
 * =========================================================================
 *
 *        Filename: Cuser.h
 *
 *        Version:  1.0
 *        Created:  2011-05-13 15:29:51
 *        Description:  
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */
#ifndef  CUSER_H
#define  CUSER_H

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
//flag 0x01 :是否已经注册  

class Cuser:public CtableRoute100x10
{
private:
public:
	Cuser (mysql_interface * db);
	int get_all_info(userid_t userid  ,   user_base_info_t* p_out);
	int get_last_online_id(userid_t userid, uint32_t& last_online_id);
	int	del(userid_t userid );
	int	insert(userid_t userid, pop_reg_in *p_in );
	int	set_nick(userid_t userid,  pop_set_nick_in *p_in );
	int	set_flag(userid_t userid,  pop_set_flag_in *p_in );
	int	set_color(userid_t userid,  pop_set_color_in *p_in );
	int	update_logout_info(userid_t userid, pop_logout_in *p_in );
	int	update_reg(userid_t userid, pop_reg_in *p_in );
	int	set_field_value(userid_t userid,  pop_user_set_field_value_in *p_in );
	int	change_xiaomee(userid_t userid, int change_value );

};

#endif  /*CUSER_H*/
