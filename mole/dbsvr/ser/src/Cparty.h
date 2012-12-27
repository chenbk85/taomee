/*
 * =====================================================================================
 * 
 *       Filename:  Cparty.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CPARTY_INCL
#define  CPARTY_INCL
#include "CtableDate.h"
#include "proto.h"
class Cparty : public Ctable{
	protected:
		int add_db(party_add_party_in  *p_item  );
		int check_count(uint32_t partydate, uint32_t  serverid);
	public:
		Cparty(mysql_interface * db ); 

		int get_count(uint32_t partydate, uint32_t  serverid,uint32_t *p_count );
		int add(party_add_party_in  *p_item  );
		int get_partylist(uint32_t serverid,  uint32_t index , uint32_t pagecount  ,
	 		uint32_t *count, party_get_partylist_out_item ** pp_list );
 
		int get_partylist_by_ownerid(uint32_t ownerid, 
	 		uint32_t *count, party_getlist_by_ownerid_out_item ** pp_list );

		int get_count_by_time(uint32_t partytime, uint32_t  serverid,uint32_t *p_count );
};

#endif   /* ----- #ifndef CPARTY_INCL  ----- */

