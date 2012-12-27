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
#include "Cuser_npc_task.h"



/**
 * @brief   Cuser_task_ex类的构造函数 
 * @param   db数据库类指针
 * @return  无返回值 
 * @notice  基类可以根据userid找到相应的数据库和表，USER数据库前缀，t_user_task_ex表的前缀，
 *          userid根据userid查询表
 */
Cuser_npc_task :: Cuser_npc_task(mysql_interface *db) : CtableRoute100x10(db, "USER", "t_user_npc_task", "userid")
{

}

int Cuser_npc_task :: insert(userid_t userid, uint32_t npc, uint32_t taskid, uint32_t week_day)
{
	uint32_t date = 0;
	//每天任务
	if (week_day == 1) {
		date = get_date(time(NULL));	
	//每周如无
	} else if(week_day == 2) {
		date = get_week_id(time(NULL));
	//每月任务
	} else if(week_day == 4) {
		date = get_year_month(time(NULL));
	} else if(week_day != 0) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, 1, 1, %u, %u, 0x00000000)",
			this->get_table_name(userid), 
			userid, 
			npc,
			taskid,
			week_day,
			date
			);
	STD_INSERT_RETURN(this->sqlstr, SUCC);
}

/**
 * @brief  更新用户任务数据
 * @param  userid 用户的ID号
 * @param  p_in 任务ID号和需更新的数据
 */
int Cuser_npc_task :: update_buf(userid_t userid, uint32_t npc, uint32_t taskid, char *p_char)
{
	char tasklist_mysql[mysql_str_len(TASK_LIST_EX_LEN)];
	//处理字体的格式
	set_mysql_string(tasklist_mysql, p_char, TASK_LIST_EX_LEN);
	sprintf( this->sqlstr, "update %s  set taskvalue = '%s' where userid=%u and npc = %u and taskid=%u",
			 this->get_table_name(userid), 
			 tasklist_mysql, 
			 userid,
			 npc,
			 taskid
		    );
    STD_SET_RETURN_EX(this->sqlstr,TASKID_NOT_EXIST_ERR);
}

int Cuser_npc_task :: finish_task(userid_t userid, uint32_t npc, uint32_t taskid, uint32_t *p_num)
{
	uint32_t state = 0;
	uint32_t num = 0;
	uint32_t flag = 0;
	uint32_t date = 0;
	uint32_t ret = this->get_task_info(userid, npc, taskid, &state, &num, &flag, &date);
	if (ret != SUCC) {
		return ret;
	}
	uint32_t check_same = 0;
	uint32_t now = 0;
	ret = this->check_same_time(date, flag, &check_same, &now);
	if (ret != SUCC) {
		return ret;
	}
	if (check_same) {
		num = 0;
	}
	if (state != 1) {
		return YOU_HAVE_NOT_RECV_TASK_ERR;
	}
	state = 2;
	ret = this->update(userid, npc, taskid, state, num, now);
	*p_num = num;
	return ret;
}

int Cuser_npc_task :: check_same_time(uint32_t date, uint32_t flag, uint32_t *p_flag, uint32_t *p_now)
{
	*p_flag = 0;
	//每日任务
	if (flag == 1) {
		uint32_t now = get_date(time(NULL));
		if (now != date) {
			*p_flag = 1;
		}	
		*p_now = now;
	//每周任务	
	} else if (flag == 2) {
		uint32_t now = get_week_id(time(NULL));
		if (now != date) {
			*p_flag = 1;
		}
		*p_now = now;
	//每月任务
	} else if (flag == 4) {
		uint32_t now = get_year_month(time(NULL));
		if (date != now) {
			*p_flag = 1;
		}
		*p_now = now;
	} 
	return SUCC;
}

