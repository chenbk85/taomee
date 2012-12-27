/*
 * =====================================================================================
 * 
 *       Filename:  Cadmin.h
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

#ifndef  CADMIN_POWER_INCL
#define    CADMIN_POWER_INCL
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"


class Cadmin_power:public Ctable{
	private:
	public:
		Cadmin_power(mysql_interface * db ); 
		int  add_powerid( userid_t  adminid, uint32_t powerid );
		int  del_powerid( userid_t  adminid, uint32_t powerid );
		int  get_powerlist(userid_t  adminid,

			   	uint32_t *p_count, power_item ** pp_list  );
		int del_by_adminid( userid_t  adminid  );

		int  get_adminlist_by_powerid(uint32_t powerid ,
			   	uint32_t *p_count, stru_id** pp_list );

};
#endif   /* ----- #ifndef CADMIN_POWER_INCL  ----- */
