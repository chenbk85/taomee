/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_hot_play.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/01/2011 02:00:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Csysarg_hot_play.h"
#include "proto.h"


Csysarg_hot_play::Csysarg_hot_play(mysql_interface *db):
	Ctable(db, "SYSARG_DB", "t_sysarg_hot_play")
{

}

int Csysarg_hot_play::insert(uint32_t id, uint32_t count)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u)",
			this->get_table_name(),
			id,
			count
			);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Csysarg_hot_play::update(uint32_t id, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set count = count + %u where id = %u",
			this->get_table_name(),
			value,
			id
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}

int Csysarg_hot_play::get_all_items(sysarg_get_hot_play_out_item **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select id, count from %s ",
			this->get_table_name()
		   );

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->count);
	STD_QUERY_WHILE_END();;
}
