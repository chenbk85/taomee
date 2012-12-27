/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_mark_attire.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2012 11:39:35 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_mark_attire.h"



Csysarg_mark_attire::Csysarg_mark_attire(mysql_interface *db):
	Ctable(db,"SYSARG_DB", "t_sysarg_mark_attire")
{

}


int Csysarg_mark_attire::insert(uint32_t attireid, uint32_t score, uint32_t count)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u)",
			this->get_table_name(),
			attireid,
			score,
			count
			);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}


int Csysarg_mark_attire::update(uint32_t attireid, uint32_t score, uint32_t count)
{
	sprintf(this->sqlstr, "update %s set score = score + %u, count = count + %u where itemid = %u",
			this->get_table_name(),
			score,
			count,
			attireid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csysarg_mark_attire::add(uint32_t attireid, uint32_t score, uint32_t count)
{
	int ret = update(attireid, score, count);
	if(ret != SUCC){
		ret = insert(attireid, score, count);
	}
	return ret;
}

int Csysarg_mark_attire::select_all(uint32_t *p_count, sysarg_get_mark_attire_out_item **pp_list)
{
	sprintf(this->sqlstr, "select itemid, score div count as average from %s",
			this->get_table_name()
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->attireid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->aver);
	STD_QUERY_WHILE_END();
}
