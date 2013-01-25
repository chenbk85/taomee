
/*
 * =====================================================================================
 * 
 *       Filename:  Cuser.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  g++
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CRELATION_INC
#define  CRELATION_INC
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Crelation:public CtableRoute100x10
{
public:
	Crelation (mysql_interface * db);

	int get_relation_info(uint32_t userid,mole2_get_relation_out *info);
	int set_relation_info(uint32_t userid,mole2_get_relation_out *info);
};

#endif /* ----- #ifndef CRELATION_INC  ----- */
