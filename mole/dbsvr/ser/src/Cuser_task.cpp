/*
 * =====================================================================================
 *
 *       Filename:  Cuser_task_pet.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 13时46分59秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include "Cuser_task.h"

uint32_t super_id[] = {101};
#define SUPER_NUM sizeof(super_id) / sizeof(super_id[0])

uint32_t lamu_id[] = {102, 103, 104};
#define LAMU_NUM sizeof(lamu_id) / sizeof(lamu_id[0])

//user_task
Cuser_task::Cuser_task(mysql_interface * db ) 
	:CtableRoute100x10( db,"USER","t_user_task","userid")
{ 

}
int Cuser_task::remove(userid_t userid, uint32_t petid,uint32_t taskid )
{
	sprintf( this->sqlstr, " delete from  %s \
		where userid=%u and petid=%u and taskid=%u " ,
		this->get_table_name(userid), 
		userid, petid,taskid );
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}
int Cuser_task::remove_pet_task(userid_t userid, uint32_t petid)
{
	sprintf( this->sqlstr, " delete from  %s \
		where userid=%u and petid=%u " ,
		this->get_table_name(userid), 
		userid, petid);
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}


int Cuser_task::get(userid_t userid, user_pet_task_get_list_in *p_in, 
	   	uint32_t *p_count, user_task_item	**pp_out_item )
{
	char  flagstr[100];
	char  petidstr[100];
	if (p_in->petid==0xFFFFFFFF){
		strcpy(petidstr,"true" );
	}else{
		sprintf(petidstr, "petid=%u",p_in->petid);
	}

	if (p_in->flag==0){
		strcpy(flagstr,"true" );
	}else{
		sprintf(flagstr,"flag=%u",p_in->flag);
	}
	
	sprintf( this->sqlstr, "select petid, taskid,\
			flag,usetime,starttime,endtime from %s \
			where userid=%u and %s  and   %s  and  taskid>=%u and taskid<=%u ", 
			this->get_table_name(userid),userid, petidstr ,flagstr,
			p_in->start_taskid,p_in->end_taskid ); 
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_out_item, p_count ) ;
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->petid); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->taskid); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->flag); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->usetime); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->startime); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->endtime); 
	STD_QUERY_WHILE_END();
}

int Cuser_task::add(userid_t userid, user_pet_task_set_in * p_in )
{
	sprintf( this->sqlstr, "insert into %s values (\
		%u,%u,%u,%u,%u,%u,%u, 0, 0, 1)", 
			this->get_table_name(userid), 
			userid,
			p_in->petid,
			p_in->taskid,
			p_in->flag,
			p_in->usetime,
			p_in->startime,
			p_in->endtime
	   	);
	STD_INSERT_RETURN(this->sqlstr,SUCC );	
}

int Cuser_task::get_magic(userid_t userid, uint32_t petid, uint32_t taskid,	uint32_t *p_time, uint32_t *p_step)
{

	sprintf( this->sqlstr, "select first_time, step from %s  \
		where userid=%u and petid=%u and taskid=%u " ,
		this->get_table_name(userid), userid,petid,taskid );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_TASK_NOFIND_ERR);
	 	INT_CPY_NEXT_FIELD(*p_time);
	 	INT_CPY_NEXT_FIELD(*p_step);
	STD_QUERY_ONE_END();
}

int Cuser_task::get_client(userid_t userid, uint32_t petid, uint32_t taskid, uint32_t *p_client)
{

	sprintf( this->sqlstr, "select client from %s  \
			where userid=%u and petid=%u and taskid=%u " ,
			this->get_table_name(userid),
			userid,
			petid,
			taskid
		);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_TASK_NOFIND_ERR);
	 	INT_CPY_NEXT_FIELD(*p_client);
	STD_QUERY_ONE_END();
}

int Cuser_task::get_count(userid_t userid, uint32_t petid, uint32_t *p_count, uint32_t flag)
{
	char formatstr[6000]={};
	char valuestr[100]={};
	if ((flag & 0x01) == 0x01) {
		for(uint32_t i=0; i < SUPER_NUM; i++) {
			sprintf (valuestr, "%d,", super_id[i]);
			strcat(formatstr,valuestr );
		}
    	formatstr[strlen(formatstr)-1]='\0';
	} else {
		for(uint32_t i=0; i<LAMU_NUM; i++) {
			sprintf (valuestr, "%d,", lamu_id[i]);
			strcat(formatstr,valuestr);
		}
    	formatstr[strlen(formatstr)-1]='\0';
	}
	sprintf( this->sqlstr, "select count(*) from %s  \
			where userid=%u and petid = %u and taskid in (%s) " ,
			this->get_table_name(userid),
			userid,
			petid,
			formatstr
		);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_TASK_NOFIND_ERR);
	 	INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

int Cuser_task::get_all_count(userid_t userid, uint32_t start_id, uint32_t end_id, uint32_t *p_count)
{

	sprintf( this->sqlstr, "select count(*) from %s  \
			where userid=%u and taskid >= %u and taskid<=%u and step < 6 " ,
			this->get_table_name(userid),
			userid,
			start_id,
			end_id
		);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_TASK_NOFIND_ERR);
	 	INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}


/* @brief 完成某个阶段的任务，设置相应的标志
 * @param userid 米米号
 * @param p_in 输入的参数
 * @param p_count 返回拉姆的任务数目
 */
