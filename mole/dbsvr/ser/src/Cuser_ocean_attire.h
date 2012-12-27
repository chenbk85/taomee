/*
 * =====================================================================================
 *
 *       Filename:  Cuser_ocean_attire.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/23/2012 03:17:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_OCEAN_ATTIRE_INCL
#define CUSER_OCEAN_ATTIRE_INCL


#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"
#include "Citem_change_log.h"

class Cuser_ocean_attire:public CtableRoute100x10
{
	private:

		char *msglog_file;
		Citem_change_log *p_item_change_log;

		int inner_add(userid_t userid, uint32_t itemid, uint32_t count, uint32_t limit);
		int inner_sub(userid_t userid, uint32_t itemid, uint32_t count);
		int add_attire_msglog(userid_t userid, uint32_t itemid, uint32_t count, uint32_t type);

	public:
		Cuser_ocean_attire(mysql_interface *db, Citem_change_log *p_log);
		int get_count(userid_t userid, uint32_t itemid, uint32_t &count);
		int add_count(userid_t userid, uint32_t itemid, uint32_t count ,uint32_t limit, uint32_t is_vip_opt_type = 0);
		int sub_count(userid_t userid, uint32_t itemid, uint32_t count, uint32_t is_vip_opt_type = 0);
		int get_noused_count(userid_t userid, uint32_t itemid, uint32_t &count);
		int remove(userid_t userid, uint32_t itemid);
		int get_items_range(userid_t userid, uint32_t start, uint32_t end, uint8_t usedflag, 
				uint32_t *count, attire_count ** list);	
		int get_interval_item(userid_t userid, uint32_t *p_in_item, uint32_t in_count, 
		attire_count** pp_out_item, uint32_t *out_count);
		int check_items_enough(userid_t userid, uint32_t* material_items, uint32_t in_count, uint32_t* material_count);
		int take_off(userid_t userid, uint32_t itemid);
		int put_on(userid_t userid, uint32_t itemid);

};

#endif
