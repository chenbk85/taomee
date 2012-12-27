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

#ifndef  CTEMPGS_INCL
#define  CTEMPGS_INCL
#include "CtableRoute100.h"
#include "proto.h"
//怪兽活动
class Ctempgs : public CtableRoute100{
	protected:
	public:
	Ctempgs(mysql_interface * db );
	int insert(userid_t userid, stru_tempgs* p_tempgs );
	int get_value(userid_t userid, stru_tempgs* p_tempgs  );
//	int update_value(userid_t userid, tempgs_stru * p_tempgs );
	int update_task(userid_t userid, uint32_t task );
	int add_count(userid_t userid);
	int get_value_ex(userid_t userid, stru_tempgs* p_out);
};

#endif   /* ----- #ifndef CTEMPGS_INCL  ----- */

