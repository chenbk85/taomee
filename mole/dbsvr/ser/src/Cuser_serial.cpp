/*
 * =====================================================================================
 *
 *       Filename:  Cuser_serial.cpp
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
#include "Cuser_serial.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"


Cuser_serial::Cuser_serial(mysql_interface * db ) 
	:Ctable( db,"USER_SERIAL_DB","t_user_serial")
{ 

}

int Cuser_serial::gen_serial(char * user_serialid )
{
	int rid;	
	for (int i=0;i<8;i++)  {
		rid=random()%36;	
		if (rid<10){
			user_serialid[i]='0'+rid;	
		}else{
			user_serialid[i]='A'+(rid-10);	
		}
	}
	return SUCC;
}

int Cuser_serial::add_serial (userid_t userid,uint32_t type,  char * user_serialid)
{
	int ret;
	this->gen_serial(user_serialid);	
	ret=this->insert(userid,type,user_serialid);  
	if (ret==USER_ID_EXISTED_ERR){
		return this->update(userid,type,user_serialid);  
	}else{
		return ret;
	}
}

int Cuser_serial::insert(userid_t userid,uint32_t type,  char * user_serialid)
{
	sprintf( this->sqlstr, "insert into %s values (%u,%u,%u,'%8.8s' )",
		this->get_table_name(), userid,type,(uint32_t )time(NULL),user_serialid);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);	
}

int Cuser_serial::update(userid_t userid,uint32_t type,  char * user_serialid)
{
	sprintf( this->sqlstr, "update %s set gentime=%u ,serialid='%8.8s'\
		   	where userid=%u and type=%u ",
		this->get_table_name(), (uint32_t )time(NULL),user_serialid,userid,type);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cuser_serial::del(userid_t userid,uint32_t type)
{
	sprintf( this->sqlstr, "delete from  %s \
		   	where userid=%u and type=%u ", 
		this->get_table_name(), userid,type);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);	
}
int Cuser_serial::get_serialid(userid_t userid ,uint32_t type,  
		userserial_get_serial_out * p_out )
{
	sprintf( this->sqlstr, "select  serialid from %s \
		   	where userid=%u and type=%u ",
			this->get_table_name(),userid,type);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_out->serialid,sizeof (p_out->serialid));
	STD_QUERY_ONE_END();
}


