/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_auction.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/09/2010 03:32:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef CSYSARG_AUCTION_INCL
#define CSYSARG_AUCTION_INCL

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_auction : Ctable
{
public:
	Csysarg_auction(mysql_interface * db);
	int auction_begin(uint32_t& attire_id, uint32_t& attire_cnt);
	int get_attireid_and_time(uint32_t& attireid, uint32_t& end_time, uint32_t& attire_cnt);
	int auction_end(sysarg_auction_end_auction_out_item **p_list, sysarg_auction_end_auction_out_header* out_header);
	int add_auction_record(userid_t userid, int32_t xiaomee);

protected:
	int insert(userid_t userid, int32_t xiaomee);
	int get_value(uint32_t, uint32_t*);
	int update_value(uint32_t id, uint32_t value);
	int get_success_auction_list(sysarg_auction_end_auction_out_item **p_list, uint32_t *p_count, uint32_t item_price);
	int check_in_auction_time();
};

#endif /***** ----end #ifndef CSYSARG_AUCTION_INCL *****/