int Cuser_task::set_stage_flag(userid_t userid, user_task_pet_first_stage_set_in *p_in, uint32_t *p_count)
{
	uint32_t stage = p_in->stage;
	if (stage > 3 || stage < 1) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	uint32_t pre_step = 0;
	uint32_t ret = this->get_pet_magic(userid, p_in->petid, p_in->taskid, &pre_step);
	if (ret != SUCC) {
		return ret;
	}
	/*第一阶段结束 */
	if (stage == 1) {
		if (pre_step != 1) {
			return YOU_HAVE_NOT_FINISH_PRE_ERR;
		}
		uint32_t now = time(NULL);
		sprintf(this->sqlstr, " update %s set first_time = %u, step =2\
			where userid=%u and petid=%u and taskid=%u " ,
			this->get_table_name(userid), 
			now,
			userid,
			p_in->petid,
			p_in->taskid
			);
		*p_count = 1;
	} 
	/* 第二阶段结束 */
	if (stage == 2) {
		if (pre_step != 3) {
			return YOU_HAVE_NOT_FINISH_PRE_ERR;
		}
		sprintf(this->sqlstr, " update %s set step = 4\
			where userid=%u and petid=%u and taskid=%u " ,
			this->get_table_name(userid), 
			userid,
			p_in->petid,
			p_in->taskid
			);
		*p_count = 1;
	}
	/*任务结束*/
	if (stage == 3) {
		if (pre_step != 5) {
			return YOU_HAVE_NOT_FINISH_PRE_ERR;
		}
		ret = this->get_all_count(userid, 101, 104, p_count);
		if (ret != SUCC) {
			return ret;
		}
		*p_count -= 1;
		sprintf(this->sqlstr, " update %s set flag = 3, step = 6\
			where userid=%u and petid=%u and taskid=%u " ,
			this->get_table_name(userid), 
			userid,
			p_in->petid,
			p_in->taskid
			);
	}
	STD_SET_RETURN_EX(this->sqlstr,USER_TASK_NOFIND_ERR);	
}

/* @breif 更新相应字段的值
 * @param userid 米米号
 * @param petid 宠物的ID号
 * @param taskid 任务的ID号
 * @param str_field 设置字段的名称
 * @param value 设置的值
 */
int Cuser_task::update_magic(userid_t userid, uint32_t petid, uint32_t taskid, const char *str_field, uint32_t value)
{
	sprintf( this->sqlstr, " update %s set %s = %u\
		where userid=%u and petid=%u and taskid=%u " ,
		this->get_table_name(userid), 
		str_field,
		value,
		userid,
		petid,
		taskid
		);
	STD_SET_RETURN_EX(this->sqlstr,USER_TASK_NOFIND_ERR);	
}

/* @brief 接某个阶段的任务，进行相应的操作 
 * @param userid 米米号
 * @param p_in 输入参数
 * @param flag 是否是超拉的标志
 */
