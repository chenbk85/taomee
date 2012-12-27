/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_contribute_rank.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/09/2010 07:10:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_CONTRIBUTE_XIAOMEE_INCL
#define CSYSARG_CONTRIBUTE_XIAOMEE_INCL

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_contribute_rank : Ctable
{
public:
	Csysarg_contribute_rank(mysql_interface * db);
	int insert(userid_t userid, uint32_t , uint32_t , uint32_t );
	int update(userid_t userid, const char *type, uint32_t);
	int get_rank(sysarg_contribute_get_rank_out_item **pp_list, const char *field, uint32_t *p_count);
	int get_user_count(sysarg_contribute_get_user_count_out *p_out);
	int get_type_count(const char *filed, uint32_t *p_count); 
};

#endif /***** #ifndef CSYSARG_CONTRIBUTE_XIAOMEE_INCL *****/
