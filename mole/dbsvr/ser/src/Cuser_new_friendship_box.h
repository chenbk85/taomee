/*
 * =====================================================================================
 *
 *       Filename:  Cuser_new_friendship_box.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/13/2012 10:29:16 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#ifndef  CUSER_NEW_FRIENDSHIP_BOX_INCL
#define  CUSER_NEW_FRIENDSHIP_BOX_INCL
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_new_friendship_box : public CtableRoute100x10 {
public:
	Cuser_new_friendship_box(mysql_interface * db); 

	int init_record(userid_t userid);
	int get_item_list(userid_t userid,get_friend_box_items_out *p_out);
	int set_item_list(userid_t userid,get_friend_box_items_out *p_out);
	int get_history_list(userid_t userid,get_friend_box_history_out *p_out);
	int set_history_list(userid_t userid,get_friend_box_history_out *p_out);
	int get_share_src_list(userid_t userid,uint32_t &date,share_src_t *p_out);
	int set_share_src_list(userid_t userid,uint32_t date,share_src_t *p_out);
};

#endif   /* ----- #ifndef CUSER_CARD_INCL  ----- */