int Cuser_task::add_magic(userid_t userid, user_pet_task_set_ex_in *p_in, uint32_t flag)
{

	user_pet_task_set_in temp = {};
	memcpy(&temp, p_in, sizeof(user_pet_task_set_in));
	uint32_t ret = 0;
	if (p_in->stage_flag < 1 || p_in->stage_flag > 3) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	uint32_t i = 0;
	/* 接第一阶段任务  */
	if (p_in->stage_flag == 1) {
		if ((flag & 0x01) == 0x01) {
			for (i = 0; i < SUPER_NUM; i++) {
				
				if (super_id[i] == p_in->taskid) {
					break;
				}
			}
			if (i == SUPER_NUM) {
				return YOU_HAVE_NOT_RIGHT_ERR;
			}
		}
		if ((flag & 0x01) == 0x00) {
			for (i = 0; i < LAMU_NUM; i++) {
				if (lamu_id[i] == p_in->taskid) {
					break;
				}
			}
			if (i == LAMU_NUM) {
				return YOU_HAVE_NOT_RIGHT_ERR;
			}
		}

		uint32_t count = 0;
		ret = this->get_count(userid, p_in->petid, &count, flag);
		if (count != 0) {
			return YOU_HAVE_ATTEND_MAGIC_ERR;
		}
		ret = this->set_info(userid, &temp);
		if (ret != 0) {
			return ret;
		}
	} 
	/* 接第二阶段任务 */
	if (p_in->stage_flag == 2) {
		uint32_t first_time = 0, step = 0;
		ret = this->get_magic(userid, p_in->petid, p_in->taskid, &first_time, &step);
		if (ret != SUCC) {
			return ret;
		}
		if ((step != 2) || (time(NULL) - first_time) < 3600 * 24) {
			return PET_TIME_IS_NOT_ENOUGH_ERR;
		}
		ret = this->update_magic(userid, p_in->petid, p_in->taskid, "step", 3);
		if (ret != SUCC) {
			return ret;
		}
	}
	/* 接考核任务 */
	if (p_in->stage_flag == 3) {
		 uint32_t first_time = 0, step = 0;
		 ret = this->get_magic(userid, p_in->petid, p_in->taskid, &first_time, &step);
		 if (ret != SUCC) {
			 return ret;
		 }
		 if (step != 4) {
			 return YOU_HAVE_NOT_FINISH_SECOND_STAGE_ERR;
		 }
		ret = this->update_magic(userid, p_in->petid, p_in->taskid, "step", 5);
	}
	return ret;
}

int Cuser_task::update(userid_t userid, user_pet_task_set_in *p_in)
{
	int ret; 
	uint32_t usetime;

	ret=this->get_usetime(userid,p_in->petid,p_in->taskid, &usetime );
	if (ret!=SUCC) return ret;
	
	if (p_in->usetime+usetime >= p_in->def_usetime)	{
		p_in->usetime=p_in->def_usetime;
		p_in->flag=p_in->def_flag;
	}else{
		p_in->usetime=usetime+ p_in->usetime ;
	}

	
	sprintf( this->sqlstr, " update %s set \
		flag=%u ,\
		usetime=%u ,\
		starttime=%u ,\
		endtime=%u \
		where userid=%u and petid=%u and taskid=%u " ,
		this->get_table_name(userid), 
		p_in->flag,
		p_in->usetime,
		p_in->startime,
		p_in->endtime,
		userid, p_in->petid ,p_in->taskid
		);
	STD_SET_RETURN_EX(this->sqlstr,USER_TASK_NOFIND_ERR);	
}

int Cuser_task::set_info(userid_t userid,  user_pet_task_set_in *p_in)
{
	int ret;
	ret=this->update(userid,p_in);	
	if (ret!=SUCC)
	{
		ret=this->add(userid,p_in );
	}
	return ret;
}

int Cuser_task::get_usetime(userid_t userid, uint32_t petid, 
	   uint32_t taskid,	uint32_t *p_usetime )
{

	sprintf( this->sqlstr, "select usetime  from %s  \
		where userid=%u and petid=%u and taskid=%u " ,
		this->get_table_name(userid), userid,petid,taskid );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_TASK_NOFIND_ERR);
	 	INT_CPY_NEXT_FIELD(*p_usetime);
	STD_QUERY_ONE_END();
}


/* @brief  删除任务，并且返回目前拉姆目前学习的课程数 
 * @param  userid 米米好
 * @param  petid  宠物的ID号
 * @param  taskid 任务ID号
 * @param  *p_count 返回拉姆目前学习的任务数目
 */
int Cuser_task::del(userid_t userid, uint32_t petid, uint32_t taskid, uint32_t *p_count)
{
	uint32_t ret = this->delete_task(userid, petid, taskid);
	if (ret != SUCC) {
		return ret;
	}
	ret = this->get_all_count(userid, 101, 104, p_count);
	if (ret != SUCC) {
		return ret;
	}
	return ret;
}

/* @brief 删除给定的任务 
 * @param userid 米米号
 * @param petid  宠物号
 * @param taskid 任务号
 */
