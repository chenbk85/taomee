/*
 * =====================================================================================
 *
 *       Filename:  Csys_arg.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include "Csys_msg.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
#include <algorithm>

/* @brief 构造函数
 * @param db DB的类指针
 */
Csys_msg::Csys_msg(mysql_interface * db ) : Ctable( db, "SYSARG_DB","t_sysarg_msg")
{ 

}

/* @brief 插入队的记录 
 * @team 插入队记录的ID号 
 */
int Csys_msg :: insert(sysarg_db_msg_edit_in *p_in)
{
	char content[mysql_str_len(sizeof(p_in->content))];
    set_mysql_string(content, p_in->content, sizeof(p_in->content));

	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, '%s')",
			this->get_table_name(),
			p_in->date,
			p_in->start,
			p_in->end,
			content
		   );
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);	
}

int Csys_msg:: del(uint32_t date, uint32_t start) 
{
	sprintf(this->sqlstr, "delete from %s where date = %u and start = %u",
			this->get_table_name(),
			date,
			start
			);
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR);
}


int Csys_msg :: update(sysarg_db_msg_edit_in *p_in)
{

	char content[mysql_str_len(sizeof(p_in->content))];
	set_mysql_string(content, p_in->content, sizeof(p_in->content));
	
	sprintf( this->sqlstr, "update %s set end = %u, content = '%s' where date = %u and start = %u",
			 this->get_table_name(),
			 p_in->end,
			 content,
			 p_in->date,
			 p_in->start
     		);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Csys_msg :: msg_edit(sysarg_db_msg_edit_in *p_in)
{
	uint32_t ret = this->insert(p_in);
	if (ret != SUCC) {
		ret = this->update(p_in);
	}
	return ret;
}

int Csys_msg:: get_list(sysarg_db_msg_get_in *p_in, sysarg_db_msg_get_out_item **pp_list,
		             uint32_t *p_count)
{
	sprintf(this->sqlstr, "select date, start, end, content from %s where date >= %u and date <= %u",
			this->get_table_name(),
			p_in->date_start,
			p_in->date_end
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->date);
		INT_CPY_NEXT_FIELD((*pp_list + i)->start);
		INT_CPY_NEXT_FIELD((*pp_list + i)->end);
		BIN_CPY_NEXT_FIELD((*pp_list + i)->content, sizeof((*pp_list)->content));
	STD_QUERY_WHILE_END();
}


int Csys_msg:: get_all(sysarg_db_msg_get_all_web_out_item **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select date, start, end, content from %s",
			this->get_table_name()
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->date);
		INT_CPY_NEXT_FIELD((*pp_list + i)->start);
		INT_CPY_NEXT_FIELD((*pp_list + i)->end);
		BIN_CPY_NEXT_FIELD((*pp_list + i)->content, sizeof((*pp_list)->content));
	STD_QUERY_WHILE_END();
}


