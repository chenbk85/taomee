/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_beauty_contend.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/22/2011 09:57:04 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_BEAUTY_CONTEND_INCL
#define CSYSARG_BEAUTY_CONTEND_INCL


#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_beauty_contend:Ctable
{
	public:
		Csysarg_beauty_contend(mysql_interface *db);
		int insert(uint32_t value_1, uint32_t value_2, uint32_t value_3, uint32_t total);
		int get_all(uint32_t *value_1, uint32_t *value_2, uint32_t *value_3, uint32_t *total);
		int update_two_col_inc(const char* col_1, const char* col_2, uint32_t value_1, uint32_t value_2);
		int add(sysarg_join_beauty_contend_in *p_in);
		int get_total(uint32_t *total);
		int update_two_cols(const char* col_1, const char* col_2, uint32_t value_1, uint32_t value_2);
		int get_two_cols(const char* col_1, const char* col_2, uint32_t *value_1, uint32_t *value_2);
};

#endif
