/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_new_card.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2010年06月21日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  tommychen,tommychen@taomee.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_NEW_CARD_INCL
#define  CUSER_NEW_CARD_INCL
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "Citem_change_log.h"


class Cuser_new_card:public CtableRoute100x10 
{
private:
	char *msglog_file;
	Citem_change_log *p_item_change_log;
public:
	Cuser_new_card(mysql_interface *db, Citem_change_log *p_log);
	int init_new_card(userid_t userid, user_new_card_init_card_in *p_in);
	int get_new_card_info(userid_t userid, user_new_card_get_card_info_out *p_out, uint32_t& logdate);
	int	add_new_card(userid_t userid, uint32_t cardid, uint32_t is_vip_opt_type = 0);
	int update_exp_inc(userid_t userid, uint32_t exp);
	int get_new_card_list(userid_t userid, user_cardlist *p_cardlist);
	int set_new_card_list(userid_t userid, user_cardlist *p_cardlist);
	int get_exp(userid_t userid, uint32_t &exp, uint32_t &flag);
	int get_dayexp_date(userid_t userid, uint32_t& dayexp, uint32_t& logdate);
	int update_exp_inc_set_date(userid_t userid, uint32_t exp_in, uint32_t logdate);
	int update_today_date(userid_t userid);
	int update_monster_inc(userid_t userid, uint32_t count);
	int set_flag(userid_t userid, uint32_t flag);
	int set_exp(userid_t userid, uint32_t exp);
};

#endif   /* ----- #ifndef CUSER_NEW_CARD_INCL  ----- */

