/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_love_auction.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/26/2011 01:23:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alexhe, alexhe@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_LOVE_AUCTION_INCL
#define CSYSARG_LOVE_AUCTION_INCL

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_love_auction : Ctable
{
public:
	Csysarg_love_auction(mysql_interface * db);
	
	int get_auction_list(uint32_t *count, sysarg_get_auction_list_out_item** pp_list, uint32_t is_old_flag, uint32_t* start_time);
	int renew_auction_list(uint32_t count, contribute_info* plist);
	int get_auction_item(uint32_t itemid, auction_list_info* p_out);
	int set_auction_item(uint32_t itemid, uint32_t userid, uint32_t money, char* auction_nick);
private:
	int insert_item(uint32_t itemid, uint32_t contri_userid, char* nick, uint32_t cur_time);
	int delete_item();
	int set_old_flag();
};
#endif
