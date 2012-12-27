/*
 * =====================================================================================
 * 
 *       Filename:  Ccup.h
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

#ifndef  CCHRISTMAS_INCL
#define  CCHRISTMAS_INCL
#include "CtableRoute10.h"
#include "proto.h"

class Cchristmas : public CtableRoute10{
	protected:
		int get_msg_db(userid_t userid, christmas_get_msg_out * p_out  );
	public:
	Cchristmas(mysql_interface * db );
	int add(userid_t userid, christmas_msg* p_in  );
	int get_msg(userid_t userid, christmas_get_msg_out * p_out  );
	int is_get_msg(userid_t userid, stru_is_seted *p_is_existed ) ;
	int set_isget_msg(userid_t userid);
	int set_msg_lost(userid_t userid);
};

#endif   /* ----- #ifndef CCHRISTMAS_INCL  ----- */

