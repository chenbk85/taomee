/*
 * =====================================================================================
 * 
 *       Filename:  Csys_animal.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CSYS_ANIMAL_INCL
#define  CSYS_ANIMAL_INCL
#include "Ctable.h"
#include "proto.h"

class Csys_animal : public Ctable{
	private:
		int insert(uint32_t sys_animal_type, uint32_t value);

		int update_sql(uint32_t sys_animal_type, uint32_t value);

		int update_add_sql(uint32_t sys_animal_type, uint32_t value);

		int get_value_list(uint32_t start_type, uint32_t end_type, uint32_t **pp_value, uint32_t *p_count);

		int update_value(uint32_t sys_animal_type, uint32_t value);

		int add_value(uint32_t sys_animal_type,uint32_t value);
	public:
		Csys_animal(mysql_interface * db );

		int change_value(uint32_t type, int32_t change_value, uint32_t maxvalue);

		int get_col_value(uint32_t type, uint32_t *p_value);

		int set_max_weight(userid_t userid, uint32_t weight, uint32_t *p_max_weight, userid_t *p_max_userid);

};

#endif   /* ----- #ifndef CSYS_ARG_INCL  ----- */

