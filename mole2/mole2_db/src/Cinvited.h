
/*
 * =====================================================================================
 * 
 *       Filename:  Cpet.h
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

#ifndef  CINVITED_INC
#define  CINVITED_INC

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"
#include <time.h>
#include "benchapi.h"
#include "db_error.h"
#include "Ccommon.h"


class Cinvited: public Ctable
{
public:
	Cinvited(mysql_interface * db);
	int add_invited(userid_t userid);
	int get_invited(uint32_t userid, uint32_t *p_invited, uint32_t *p_used);
};

#endif /* ----- #ifndef CINVITED_INC  ----- */
