/*
 * =====================================================================================
 * 
 *       Filename:  Cappeal.h
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

#ifndef  CAPPEAL_INC
#define  CAPPEAL_INC
#include "Ctable.h"
#include "proto.h"

//申诉
class Cappeal: public Ctable{
	protected:
	public:
		Cappeal(mysql_interface * db ); 
		int get_count(uint32_t *p_count);

		int insert( stru_appeal *p_item , uint32_t *p_id) ;
		int insert( stru_appeal *p_item );
		int getlist(appeal_get_list_in *p_in,
				uint32_t *p_count, stru_appeal ** pp_list  );

		int get(uint32_t id, stru_appeal * p_list  );

		int set_deal(uint32_t id, uint32_t state,uint32_t dealflag,uint32_t adminid );
};

#endif   /* ----- #ifndef CAPPEAL_INC  ----- */
