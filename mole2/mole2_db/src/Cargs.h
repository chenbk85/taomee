
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

#ifndef  CARGS_INC
#define  CARGS_INC

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"
#include <time.h>
#include "benchapi.h"
#include "db_error.h"
#include "Ccommon.h"

class Cargs : public Ctable
{
public:
	Cargs(mysql_interface * db);

	int update(userid_t userid,uint32_t key,uint32_t  value);
	int select(userid_t userid,uint32_t min,uint32_t max,get_args_out_item **pp_key,uint32_t *p_count);
};

#endif /* ----- #ifndef CPET_INC  ----- */