int Cuser_npc_task :: recv_task(userid_t userid, uint32_t npc, uint32_t taskid, uint32_t maxnum,
		                        uint32_t week_day, uint32_t *p_num)
{
	uint32_t state = 0;
	uint32_t num = 0;
	uint32_t flag = 0;
	uint32_t date = 0;
	uint32_t ret = this->get_task_info(userid, npc, taskid, &state, &num, &flag, &date);
	if (ret != SUCC) {
		if (ret == TASKID_NOT_EXIST_ERR) {
		 	ret = this->insert(userid, npc, taskid, week_day);
			if (ret != SUCC) {
				return ret;
			}
			*p_num = 1;
			return SUCC;
		} else {
			return ret;
		}
	}
	uint32_t check_same = 0;
	uint32_t now = 0;
	ret = this->check_same_time(date, flag, &check_same, &now);
	if (ret != SUCC) {
		return ret;
	}
	if (check_same) {
		num = 0;
	}
	if (state == 1) {
		return YOU_HAVE_NOT_FINISH_TASK_ERR;
	}
	if (num >= maxnum) {
		return YOU_HAVE_GOT_TASK_TOO_MANY_ERR;
	}
	num++;
	state = 1;
	ret = this->update(userid, npc, taskid, state, num, now);
	*p_num = num;
	return ret;
}

int Cuser_npc_task :: update(userid_t userid, uint32_t npc, uint32_t taskid, uint32_t state, uint32_t num, uint32_t now)
{
	sprintf( this->sqlstr, "update %s set state = %u, num = %u, recv_tm = %u where userid=%u and npc = %u and taskid=%u",
			 this->get_table_name(userid), 
			 state,
			 num,
			 now,
			 userid,
			 npc,
			 taskid
		    );

    STD_SET_RETURN_EX(this->sqlstr,TASKID_NOT_EXIST_ERR);
}

int Cuser_npc_task :: get_task_info(userid_t userid, uint32_t npc, uint32_t taskid,
	                  uint32_t *p_state, uint32_t *p_num, uint32_t *p_flag, uint32_t *p_tm)
{
	sprintf(this->sqlstr, "select state, num, flag, recv_tm from %s where userid=%u and npc = %u and taskid=%u",
			this->get_table_name(userid),
			userid,
			npc,
			taskid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, TASKID_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(*p_state);
		INT_CPY_NEXT_FIELD(*p_num);
		INT_CPY_NEXT_FIELD(*p_flag);
		INT_CPY_NEXT_FIELD(*p_tm);
	STD_QUERY_ONE_END();
}

/**
 * @brief 得到用户的任务记录
 * @param userid 用户ID号
 * @param taskid 任务的ID号
 * @param p_out 存储返回用户的数据
 */
int Cuser_npc_task :: get_buf(userid_t userid, uint32_t npc, uint32_t taskid, char *p_out)
{
	sprintf(this->sqlstr, "select taskvalue from %s where userid=%u and npc = %u and taskid=%u",
			this->get_table_name(userid),
			userid,
			npc,
			taskid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, TASKID_NOT_EXIST_ERR);
		BIN_CPY_NEXT_FIELD(p_out, TASK_LIST_EX_LEN);
	STD_QUERY_ONE_END();
}

int Cuser_npc_task:: get_list(userid_t userid, uint32_t npc, user_npc_task_get_npc_task_out_item **pp_list, uint32_t *p_count)
{
	if (npc == 0) {/* 得到全部NPC的任务 */
		sprintf(this->sqlstr, "select state, num, taskid, flag, recv_tm from %s where userid = %u",
				this->get_table_name(userid),
				userid
				);
	} else {
		sprintf(this->sqlstr, "select state, num, taskid, flag, recv_tm from %s where userid = %u and npc = %u",
				this->get_table_name(userid),
				userid,
				npc
				);

	}

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->state);
		INT_CPY_NEXT_FIELD((*pp_list + i)->num);
		INT_CPY_NEXT_FIELD((*pp_list + i)->taskid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->flag);
		INT_CPY_NEXT_FIELD((*pp_list + i)->recv_tm);
	STD_QUERY_WHILE_END();
}

int Cuser_npc_task::delete_task(const userid_t uid, const uint32_t npc, const uint32_t taskid)
{
	//任务计数减一，同时把任务状态置0
	sprintf(this->sqlstr, "update %s set num=if(num>0, num-1, 0), state=0 where userid=%u and npc=%u and taskid=%u",
		this->get_table_name(uid),
		uid,
		npc,
		taskid
	);
	
	STD_SET_RETURN(this->sqlstr, uid, USER_ID_NOFIND_ERR);
}

