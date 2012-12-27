/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_item_sell.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/10/2010 07:08:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

       
#ifndef  CROOMINFO_ITEM_SELL_INCL
#define  CROOMINFO_ITEM_SELL_INCL

#include "CtableRoute10x10.h"
#include "proto.h"
#include "benchapi.h"

class Croominfo_item_sell: public CtableRoute10x10 
{
	public:
		Croominfo_item_sell(mysql_interface * db);
		
	   	int get_npc_item_count(uint32_t npcid);
		int get_item_sell_max(uint32_t npcid,uint32_t itemid);
		int get_max_list(uint32_t npcid,roominfo_get_item_sell_out_item **p_out);

		int update_now(userid_t userid,uint32_t npcid);
		
		int insert_add(userid_t userid,uint32_t npcid, uint32_t itemid,uint32_t count);
		int update_add(userid_t userid,uint32_t npcid, uint32_t itemid,uint32_t count);
		int get(userid_t userid,uint32_t npcid, uint32_t itemid, uint32_t &count);
		int get_sell_list(userid_t userid,uint32_t npcid, roominfo_get_item_sell_out_item **p_out, uint32_t *count);	
};


#endif

