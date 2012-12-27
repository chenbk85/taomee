/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_user_cake.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/27/2010 02:14:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CROOMINFO_USER_CAKE_INCL
#define CROOMINFO_USER_CAKE_INCL
#include "proto.h"
#include "benchapi.h"
#include "CtableRoute10x10.h"

class Croominfo_user_cake : public CtableRoute10x10
{
	public:
		Croominfo_user_cake(mysql_interface *db);
		int add_cake(userid_t userid, uint32_t date, uint32_t cakeid);
		int update_state(userid_t userid, uint32_t cur_date, uint32_t cakeid, uint32_t state);
		int update_state_level(userid_t userid, uint32_t cur_date, uint32_t cakeid, uint32_t state, uint32_t level);
		int query_count(userid_t userid, uint32_t t_date, uint32_t* num);
		int query_count_state(userid_t userid, uint32_t t_date, uint32_t* num);
		int get_date_cake_info(uint32_t userid, uint32_t cur_date, roominfo_query_user_cake_info_out_item  **pp_items, uint32_t *p_count);
		int get_cake_info(uint32_t userid, roominfo_query_user_cake_info_out_item  **pp_items, uint32_t *p_count);
		int get_state_level(userid_t userid, uint32_t t_date, uint32_t cakeid, uint32_t& state, uint32_t& level);
};
#endif
