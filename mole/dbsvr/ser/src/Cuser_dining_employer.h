/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_dining_employer.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2010年03月09日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  tommychen, tommychen@taomee.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_DINING_EMPLOYER_INCL
#define  CUSER_DINING_EMPLOYER_INCL
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_dining_employer: public CtableRoute100x10 {
public:
	Cuser_dining_employer(mysql_interface * db);

	int add_employer(userid_t userid, user_dining_add_employer_in* p_in); 
	
	int del_employer(userid_t userid, user_dining_del_employer_in* p_in); 

	int get_user_employer(userid_t userid, user_dining_employer_item **pp_list, uint32_t* p_count);
	
	int get_user_employer_count(userid_t userid, uint32_t& num);
	
	int add_employer_work_for_other(userid_t userid, user_dining_user_work_for_other_in* p_in); 
	
	int get_sys_employer_list(userid_t userid,  user_dining_get_sys_employer_list_out_item  **pp_list, user_dining_get_sys_employer_list_out_item  **pp, uint32_t *p_count, uint32_t *count_p);

	int get_sys_employer_xiaomee(uint32_t level, uint32_t& xiaomee);

	int get_em_user_employer_time(userid_t userid, userid_t em_userid, uint32_t em_petid, uint32_t& time,uint32_t &level, uint32_t &skill);

};

#endif   /* ----- #ifndef CUSER_DINING_EMPLOYER_INCL  ----- */

