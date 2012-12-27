/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_task.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 13时47分01秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_TASK_INCL
#define    CUSER_TASK_INCL
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_task:public CtableRoute100x10
{
private:
	userid_t _userid;

	int add(userid_t userid, user_pet_task_set_in * p_in );
	int update(userid_t userid, user_pet_task_set_in *p_in);
public:
	Cuser_task(mysql_interface * db ); 

	void userid(const userid_t uid)
	{
		_userid=uid;
	}

	int get(userid_t userid, user_pet_task_get_list_in *p_in, uint32_t *p_count, user_task_item	**pp_out_item );
	int remove(userid_t userid, uint32_t petid,uint32_t taskid );

	/**
	 * 设置任务参数，如果任务不存在，则加入
	 */
	int set_info(userid_t userid, user_pet_task_set_in *p_in);
	int get_usetime(userid_t userid, uint32_t petid, uint32_t taskid,	uint32_t *p_usetime );
	int remove_pet_task(userid_t userid, uint32_t petid);
	int update_magic(userid_t userid, uint32_t petid, uint32_t taskid, const char *str_field, uint32_t value);
	int add_magic(userid_t userid, user_pet_task_set_ex_in *p_in, uint32_t flag);
	int get_magic(userid_t userid, uint32_t petid, uint32_t taskid, uint32_t *p_time, uint32_t *p_step);
	int set_stage_flag(userid_t userid, user_task_pet_first_stage_set_in *p_in, uint32_t *p_count);
	int delete_task(userid_t userid, uint32_t petid, uint32_t taskid);
	int del(userid_t userid, uint32_t petid, uint32_t taskid, uint32_t *p_count);
	int get_count(userid_t userid, uint32_t petid, uint32_t *p_count, uint32_t flag);
	int get_client(userid_t userid, uint32_t petid, uint32_t taskid, uint32_t *p_client);
	int get_pet_magic(userid_t userid,user_task_get_pet_task_in *p_in,
					uint32_t *p_count, user_task_get_pet_task_out_item  **pp_out_item);
	int get_all_count(userid_t userid, uint32_t start_id, uint32_t end_id, uint32_t *p_count);
	int get_pet_magic(userid_t userid, uint32_t petid, uint32_t taskid, uint32_t *p_step);

	/**
	 * @brief 得到宠物任务的状态
	 * @param userid 米米号
	 * @param petid  宠物的ID号
	 * @param pp_out_item 返回拉姆的任务的ID号和状态
	 * @param p_count 返回任务的个数
	 */
	int get_taskid_state(userid_t userid, uint32_t petid,  pet_task_state **pp_out_item, uint32_t *p_count);

	int get_all_field(userid_t userid, user_magic_task_pet_get_all_out_item **pp_out_item, uint32_t *p_count);
	int update_all_field(userid_t userid, user_magic_task_pet_update_in *p_in);

	int get_pet_task_info_by_one(const uint32_t petid, const uint32_t taskid, uint32_t& state)
	{
		sprintf(this->sqlstr, "select flag from %s where userid=%u and petid = %u and taskid=%u", 
				this->get_table_name(_userid),
				_userid,
				petid,
				taskid
			   ); 
		STD_QUERY_ONE_BEGIN(this->sqlstr,TASKID_NOT_EXIST_ERR);
			INT_CPY_NEXT_FIELD(state); 
		STD_QUERY_ONE_END();
	}

};
#endif   /* ----- #ifndef CUSER_TASK_INCL  ----- */
