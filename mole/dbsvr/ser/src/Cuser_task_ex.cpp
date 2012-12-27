/**
 * ============================================================================
 * @file    Cuser_task_ex.cpp
 *
 * @brief   添加，删除，更新，获取用户的任务信息。
 *
 * compiler gcc version 4.1.2
 *
 * platfomr Debian 4.1.1-12
 *
 * copyright TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 * ============================================================================
 */


#include "proto.h"
#include "common.h"
#include "Cuser_task_ex.h"
#include <sstream>


/**
 * @brief   Cuser_task_ex类的构造函数 
 * @param   db数据库类指针
 * @return  无返回值 
 * @notice  基类可以根据userid找到相应的数据库和表，USER数据库前缀，t_user_task_ex表的前缀，
 *          userid根据userid查询表
 */
Cuser_task_ex :: Cuser_task_ex(mysql_interface *db) : CtableRoute100x10(db, "USER", "t_user_task_ex", "userid")
{

}



/**
 * @brief  更新用户任务数据
 * @param  userid 用户的ID号
 * @param  p_in 任务ID号和需更新的数据
 */
int Cuser_task_ex :: update(userid_t userid, user_task_set_in *p_in)
{
	char tasklist_mysql[mysql_str_len(TASK_LIST_EX_LEN)];
	//处理字体的格式
	set_mysql_string(tasklist_mysql, p_in->data, TASK_LIST_EX_LEN);
	sprintf( this->sqlstr, "update %s  set taskvalue='%s'\
			 where userid=%u and taskid=%u",
			 this->get_table_name(userid), 
			 tasklist_mysql, 
			 userid,
			 p_in->taskid
		    );

    STD_SET_RETURN_EX(this->sqlstr,TASKID_NOT_EXIST_ERR);
}



/**
 * @brief 插入相关的用户记录
 * @param userid 用户ID号
 * @param p_in 包括任务的ID号和需插入的数据
 */
int Cuser_task_ex :: insert(userid_t userid, user_task_set_in *p_in)
{
	char tasklist_mysql[mysql_str_len(TASK_LIST_EX_LEN)];
	set_mysql_string(tasklist_mysql, p_in->data, TASK_LIST_EX_LEN);
	sprintf(this->sqlstr, "insert into %s values(%u, %u,'%s')",
			this->get_table_name(userid), 
			userid, 
			p_in->taskid,
			tasklist_mysql
			);
	STD_INSERT_RETURN(this->sqlstr, SUCC);
}



/**
 * @brief 删除用户任务记录
 * @param userid 用户ID号
 * @param taskid 任务的ID号
 */
int Cuser_task_ex :: del(userid_t userid, uint32_t taskid)
{
	sprintf(this->sqlstr,"delete from %s where userid=%u and taskid=%u",
			this->get_table_name(userid),
			userid,
			taskid
		   );
	STD_INSERT_RETURN(this->sqlstr, TASKID_NOT_EXIST_ERR);
}

/**
 * @brief 得到用户的任务记录
 * @param userid 用户ID号
 * @param taskid 任务的ID号
 * @param p_out 存储返回用户的数据
 */
int Cuser_task_ex :: get(userid_t userid, uint32_t taskid, user_task_get_out *p_out)
{
	sprintf(this->sqlstr, "select taskvalue from %s where userid=%u and taskid=%u",
			this->get_table_name(userid),
			userid,
			taskid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, TASKID_NOT_EXIST_ERR);
		MEM_CPY_NEXT_FIELD(p_out->data, TASK_LIST_EX_LEN);
	STD_QUERY_ONE_END();
}

int Cuser_task_ex:: get_list(userid_t userid, stru_user_task_ex **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select taskid,taskvalue  from %s \
			where userid = %u ",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->taskid);
		BIN_CPY_NEXT_FIELD( (*pp_list + i)->data,sizeof( (*pp_list + i)->data));
	STD_QUERY_WHILE_END();
}

int Cuser_task_ex::get_task_id_list(userid_t userid, get_user_task_ex_idlist_out_item **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select taskid from %s where userid = %u and taskid > 1000000",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->task_id);
	STD_QUERY_WHILE_END();
}

int Cuser_task_ex::get_specify_list(userid_t userid, uint32_t in_count, user_get_task_list_in_item *p_in_item,
	  	user_get_task_list_out_item** p_out_item, uint32_t *out_count)
{
	std::ostringstream in_str;
	for(uint32_t i = 0; i < in_count; ++i){
		in_str << (p_in_item + i)->taskid;
		if(i < in_count - 1){
			in_str << ',';
		}
	}
	sprintf(this->sqlstr, "select taskid, taskvalue from %s where userid = %u and taskid in (%s)",
			this->get_table_name(userid),
			userid,
			in_str.str().c_str()
		   );

	STD_QUERY_WHILE_BEGIN(this->sqlstr, p_out_item, out_count);
		INT_CPY_NEXT_FIELD((*p_out_item + i)->taskid);
		BIN_CPY_NEXT_FIELD( (*p_out_item + i)->data,sizeof( (*p_out_item + i)->data));
	STD_QUERY_WHILE_END();

}
