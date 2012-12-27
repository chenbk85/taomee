/*
 * =====================================================================================
 * 
 *       Filename:  Cappeal_ex.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2009年02月09日 11时36分18秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  Cappeal_ex_ex_INC
#define  Cappeal_ex_ex_INC
#include "Ctable.h"
#include "proto.h"

//申诉
class Cappeal_ex: public Ctable{
	protected:
	public:
		Cappeal_ex(mysql_interface * db ); 
		int insert( stru_appeal_ex *p_item , uint32_t *p_id) ;

		int set_deal(appeal_ex_deal_in *p_in );

		int get_state(uint32_t id, uint32_t *p_state );

		int getlist( appeal_ex_get_list_in *p_in,
				uint32_t *p_count, stru_appeal_ex ** pp_list  );

		int get_user_last_info(uint32_t userid,appeal_ex_get_user_last_state_out *p_out );
};

#endif   /* ----- #ifndef Cappeal_ex_ex_INC  ----- */
