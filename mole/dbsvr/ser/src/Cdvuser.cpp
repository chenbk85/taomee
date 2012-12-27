/*
 * =====================================================================================
 *
 *       Filename:  Cdvuser.cpp
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
#include "Cdvuser.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
#include <stdint.h>
#include "iconv.h"
#include <strings.h>
#include <stdint.h>

Cdvuser::Cdvuser(mysql_interface * db ) :CtableWithKey( db, "dvbbs","dv_user", "username" )
{ 

}

int Cdvuser::insert( userid_t  userid , char *passwd ,uint32_t sex,char * nick, char * email  ) 
{
	
	char email_mysql[mysql_str_len(EMAIL_LEN)];
	char nick_mysql[mysql_str_len(NICK_LEN)];
	const char * userface[]= {
		"images/userface/image2.gif",
		"images/userface/image1.gif"
	};
	//bool existed;
	//int ret;
	//ret=this->id_is_existed(userid,&existed ); 
	//if(ret!=SUCC) return ret;
	//if (existed){
		//DEBUG_LOG("==find user==");
		//return USER_ID_EXISTED_ERR;
	//}

	//uint32_t joindate=time(NULL)-20*60;
	uint32_t joindate=time(NULL);
	set_mysql_string(email_mysql,
			set_space_end( email,EMAIL_LEN ), EMAIL_LEN);
	set_mysql_string(nick_mysql, 
			set_space_end( nick ,NICK_LEN ), NICK_LEN);
	if (sex!=0)	{ 
		sex=1; 
	}
	/*
	sprintf( this->sqlstr, "insert into %s (username, userpassword, usersex,\
		  userface ,useremail,joindate,nick ) values (\
		'%u','%s','%u','%s','%s',%u, '%s')", 
		this-> get_table_name(), userid,passwd,sex,userface[sex],email_mysql,joindate,nick_mysql
	 );
	*/

	sprintf( this->sqlstr, "insert into %s (username, userpassword, usersex,\
		  userface ,joindate,nick ) values (\
		'%u','%s','%u','%s', %u, '%s')", 
		this-> get_table_name(), userid,passwd,sex,userface[sex],joindate,nick_mysql);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);	
}

int Cdvuser::change_passwd(userid_t  userid , char *newpasswd)
{
	sprintf( this->sqlstr, " update %s set  userpassword='%s' where username='%u' " ,
			this->get_table_name(), newpasswd , userid);
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}

int Cdvuser::change_nick(userid_t userid ,  char *newnick)
{

	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql, 
			set_space_end( newnick ,NICK_LEN ), NICK_LEN);

	sprintf( this->sqlstr, " update %s set nick='%s' where username='%d'", 
		this-> get_table_name(), nick_mysql,userid);
	STD_SET_RETURN_EX(this->sqlstr, SUCC );	
}

