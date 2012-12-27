/*
 * =====================================================================================
 *
 *       Filename:  Ccup.cpp
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
#include "Cconfect.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
//#define INIT_SHM_FLAG  -100 
//create
Cconfect::Cconfect (mysql_interface * db ) :CtableRoute100( db, "CONFECT_DB","t_confect","userid" )
{ 

}

int Cconfect::get_value(userid_t userid, confect_stru * p_confect  )
{

	sprintf( this->sqlstr, "select flag, task, logdate, count_today,count_all \
			from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		p_confect->flag=atoi_safe(NEXT_FIELD);
		p_confect->task=atoi_safe(NEXT_FIELD);
		p_confect->logdate=atoi_safe(NEXT_FIELD);
		p_confect->count_today=atoi_safe(NEXT_FIELD);
		p_confect->count_all=atoi_safe(NEXT_FIELD);
	STD_QUERY_ONE_END();
}

int Cconfect::update_value(userid_t userid, confect_stru * p_confect )
{

	sprintf( this->sqlstr, " update %s set \
		flag=%u, \
		logdate=%u, \
		count_today=%u, \
		count_all=%u \
		where userid=%u " ,
		this->get_table_name(userid), 
		p_confect->flag,
		p_confect->logdate,
		p_confect->count_today,
		p_confect->count_all,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cconfect::update_count_all(userid_t userid, confect_set_value_in *p_in )
{

	if (p_in->count_all>200) p_in->count_all=200;
	sprintf( this->sqlstr, " update %s set \
		flag=%u, \
		task=%u, \
		count_all=%u \
		where userid=%u " ,
		this->get_table_name(userid), 
		p_in->flag,
		p_in->task,
		p_in->count_all,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cconfect::update_task(userid_t userid, uint32_t task )
{

	sprintf( this->sqlstr, " update %s set \
		task=%u \
		where userid=%u " ,
		this->get_table_name(userid), 
		task,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}


int Cconfect::add_count(userid_t userid, confect_change_value_in * p_in,
	 confect_change_value_out * p_out   	)
{

	uint32_t max_count_today=0; 
	uint32_t max_count_all=0; 
	int32_t leave_count_today=0; 
	int32_t leave_count_all=0; 
	int32_t leave_min=0; 
	
	uint32_t now=0; 
	bool 	insert_flag=false;
	confect_stru confect;
	int ret;
	ret=this->get_value(userid,&confect );
	if (ret==DB_ERR){
		return ret;
	}else if (ret==USER_ID_NOFIND_ERR) {
		//DB没有相关记录
		DEBUG_LOG("init confect" );
		confect.flag=0;
		confect.logdate=0;
		confect.count_today=0;
		confect.count_all=0;
 		insert_flag=true;//需要插入记录
	}else{
		DEBUG_LOG("ret=%d",ret);
	}
	//DEBUG_LOG("count_all:%u",confect.count_all);

	//设置当前日期
	now=get_date(time(NULL));
	if (now!=confect.logdate){
		confect.count_today=0;	
		confect.logdate=now;
	}

	//设置捡到的糖果类型
	confect.flag=(confect.flag | p_in->type );

	p_out->flag=confect.flag;

	//计算数量
	if (p_in->vipflag ==0) {
		max_count_today=50;
		max_count_all=100;
	}else{//VIP
		max_count_today=100;
		max_count_all=200;
	}
	if (p_in->value>0){//增加
		//可加的个数
		leave_count_today=max_count_today-confect.count_today;	
		leave_count_all=max_count_all-confect.count_all;	

		DEBUG_LOG("%u %u",leave_count_today,leave_count_all );
		if (leave_count_today<0 || leave_count_all<0  ) {
			leave_min=0;
		}else if(leave_count_today<=leave_count_all){
			leave_min=leave_count_today;
		}else{
			leave_min=leave_count_all;
		}
		DEBUG_LOG("leave_min %d",leave_min );

		if (leave_min >p_in->value){
			p_out->real_add_value=p_in->value;
		}else{
			p_out->real_add_value=leave_min;
		}			
		confect.count_today+=p_out->real_add_value;
		confect.count_all+=p_out->real_add_value;
	}else{//减少
		if (int (confect.count_all) + p_in->value >=0){//足够
			p_out->real_add_value=p_in->value;
		}else{
			p_out->real_add_value=-confect.count_all;
		}	
		confect.count_all+=p_out->real_add_value;
	}

	p_out->count_all=confect.count_all;
	if (!insert_flag){
		return this->update_value(userid,&confect );
	}else{
		return this->insert(userid,&confect );
	}

}


int Cconfect::get_value_ex(userid_t userid, confect_get_value_out* p_out)
{
	int ret;
	confect_stru confect;
	ret=this->get_value(userid,&confect );
	if (ret!=SUCC){
		memset(&confect,0,sizeof(confect) );
		this->insert(userid,&confect);
		p_out->flag=0;
		p_out->count_all=0;
		p_out->task=0;
		return SUCC;
	}
	p_out->flag=confect.flag;
	p_out->count_all=confect.count_all;
	p_out->task= confect.task;
	return SUCC;
}

int Cconfect::insert(userid_t userid, confect_stru * p_confect )
{
	sprintf( this->sqlstr, "insert into %s values (%u,%u,%u,%u,%u,%u)",
	this->get_table_name(userid),userid,p_confect->flag,p_confect->task,
		p_confect->logdate, p_confect->count_today , p_confect->count_all );
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);	
}

