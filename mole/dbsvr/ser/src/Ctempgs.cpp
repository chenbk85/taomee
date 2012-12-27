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
#include "Ctempgs.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
//#define INIT_SHM_FLAG  -100 
//create
Ctempgs::Ctempgs (mysql_interface * db ) :CtableRoute100( db, "TEMPGS_DB","t_tempgs","userid" )
{ 

}

int Ctempgs::get_value(userid_t userid, stru_tempgs* p_tempgs  )
{
	sprintf( this->sqlstr, "select  task, count\
			from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		p_tempgs->task=atoi_safe(NEXT_FIELD);
		p_tempgs->count=atoi_safe(NEXT_FIELD);
	STD_QUERY_ONE_END();
}

int Ctempgs::update_task(userid_t userid, uint32_t task )
{
	sprintf( this->sqlstr, " update %s set \
		task=%u \
		where userid=%u " ,
		this->get_table_name(userid), 
		task,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}
int Ctempgs::add_count(userid_t userid)
{
	sprintf( this->sqlstr, " update %s set \
		count=count+1 \
		where userid=%u " ,
		this->get_table_name(userid), 
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}


int Ctempgs::insert(userid_t userid, stru_tempgs* p_tempgs )
{
	sprintf( this->sqlstr, "insert into %s values (%u,%u,%u)",
	this->get_table_name(userid),userid,p_tempgs->task,p_tempgs->count);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);	
}

int Ctempgs::get_value_ex(userid_t userid, stru_tempgs* p_out)
{
	int ret;
	stru_tempgs tempgs;
	ret=this->get_value(userid,&tempgs );
	if (ret!=SUCC){
		memset(&tempgs,0,sizeof(tempgs) );
		this->insert(userid,&tempgs);
		p_out->task=0;
		p_out->count=0;
		return SUCC;
	}else{
		p_out->count=tempgs.count;
		p_out->task= tempgs.task;
		return SUCC;
	}
}
