/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_mdog.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2010年03月09日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  tommychen, tommychen@taomee.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_MDOG_INCL
#define  CUSER_MDOG_INCL
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "Citem_change_log.h"

class Cuser_mdog: public CtableRoute100x10 {
private:
	Citem_change_log *p_item_change_log;
public:
	Cuser_mdog(mysql_interface * db, Citem_change_log *p_log); 

	int insert(userid_t userid, uint32_t mdog); 
	int add(userid_t userid, uint32_t mdog, uint32_t is_vip_opt_type = 0);

	int get_mdog_info(uint32_t userid, uint32_t mdog, user_mdog_query_mdog_info_out& out);
	
	//int get_mdog(uint32_t userid, uint32_t mdog);
	int get_mdog(uint32_t userid, uint32_t mdog, uint32_t &count);

	int get_mdog_count(uint32_t userid, uint32_t mdog, user_mdog_query_user_mdog_out& out);

	int del(userid_t userid, uint32_t mdog);

	int get_mdog_end_time(userid_t userid, uint32_t mdog, uint32_t& end_time);

	int set_end_time(userid_t userid, uint32_t mdog, uint32_t end_time);

	int update_kill_water(userid_t userid, uint32_t mdog, uint32_t insent_kill, uint32_t plant_water);

	int update_drink_catch(userid_t userid, uint32_t mdog, uint32_t animal_drink, uint32_t animal_catch);

	int update_flag(userid_t userid, uint32_t mdog, uint32_t flag);
};

#endif   /* ----- #ifndef CUSER_MDOG_INCL  ----- */

