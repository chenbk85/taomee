/*
 * =====================================================================================
 * 
 *       Filename:  Cdiningroom.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2010年03月30日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  tommychen@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CDININGROOM_INCL
#define  CDININGROOM_INCL
#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Cdiningroom: public Ctable{
	public:
		Cdiningroom(mysql_interface * db );
		int insert(userid_t userid, diningroom_user_create_diningroom_in *p_in, diningroom_user_create_diningroom_out& out);
		int set_room_name(diningroom_user_set_room_name_in *p_in);
		int set_room_style(diningroom_user_set_room_style_in *p_in);
		int get_frame_diningrooms(uint32_t frameid , uint32_t *p_count, diningroom_user_get_frame_diningroom_out_item** pp_list);	
		int get_diningroom_count(uint32_t& count);	
		int get_diningroom_min_frame(uint32_t& frame_id);	
		int get_diningroom_roomid(userid_t userid,  uint32_t& roomid);	

};

#endif   /* ----- #ifndef CDININGROOM_INCL  ----- */

