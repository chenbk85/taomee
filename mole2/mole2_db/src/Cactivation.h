
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

#ifndef  CACTIVATION_INC
#define  CACTIVATION_INC

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"
#include <time.h>
#include "benchapi.h"
#include "db_error.h"
#include "Ccommon.h"




class Cactivation : public Ctable
{
public:
	Cactivation(mysql_interface * db);
	
	int update(char* strcode, userid_t userid);

	int strcode_get(char* sz_strcode, userid_t* p_userid);

	int uid_count_get(uint32_t userid, uint32_t* p_count);

};

#endif /* ----- #ifndef CPET_INC  ----- */

