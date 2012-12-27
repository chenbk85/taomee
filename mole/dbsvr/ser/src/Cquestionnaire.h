/*
 * =====================================================================================
 * 
 *       Filename:  Cquestionnaire.h
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

#ifndef  CQUESTIONNAIRE_INCL
#define  CQUESTIONNAIRE_INCL
#include "Ctable.h"
#include "proto.h"

class Cquestionnaire : public Ctable{
	protected:
		int insert(uint32_t type, uint32_t maintype,uint32_t subtype, int value );
		int update_add(uint32_t type, uint32_t maintype,uint32_t subtype, int value );
	public:
		int add_value(uint32_t type, uint32_t maintype,uint32_t subtype, int value );
		Cquestionnaire(mysql_interface * db );

		int get_value_list(uint32_t type, uint32_t * p_count,	
				sysarg_get_questionnaire_list_out_item **pp_item  );
		int get_subtype_with_max_value(uint32_t type, uint32_t maintype, uint32_t* sub_type);
};
#endif   /* ----- #ifndef CQUESTIONNAIRE_INCL  ----- */

