/*
 * =====================================================================================
 * 
 *       Filename:  Cpet_sports.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2009年02月16日 13时40分54秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CPET_SPORTS_INC
#define  CPET_SPORTS_INC

#include "CtableRoute100.h"
#include "proto.h"


class Cpet_sports : public CtableRoute100{
	protected:

	public:
		Cpet_sports(mysql_interface * db );

		int get_petcount(userid_t userid ,uint32_t* p_count );
			
		int init(userid_t userid,uint32_t petid, uint32_t groupid);
		int	get_petlist(userid_t userid,
				uint32_t *count, temp_pet_get_pet_list_out_item** list );

		int  get_scorelist(userid_t userid,
				uint32_t *count, temp_pet_get_score_list_out_item  **pp_list);

		int  get_score(userid_t userid, uint32_t petid,  uint32_t type,
				uint32_t *p_groupid,uint32_t* p_score );

		int  set_score(userid_t userid, uint32_t petid,  uint32_t type,
				uint32_t score);

};


#endif   /* ----- #ifndef CPET_SPORTS_INC  ----- */

