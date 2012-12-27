/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_beauty_contend.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/22/2011 09:57:01 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_beauty_contend.h"
#include "proto.h"
#include "benchapi.h"


Csysarg_beauty_contend::Csysarg_beauty_contend(mysql_interface *db):
	Ctable(db, "SYSARG_DB", "t_sysarg_beauty_contend")
{

}

int Csysarg_beauty_contend::insert(uint32_t value_1, uint32_t value_2, uint32_t value_3, uint32_t total)
{
	sprintf(this->sqlstr, "insert into %s values(%u,%u,%u,%u,0,0)",
			this->get_table_name(),
			value_1,
			value_2,
			value_3,
			total
			);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Csysarg_beauty_contend::update_two_col_inc(const char* col_1, const char* col_2, uint32_t value_1,
	   	uint32_t value_2)
{
	sprintf(this->sqlstr, "update %s set %s = %s + %u, %s = %s + %u ",
			this->get_table_name(),
			col_1,
			col_1,
			value_1,
			col_2,
			col_2,
			value_2
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Csysarg_beauty_contend::update_two_cols(const char* col_1, const char* col_2, uint32_t value_1,
	   	uint32_t value_2)
{
	sprintf(this->sqlstr, "update %s set %s = %u, %s = %u ",
			this->get_table_name(),
			col_1,
			value_1,
			col_2,
			value_2
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}


int Csysarg_beauty_contend::get_all(uint32_t *value_1, uint32_t *value_2, uint32_t *value_3, uint32_t *total)
{
	sprintf(this->sqlstr, "select value_1, value_2, value_3, total from %s ",
			this->get_table_name()
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(*value_1);
		INT_CPY_NEXT_FIELD(*value_2);
		INT_CPY_NEXT_FIELD(*value_3);
		INT_CPY_NEXT_FIELD(*total);
	STD_QUERY_ONE_END();
}

int Csysarg_beauty_contend::add(sysarg_join_beauty_contend_in *p_in)
{
	char* col = (char*)"";	
	if(p_in->type == 1){
		col = (char*)"value_1";	
	}
	else if(p_in->type == 2){
		col = (char*)"value_2";
	}
	else if(p_in->type == 3){
		col = (char*)"value_3";
	}
	int ret = 0;
	if(strcmp(col, "") != 0){
		ret = this->update_two_col_inc(col, (char*)"total", 1, 1);
		if(ret == USER_ID_NOFIND_ERR){
			if(p_in->type == 1){
				ret = this->insert(1, 0, 0, 1);
			}
			else if(p_in->type == 2){
				ret = this->insert(0, 1, 0, 1);
			}
			else if(p_in->type == 3){
				ret = this->insert(0, 0, 1, 1);
			}
		}
	}//if
	return ret ;

}
int Csysarg_beauty_contend::get_total(uint32_t *total)
{
	sprintf(this->sqlstr, "select total from %s for update",
			this->get_table_name()
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(*total);
	STD_QUERY_ONE_END();
}

int Csysarg_beauty_contend::get_two_cols(const char* col_1, const char* col_2, 
		uint32_t *value_1, uint32_t *value_2)
{
	sprintf(this->sqlstr, "select %s, %s from %s",
			col_1,
			col_2,
			this->get_table_name()
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(*value_1);
		INT_CPY_NEXT_FIELD(*value_2);
	STD_QUERY_ONE_END();
}
