/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_articles.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/20/2011 01:56:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_BATTLE_ARTICLES_INC
#define CUSER_BATTLE_ARTICLES_INC

#include <vector>
#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"
#include "Citem_change_log.h"

class Cuser_battle_articles:public CtableRoute100x10
{
	private:
		char *msglog_file;
		Citem_change_log *p_item_change_log;

		int inner_add(userid_t userid, uint32_t itemid, uint32_t count, uint32_t limit);
		int inner_sub(userid_t userid, uint32_t itemid, uint32_t count);
		int remove(userid_t userid, uint32_t itemid);
	public:
		Cuser_battle_articles(mysql_interface *db, Citem_change_log *p_log);
		int get_count(userid_t userid, uint32_t itemid, uint32_t &count);
		int add_count(userid_t userid, uint32_t itemid, uint32_t count ,uint32_t limit, uint32_t is_vip_opt_type = 0);
		int sub_count(userid_t userid, uint32_t itemid, uint32_t count, uint32_t is_vip_opt_type = 0);
		int get_noused_count(userid_t userid, uint32_t itemid, uint32_t &count);
		int get_items_range(userid_t userid, uint32_t start, uint32_t end, uint8_t usedflag, 
				uint32_t *count, attire_count ** list);	
		int add_achievement_level(userid_t userid, uint32_t new_level, uint32_t old_level, 
				uint32_t *count, user_battle_over_info_out_item_1* &p_offset);
		int get_pass_boss_achivement_card(userid_t userid, uint32_t pass_count, uint32_t comp_degree, uint32_t barrier_id, 
				uint32_t *count, user_battle_over_info_out_item_1* &p_offset);
};
#endif
