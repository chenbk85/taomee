/*
 * =====================================================================================
 * 
 *       Filename:  Chope_history.h
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

#ifndef  CHOPE_HISTORY_INCL
#define  CHOPE_HISTORY_INCL
#include "Ctable.h"
#include "proto.h"

class Chope_history : public Ctable{
	protected:
	public:
		int insert( hope_add_hope_in *p_in  );
		Chope_history(mysql_interface * db );
		int get_hope_list(userid_t recv_id, 
		uint32_t *p_count, hope_get_hope_list_out_item ** pp_list   );

		int get_hope_list_ex(userid_t recv_id, 
		uint32_t *p_count, hope_get_hope_all_out_item ** pp_list   );

		int set_useflag(userid_t send_id, userid_t recv_id,
	  	uint32_t hope_date );
		int check_today_hope(userid_t send_id );

		int get_hope(userid_t send_id,  userid_t recv_id , uint32_t hopedate,
	  	hope_set_hope_used_out * p_out );

		int get_hope_list_by_date(uint32_t hopedate,uint32_t index ,
		uint32_t *p_count, hope_get_hope_by_date_out_item ** pp_list   );
		int set_ex(userid_t send_id,  userid_t recv_id , uint32_t old_hopedate,
	   	uint32_t new_hopedate, uint32_t recv_type );

};

#endif   /* ----- #ifndef CHOPE_HISTORY_INCL  ----- */

