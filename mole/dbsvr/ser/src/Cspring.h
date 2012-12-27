/*
 * =====================================================================================
 * 
 *       Filename:  Ccup.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CSPRING_INCL
#define  CSPRING_INCL
#include "CtableRoute100.h"
#include "proto.h"

class Cspring : public CtableRoute100{
	protected:
	int get_value_db(userid_t userid, spring_stru * p_spring  );
	int insert(userid_t userid, spring_stru * p_spring );
	int update_msg_db(userid_t userid, spring_set_msg_in *p_in );
	public:
	Cspring(mysql_interface * db );
	int get_value(userid_t userid, spring_stru * p_spring  );
	int update_count(userid_t userid, spring_stru *p_in );
	int update_flag(userid_t userid, uint32_t flag );

	int get_value_ex(userid_t userid,spring_info * p_out);
	int update_msg(userid_t userid, spring_set_msg_in *p_in );
	int set_init(userid_t userid);
	int change_count(userid_t userid, spring_change_value_in * p_in,
	  spring_change_value_out *p_out	);

};

#endif   /* ----- #ifndef CSPRING_INCL  ----- */

