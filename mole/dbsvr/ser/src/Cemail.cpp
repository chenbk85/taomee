/*
 * ===============
 *
 *       Filename:  Cemail.cpp
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
 * ==============
 */
#include "Cemail.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"

//create
Cemail::Cemail(mysql_interface * db ) :CtableString( db,"EMAIL_DB","t_email_id")
{ 
}

int Cemail::insert(char* email,userid_t userid )
{
	this->set_email_str(email);	
	DEBUG_LOG("email[%s]", this->email_c);
	sprintf( this->sqlstr, "insert into %s values ('%s',%u)", 
			this->get_table_name(this->email_c), 
			this->email_mysql,
			userid
	);
	STD_INSERT_RETURN(this->sqlstr, EMAIL_EXISTED_ERR);	
}

int Cemail::map_email_userid (char* email,userid_t userid,userid_t * p_old_map_userid )
{
	int ret;	
	ret=this->insert(email, userid);	
	if (ret== EMAIL_EXISTED_ERR ){
		if (this->getuserid(email,p_old_map_userid)!=SUCC){
			return DB_ERR;
		}
	}
	return ret;
}

void Cemail::set_email_str(char * email )
{
	int i;
	for (i=0;i<EMAIL_LEN;i++ ){
		if (email[i]!='\0'&& email[i]!=' '){
			this->email_c[i]=email[i];
		}else{
			break;
		}
	}
	this->email_c[i]='\0';
	set_mysql_string(email_mysql,this->email_c, i);
}

int Cemail::update_email(userid_t userid, char* old_email, char* new_email)
{
	int ret;
	ret=this->insert(new_email,userid);
	if (ret==SUCC){
		ret=this->remove(old_email,userid);
		if (ret !=SUCC ){
			this->remove(new_email,userid);
		}
	}
	return ret;
}

int Cemail::getuserid(char* email,userid_t * userid )
{
	this->set_email_str(email);	
	DEBUG_LOG("email[%s]", this->email_c);
		sprintf( this->sqlstr, "select userid  from %s where email='%s' ", 
			this->get_table_name(email_c),email_mysql );
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			*userid=atoi_safe(NEXT_FIELD );
	STD_QUERY_ONE_END();
}

int Cemail::remove(char* email,userid_t userid)
{
	this->set_email_str(email);	
	sprintf( this->sqlstr, "delete  from %s where email='%s' and userid=%u ", 
			this->get_table_name(email_c),email_mysql,userid );
	STD_SET_RETURN_EX (this->sqlstr, EMAIL_NOFIND_ERR);
}