int Cuser_task::delete_task(userid_t userid, uint32_t petid, uint32_t taskid)
{
	sprintf( this->sqlstr, "delete from %s where userid = %u and petid = %u and taskid = %u", 
			this->get_table_name(userid), 
			userid,
			petid,
			taskid
	   	);
	STD_INSERT_RETURN(this->sqlstr,SUCC);	
}

/* @brief 得到宠物的任务状态
 * @param userid 米米号
 * @param p_in  输入的参数（petid如果为零，得到拉姆的全部任务信息）
 * @param pp_out_item 返回拉姆的任务信息
 */
int Cuser_task::get_pet_magic(userid_t userid, user_task_get_pet_task_in *p_in,
		uint32_t *p_count, user_task_get_pet_task_out_item	**pp_out_item )
{
	char  petidstr[100];
	if (p_in->petid==0){
		strcpy(petidstr,"true" );
	}else{
		sprintf(petidstr, "petid=%u",p_in->petid);
	}

	sprintf( this->sqlstr, "select petid, taskid,\
			first_time, step, client from %s \
			where userid=%u and %s and  taskid>=%u and taskid<=%u", 
			this->get_table_name(userid),
			userid,
			petidstr,
			p_in->start_taskid,
			p_in->end_taskid 
			); 
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_out_item, p_count ) ;
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->petid); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->taskid); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->time); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->step); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->client); 
	STD_QUERY_WHILE_END();
}

/* @breif 得到拉姆魔法任务的状态 
 * @param userid 米米号
 * @param petid 宠物的ID号
 * @param taskid 任务的ID号
 * @parm p_step 返回任务到了哪一步
 */
int Cuser_task::get_pet_magic(userid_t userid, uint32_t petid, uint32_t taskid, uint32_t *p_step)
{

	sprintf( this->sqlstr, "select step from %s \
			where userid=%u and petid = %u and taskid=%u", 
			this->get_table_name(userid),
			userid,
			petid,
			taskid
			); 
	STD_QUERY_ONE_BEGIN( this->sqlstr, USER_TASK_NOFIND_ERR) ;
		INT_CPY_NEXT_FIELD(*p_step); 
	STD_QUERY_ONE_END();
}


/**
 * @brief 得到宠物任务的状态 
 * @param userid 米米号
 * @param petid  宠物的ID号
 * @param pp_out_item 返回拉姆的任务的ID号和状态
 * @param 返回任务的个数
 */
int Cuser_task::get_taskid_state(userid_t userid, uint32_t petid,
		pet_task_state **pp_out_item, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select taskid, flag from %s where userid=%u and petid = %u", 
			this->get_table_name(userid),
			userid,
			petid
		   ); 
	STD_QUERY_WHILE_BEGIN(this->sqlstr,pp_out_item, p_count) ;
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->taskid); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->state); 
	STD_QUERY_WHILE_END();
}

/* @brief 得到用户宠物的任务信息 
 * @param userid 用户的米米号
 * @param pp_out_item 返回宠物的任务信息
 * @param p_count 任务的数目
 */
int Cuser_task::get_all_field(userid_t userid, user_magic_task_pet_get_all_out_item **pp_out_item, uint32_t *p_count)
{
	
	sprintf(this->sqlstr, "select petid, taskid, flag,usetime,starttime,endtime, first_time, client, step from %s \
			where userid=%u", 
			this->get_table_name(userid),userid); 
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_out_item, p_count ) ;
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->petid); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->taskid); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->flag); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->usetime); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->startime); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->endtime); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->first_time); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->client); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->step); 
	STD_QUERY_WHILE_END();
}

/* @brief 更新宠物任务的信息
 * @userid 米米号
 * @p_in 输入参数(宠物号和任务号)
 */
int Cuser_task::update_all_field(userid_t userid, user_magic_task_pet_update_in *p_in)
{
	
	sprintf(this->sqlstr, "update %s set flag = %u, usetime = %u, starttime = %u,\
			               endtime = %u, first_time = %u, client = %u, step = %u\
						   where userid=%u and petid = %u and taskid = %u", 
			this->get_table_name(userid),
			p_in->flag,
			p_in->usetime,
			p_in->starttime,
			p_in->endtime,
			p_in->first_time,
			p_in->client,
			p_in->step,
			userid,
			p_in->petid,
			p_in->taskid
		 ); 
	STD_SET_RETURN_EX(this->sqlstr,USER_TASK_NOFIND_ERR);	
}




