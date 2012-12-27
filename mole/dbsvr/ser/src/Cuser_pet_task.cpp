/**
 * ============================================================================
 * @file    Cuser_pet_task.cpp
 *
 * @brief   添加，删除，更新，获取用户宠物的任务信息。
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
#include "Cuser_pet_task.h"

/**
 * @brief   Cuser_task_ex类的构造函数 
 * @param   db数据库类指针
 * @return  无返回值 
 * @details  基类可以根据userid找到相应的数据库和表，USER数据库前缀，t_user_task_ex表的前缀，
 *          userid根据userid查询表
 */
Cuser_pet_task :: Cuser_pet_task(mysql_interface *db) : CtableRoute100x10(db, "USER", "t_user_pet_task", "userid")
{

}

/**
 * @brief  更新用户任务数据
 * @param  userid 用户的ID号
 * @param  p_in 任务ID号和需更新的数据
 */
int Cuser_pet_task :: update_temp_data(userid_t userid, uint32_t petid, uint32_t taskid, uint8_t *p_temp)
{
	char tasklist_mysql[mysql_str_len(TASK_LIST_EX_LEN)];
	set_mysql_string(tasklist_mysql, (char*)p_temp, TASK_LIST_EX_LEN);
	sprintf( this->sqlstr, "update %s  set data_from_client='%s' where userid=%u and petid=%u and taskid = %u",
			 this->get_table_name(userid), 
			 tasklist_mysql, 
			 userid,
			 petid,
			 taskid
		    );

    STD_SET_RETURN_EX(this->sqlstr,TASKID_NOT_EXIST_ERR);
}

/**
 * @brief 插入相关的用户记录
 * @param userid 用户ID号
 * @param p_in 包括任务的ID号和需插入的数据
 */
int Cuser_pet_task :: insert(userid_t userid, uint32_t petid, uint32_t taskid, uint32_t value)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, 0x00000000)",
			this->get_table_name(userid), 
			userid, 
			petid,
			taskid,
			value
			);
	STD_INSERT_RETURN(this->sqlstr, SUCC);
}

/**
 * @brief 删除用户任务记录
 * @param userid 用户ID号
 * @param taskid 任务的ID号
 */
int Cuser_pet_task :: del(userid_t userid, uint32_t petid, uint32_t taskid)
{
	sprintf(this->sqlstr,"delete from %s where userid=%u and petid = %u and taskid=%u",
			this->get_table_name(userid),
			userid,
			petid,
			taskid
		   );
	STD_INSERT_RETURN(this->sqlstr, TASKID_NOT_EXIST_ERR);
}

int Cuser_pet_task::del(userid_t userid, uint32_t petid)
{
	sprintf(this->sqlstr,"delete from %s where userid=%u and petid = %u",
			this->get_table_name(userid),
			userid,
			petid
		   );
	STD_INSERT_RETURN(this->sqlstr, TASKID_NOT_EXIST_ERR);
}

/**
 * @brief 取得客户端数据
 * @param userid 用户ID号
 * @param taskid 任务的ID号
 * @param p_out 存储返回用户的数据
 */
int Cuser_pet_task :: get_temp_data(userid_t userid, uint32_t petid, uint32_t taskid, uint8_t *p_out)
{
	sprintf(this->sqlstr, "select data_from_client from %s where userid=%u and petid=%u and taskid = %u",
			this->get_table_name(userid),
			userid,
			petid,
			taskid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, TASKID_NOT_EXIST_ERR);
		MEM_CPY_NEXT_FIELD(p_out, TASK_LIST_EX_LEN);
	STD_QUERY_ONE_END();
}

/**
 * @brief 取得指定用户所有宠物的所有任务
 */
int Cuser_pet_task:: get_list(userid_t userid, stru_user_task_ex **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select taskid, data_from_client  from %s \
			where userid = %u ",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->taskid);
		BIN_CPY_NEXT_FIELD((*pp_list + i)->data,sizeof( (*pp_list + i)->data));
	STD_QUERY_WHILE_END();
}

int Cuser_pet_task :: get_task_stat(userid_t userid, uint32_t petid, uint32_t taskid, uint32_t *p_value)
{

		sprintf( this->sqlstr, "select  taskstat from %s where userid=%u and petid = %u and taskid = %u",
				this->get_table_name(userid),
				userid,
			   	petid,
				taskid
				);
		STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
			INT_CPY_NEXT_FIELD(*p_value);	
		STD_QUERY_ONE_END();

}

int Cuser_pet_task :: update_task_stat(userid_t userid, uint32_t petid, uint32_t taskid, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set taskstat= %u where userid=%u and petid = %u and taskid = %u",
			this->get_table_name(userid), 
			value,
	   		userid,
			petid,
			taskid
		  );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
} 

int Cuser_pet_task :: set_task_stat(userid_t userid, uint32_t petid, uint32_t taskid, uint32_t value)
{
	uint32_t stat = 0;
	int ret=this->get_task_stat(userid, petid, taskid, &stat);
	if ((ret != SUCC) && (ret != USER_ID_NOFIND_ERR))
	{
		return ret;
	}

	if (value == 1)
	{ //任务状态的转移合法性检验
		if (stat == 1)
		{
			return USER_PET_TASK_STATU_TRANSFER_1_1_ERR;
		}

		if (stat == 2)
		{
			return USER_PET_TASK_STATU_TRANSFER_2_1_ERR;
		}

	} else if (value == 2) {
		if ((stat == 0) || (stat == 2)) {
			return USER_PET_TASK_STATU_TRANSFER_ERR;
		}
	} else {
		return USER_PET_TASK_STATU_TRANSFER_ERR;
	}

	stat = value;
	ret = this->update_task_stat(userid, petid, taskid, value);
	if (ret != SUCC)
	{
		return this->insert(userid, petid, taskid, value);		
	}

	return SUCC;
}
 
