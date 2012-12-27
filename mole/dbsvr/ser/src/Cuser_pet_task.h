/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_task_ex.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  03/23/2009 02:34:47 PM CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "Cuser.h"


#ifndef  CUSER_PET_TASK_INC
#define  CUSER_PET_TASK_INC

class Cuser_pet_task : public CtableRoute100x10
{

public:
	Cuser_pet_task(mysql_interface *mysql);

	void userid(const userid_t uid)
	{
		_userid=uid;
	}

	void petid(const uint32_t pid)
	{
		_petid=pid;
	}

	void taskid(const uint32_t tid)
	{
		_taskid=tid;
	}

	void cmd_init(const userid_t uid, const uint32_t pid )
	{
		_userid=uid;
		_petid=pid;
	}

	void cmd_init(const userid_t uid, const uint32_t pid , const uint32_t tid)
	{
		_userid=uid;
		_petid=pid;
		_taskid=tid;
	}

	int insert(userid_t userid, uint32_t petid, uint32_t taskid, uint32_t value);

	int get_task_stat(userid_t userid, uint32_t petid, uint32_t taskid, uint32_t *p_value);

	int update_task_stat(userid_t userid, uint32_t petid, uint32_t taskid, uint32_t value);

	int set_task_stat(userid_t userid, uint32_t petid, uint32_t taskid, uint32_t value);

	int update_temp_data(userid_t userid, uint32_t petid, uint32_t taskid, uint8_t *p_in);

	int del(userid_t userid,  uint32_t petid, uint32_t taskid);
	
	int del(userid_t userid,  uint32_t petid);

	int get_temp_data(userid_t userid, uint32_t petid, uint32_t taskid, uint8_t *p_out);

	int  get_list(userid_t userid, stru_user_task_ex **pp_list, uint32_t *p_count);

	int get_task_stat_by_user(uint32_t stat, user_pet_task_get_by_user_out_item** pp_list, uint32_t* p_count)
	{
		sprintf(this->sqlstr, "select taskid from %s \
				where userid = %u\
				and taskstat = %u",
				this->get_table_name(_userid),
				_userid,
				stat
			);

		STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
			INT_CPY_NEXT_FIELD(pp_list[i]->taskid);
		STD_QUERY_WHILE_END();
	}

	int get_task_stat_by_user_task(uint32_t stat, user_pet_task_get_client_by_user_task_out_item** pp_list, uint32_t* p_count)
	{
		sprintf(this->sqlstr, "select petid, data_from_client from %s "
				" where userid = %u"
				" and taskstat = %u"
				" and taskid= %u",
				this->get_table_name(_userid),
				_userid,
				stat,
				_taskid
			);

		STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
			INT_CPY_NEXT_FIELD((*pp_list+i)->petid);
			MEM_CPY_NEXT_FIELD((*pp_list+i)->data_from_client, TASK_LIST_EX_LEN);
		STD_QUERY_WHILE_END();
	}

	int get_task_stat_by_user(bool& have_doing_task)
	{
		sprintf(this->sqlstr, "select count(taskid) from %s \
				where userid = %u\
				and taskstat = 0x1",
				this->get_table_name(_userid),
				_userid
				);
		uint32_t count=0;

		STD_QUERY_ONE_BEGIN(this->sqlstr, ID_NOT_EXIST_ERR);
			INT_CPY_NEXT_FIELD(count);
		STD_QUERY_ONE_END_WITHOUT_RETURN();

		have_doing_task = count ? 1 : 0;

		return SUCC;
	}

private:
	userid_t _userid;
	uint32_t _petid;
	uint32_t _taskid;
};

#endif   /* ----- #ifndef CUSER_DATA_INC  ----- */

