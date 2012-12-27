/*
 * =====================================================================================
 *
 *       Filename:  Cincrement.cpp
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
#include "Cincrement.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"

//create
Cincrement::Cincrement(mysql_interface * db ) :
	CtableRoute ( db,"INCREMENT_DB","t_increment","userid"),
	damee_history(db)
{ 
	
}
int Cincrement::insert(TINCREMENT *t  )
{
	sprintf(this->sqlstr, "insert into %s values (%u,%u,%u,%u,%u,%u,%u,%u)", 
			this->get_table_name(t->userid), t->userid,t->flag, t->damee,
			t->month_paytype,t->month_used, t->month_enabledate, t->month_duedate,
			t->month_nexterrcount);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR );	
}

int Cincrement::set_month(  userid_t userid, uint32_t paytype, uint32_t used, 
	 uint32_t enabledate, uint32_t duedate, uint32_t  nexterrcount)
{
	sprintf( this->sqlstr, "update %s set month_paytype=%u, month_used=%u, \
			month_enabletime=%u,month_duetime= %u, month_nexterrcount=%u  \
			where userid=%u ", 
			this->get_table_name(userid), paytype,used,  
			enabledate,duedate, nexterrcount ,userid );
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);	
}


int Cincrement::init_user(userid_t userid,uint32_t damee )
{
	TINCREMENT t;
	memset(&t,0, sizeof(t) );
	t.userid=userid;
	t.damee=damee;
	return this->insert(&t);			
}

int Cincrement::set_damee(userid_t userid, int32_t value, uint32_t* p_leave_damee ,
	uint32_t paytype,char*pri_msg )
{
	int damee_tmp;
	//get mee
	sprintf( this->sqlstr, "select damee from %s  where userid=%u " ,
		this->get_table_name(userid),userid );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR );
		 damee_tmp=atoi_safe(NEXT_FIELD );
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	if (value==0){ //不修改,返回
		*p_leave_damee	= damee_tmp;
		return SUCC;
	}

	damee_tmp +=value;
	//check value
	if(damee_tmp <0 ||damee_tmp >VALUE_MAX || value>VALUE_MAX ){
		return VALUE_OUT_OF_RANGE_ERR;	
	}
	*p_leave_damee=damee_tmp;
	//update  mee
	sprintf( this->sqlstr, "update %s  set damee=%u  where userid=%u " ,
		this->get_table_name(userid), damee_tmp,userid);
	STD_SET_WITHOUT_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);	
	//log to history
	int ret=this->damee_history. gen_record(time(NULL),
		paytype,userid,value,*p_leave_damee ,pri_msg);
	if (ret!=SUCC){
		DEBUG_LOG("__ERROR__  LOG DAMEE  paytype[%u] userid[%u] damee[%d],ret[%d]", 
			paytype,userid,value,ret);
	
	}
	return SUCC;
}

int Cincrement::isMonthed(userid_t userid,uint32_t * p_duetime)
{
	sprintf( this->sqlstr, "select month_duetime  from %s  \
			where userid=%u  and month_used=1 ",
		this->get_table_name(userid),userid );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR );
		*p_duetime=atoi_safe(NEXT_FIELD );
	STD_QUERY_ONE_END();
}

int Cincrement::set_del_month(userid_t userid)
{
	uint32_t duetime;
	int ret;
	if ( (ret=this->isMonthed(userid, &duetime))!=SUCC)   return ret;
	sprintf( this->sqlstr, "update %s set month_used=%u where userid=%u ", 
		this->get_table_name(userid),0, userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cincrement::set_no_auto_month(userid_t userid)
{
	uint32_t db_used, db_nexterrcount;	

	//get db data
	sprintf( this->sqlstr, "select month_used,month_nexterrcount from %s  \
			where  userid=%u " ,
	this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR );
			db_used=atoi_safe(NEXT_FIELD );
			db_nexterrcount=atoi_safe(NEXT_FIELD);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	if (!(db_used==1 && db_nexterrcount < NOVIP_NEXTERRCOUNT) ){
		return IS_NOT_MONTHED_ERR;
	}

	//将续期错误次数加到无效
	sprintf( this->sqlstr, "update %s set month_nexterrcount=%u where userid=%u ", 
		this->get_table_name(userid),NOVIP_NEXTERRCOUNT, userid );
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cincrement::get_record(userid_t userid, USER_PAY_INFO * p_out  )
{
	sprintf( this->sqlstr, "select damee,month_paytype, \
		  	month_used,month_enabletime, month_duetime,month_nexterrcount  from %s  \
			where  userid=%u  " ,
	this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr,TRANS_ID_NOFIND_ERR );
			p_out-> damee =atoi_safe(  NEXT_FIELD  );
			p_out-> month_paytype =atoi_safe (NEXT_FIELD );
			p_out-> month_used =atoi_safe (NEXT_FIELD );
			p_out->month_enabledate=atoi_safe (NEXT_FIELD );
			p_out-> month_duedate=atoi_safe (NEXT_FIELD );
			p_out-> month_nexterrcount =atoi_safe (NEXT_FIELD );
	STD_QUERY_ONE_END();
}

int Cincrement::update_duetime(userid_t userid,uint32_t newduetime, uint32_t nexterrcount )
{
	sprintf( this->sqlstr, "update %s set  month_duetime=%u, month_nexterrcount=%u  where userid=%u and month_used=1", 
		this->get_table_name(userid),newduetime ,nexterrcount, userid );
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}
char * Cincrement ::get_table_name(uint32_t id)
{
    sprintf (this->db_table_name,"%s.%s_%02d",
    this->db_name_pre, this->table_name_pre,id%100);
    return this->db_table_name;
}

