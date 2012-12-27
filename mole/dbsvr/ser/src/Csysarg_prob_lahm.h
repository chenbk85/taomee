/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_prob_lahm.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/10/2012 04:16:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSAEG_PROB_LAHM_INCL
#define CSYSAEG_PROB_LAHM_INCL


#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_prob_lahm: Ctable
{
	public:
		Csysarg_prob_lahm(mysql_interface *db);
		int insert(uint32_t userid, char* nick, uint32_t reg_time, uint32_t datetime );
		int update_one_col(const char* col, uint32_t value);
		int get_one_col(const char* col, uint32_t *value);
		int get_all(uint32_t *super, uint32_t *ret_time, char* nick);
		int update_all(userid_t super, uint32_t reg_time, char* nick);
};
#endif
