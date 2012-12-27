/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_ranklist.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/06/2010 03:01:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_RANKLIST
#define CSYSARG_RANKLIST

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_ranklist : public Ctable
{
public:
	Csysarg_ranklist(mysql_interface * db);
	int insert(userid_t userid, uint32_t type, uint32_t count);
	int get_ranklist(sysarg_get_ranklist_out_item **pp_list, uint32_t type, uint32_t num, uint32_t *p_count);
	int get_specify_user_count(userid_t userid, uint32_t *p_out, uint32_t type);
	//int get_specify_user_ranking(uint32_t *out, uint32_t type, uint32_t user_count);
	int exchange_present(userid_t userid,exchange_flower_to_present_in *p_in);
	int get_double_count(userid_t userid, sysarg_get_double_count_in *p_in,sysarg_get_double_count_out *out);
	//int exist_count(uint32_t *out,userid_t userid, uint32_t type);
	int get_specify_user_dynamic_count(userid_t userid, uint32_t *p_out, uint32_t type);
	int get_type_count(uint32_t *out, uint32_t type);
	int get_same_count_list(uint32_t count, uint32_t type,tmp_for_rank **out_same_list,uint32_t *num);
	int update(userid_t userid, uint32_t type, uint32_t count, uint32_t dynamic_count, uint32_t datetime);
	int get_self_ranking(userid_t userid, uint32_t *out, uint32_t type);
};
#endif 
