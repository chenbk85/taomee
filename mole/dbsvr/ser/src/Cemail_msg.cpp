/*
 * =====================================================================================
 *
 *       Filename:  Cemail_msg.cpp
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
#include "Cemail_msg.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
//#define INIT_SHM_FLAG  -100 
//create
Cemail_msg::Cemail_msg(mysql_interface * db ) 
	:CtableRoute10x10( db, "EMAIL_SYS","t_email_msg","userid" )
{ 

}

int Cemail_msg::init(userid_t userid, email_msg *p_item)
{
	sprintf( this->sqlstr, "insert into %s values (%u,%u )", 
			this->get_table_name(userid),  
			userid,
			p_item->email_noread
	);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);	
}

int Cemail_msg::get_msg_noread_count(userid_t userid , emailsys_get_email_msg_out *p_out  )
{
	sprintf( this->sqlstr, "select  email_noread \
		  	from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->email_unread);
	STD_QUERY_ONE_END();
}


int Cemail_msg::init_email_noread(userid_t userid)
{
	sprintf( this->sqlstr, " update %s set \
					email_noread=0 \
		   			where userid=%u " ,
				this->get_table_name(userid), userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}



int Cemail_msg::set_add_msg_for_update(userid_t userid)
{
	sprintf( this->sqlstr, " update %s set \
					email_noread=email_noread+1\
		   			where userid=%u " ,
				this->get_table_name(userid), userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cemail_msg::set_read(userid_t userid)
{
	sprintf( this->sqlstr, " update %s set \
					email_noread=email_noread-1\
		   			where userid=%u " ,
				this->get_table_name(userid), userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	

}

int Cemail_msg::set_add_msg(userid_t userid )
{
	int ret=this->set_add_msg_for_update(userid );
	if (ret==USER_ID_NOFIND_ERR ){//记录不存在,就插入
		email_msg init_item;
		init_item.email_noread=1;
		ret=this->init(userid,&init_item);
	}
	return ret;
}	

