/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_prob_month_lahm.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/11/2012 10:21:18 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_PROB_MONTH_LAHM_INCL
#define CSYSARG_PROB_MONTH_LAHM_INCL


#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_prob_month_lahm: Ctable
{
	public:
		Csysarg_prob_month_lahm(mysql_interface *db);
		int insert(userid_t userid, uint32_t datetime, char* nick);
		int get_count(userid_t userid, uint32_t *count);
		int get_all(prob_super_lahm_t **pp_list, uint32_t* p_count);

};
#endif
