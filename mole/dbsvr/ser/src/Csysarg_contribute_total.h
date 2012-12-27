/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_contribute_total.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/10/2010 03:25:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef CSYSARG_CONTRIBUTE_TOTAL_INCL
#define CSYSARG_CONTRIBUTE_TOTAL_INCL

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_contribute_total : Ctable
{
public:
	Csysarg_contribute_total(mysql_interface * db);
	int get_contribute_total(sysarg_contribute_get_user_count_out* p_out);
	int update(uint32_t add_user, uint32_t add_xiaomee, uint32_t add_attire);
};

#endif /*****  #ifndef CSYSARG_CONTRIBUTE_TOTAL_INCL  *****/
