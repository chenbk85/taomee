/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_temp_item.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 13时47分01秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_TEMP_ITEM_INCL
#define  CUSER_TEMP_ITEM_INCL

#include "CtableRoute100x10.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"
#include "Citem_change_log.h"

class Cuser_temp_item:public CtableRoute100x10
{
private:
	char *msglog_file;
	Citem_change_log *p_item_change_log;
	int inner_add(userid_t userid, uint32_t itemid,uint32_t count,uint32_t limit);
	int inner_sub(userid_t userid, uint32_t itemid, uint32_t count);
public:
	Cuser_temp_item(mysql_interface * db, Citem_change_log *p_log ); 

	int get_count(userid_t userid, uint32_t itemid, uint32_t &count);
	int add_count(userid_t userid, uint32_t itemid, uint32_t count,uint32_t limit, uint32_t is_vip_opt_type = 0);
	int sub_count(userid_t userid, uint32_t itemid, uint32_t count, uint32_t is_vip_opt_type = 0);
	int get_items_count(userid_t userid, uint32_t start, uint32_t end, uint32_t *count, attire_count** list);
};

#endif   /* ----- #ifndef CUSER_TEMP_ITEM_INCL  ----- */
