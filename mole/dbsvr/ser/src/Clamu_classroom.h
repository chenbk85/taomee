/*
 * =====================================================================================
 *
 *       Filename:  Clamuclass.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/31/2010 05:05:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#ifndef  CLAMU_CLASSROOM_INCL
#define  CLAMU_CLASSROOM_INCL

#include <sstream>

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Clamu_classroom: public Ctable{
public:
	Clamu_classroom(mysql_interface * db );

	int insert(userid_t userid, lamu_classroom_user_create_classroom_in *p_in, lamu_classroom_user_create_classroom_out& out);
	int set_lamu_classroom_name(lamu_classroom_user_set_classroom_name_in *p_in);
	int get_lamu_classroom_exp_and_name(userid_t userid, lamu_classroom_tw_get_exp_and_name_out *p_out);
	int get_lamu_classroom_frame(uint32_t frameid, uint32_t *p_count, lamu_classroom_user_get_frame_classroom_out_item** pp_list);
	int get_lamu_classroom_count(uint32_t& count);
	int get_lamu_classroom_roomid(userid_t userid, uint32_t& roomid);
	int get_lamu_classroom_userid(userid_t room_id,  uint32_t& userid);
	int set_teacher_exp(userid_t userid, uint32_t teacher_exp);
	int set_outstand_sum(userid_t userid, uint32_t outstand_sum);
	int get_friend_rank_info(uint32_t count, lamu_classroom_get_friend_rank_in_item* p_in_item,
			        uint32_t *p_out_cnt, lamu_classroom_get_friend_rank_out_item **pp_out_item);	
	int set_exp_and_outstand_sum(userid_t userid, uint32_t exp, uint32_t outstand_sum);
};

#endif /*  ----- #ifndef CLAMUCLASS_INCL  ----- */
