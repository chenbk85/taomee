/*
 * =====================================================================================
 *
 *       Filename:  Cadmin_all.cpp
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
#include "Cadmin_all.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
Cadmin_all::Cadmin_all(mysql_interface * db ) 
	:CtableWithKey( db, "ADMIN_DB","t_admin_all","adminid" )
{ 

}

int Cadmin_all::insert( admin_add_admin_in *p_in  )
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql,p_in->nick ,NICK_LEN);
	char passwd_mysql[mysql_str_len(PASSWD_LEN)];
	set_mysql_string(passwd_mysql ,p_in->passwd ,PASSWD_LEN);

	sprintf( this->sqlstr, "insert into %s values\
		   	(%u,'%s', '%s',1)", 
		this->get_table_name(), p_in->adminid,nick_mysql,passwd_mysql);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cadmin_all::get_admin_alllist(uint32_t index,uint32_t *p_count,  admin_all_item ** pp_list)
{
	sprintf( this->sqlstr, "select adminid, flag, nick\
			from %s order by adminid limit  %u, 200 ", 
		this->get_table_name(), index);
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, p_count ) ;
		(*pp_list+i)->adminid= atoi_safe(NEXT_FIELD);
		(*pp_list+i)-> flag=atoi_safe(NEXT_FIELD);
		strncpy((*pp_list+i)->nick,NEXT_FIELD,NICK_LEN);
	STD_QUERY_WHILE_END();
}

int Cadmin_all::user_check(userid_t  adminid,  char * passwd,char * nick )
{
	uint32_t flag;
	sprintf( this->sqlstr, "select  passwd,nick,flag \
			from %s where adminid=%u", 
			this->get_table_name(),adminid );
	char  db_passwd[PASSWD_LEN] ;
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD ( db_passwd, sizeof (db_passwd ) );
		BIN_CPY_NEXT_FIELD ( nick, NICK_LEN);
		INT_CPY_NEXT_FIELD(flag);
	STD_QUERY_ONE_END_WITHOUT_RETURN();
	if (memcmp(db_passwd ,passwd,PASSWD_LEN)!=0) return CHECK_PASSWD_ERR ;
	if (( flag&0x01)==0 ) return USER_NOT_ENABLED_ERR; 
	return SUCC;
}

int Cadmin_all::get_adminid_by_nick(char *nick, uint32_t * p_adminid )
{

	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql,nick,NICK_LEN);
	sprintf( this->sqlstr, "select adminid \
			from %s where nick='%s'", 
			this->get_table_name(),nick_mysql);

	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		*p_adminid=atoi_safe(NEXT_FIELD);	
	STD_QUERY_ONE_END();
}

int Cadmin_all::change_passwd(userid_t  adminid,  char * oldpasswd,char *newpasswd)
{
	char mysql_passwd[mysql_str_len(PASSWD_LEN)];
	set_mysql_string(mysql_passwd,newpasswd ,PASSWD_LEN);
	char nick[NICK_LEN];
	int ret;
	ret=this->user_check( adminid, oldpasswd,nick);
	if (ret!=SUCC) return ret;
	sprintf( this->sqlstr, " update %s set  passwd='%s' where adminid=%u " ,
			this->get_table_name(), mysql_passwd, adminid );
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}

int Cadmin_all::del(userid_t adminid)
{
	sprintf( this->sqlstr, "delete from %s where adminid=%u " ,
			this->get_table_name(),  adminid); 
	STD_REMOVE_RETURN_EX (this->sqlstr,USER_ID_NOFIND_ERR);
}

int Cadmin_all::update_passwd(userid_t adminid, char * newpasswd  )
{
	char mysql_passwd[mysql_str_len(PASSWD_LEN)];
	set_mysql_string(mysql_passwd,newpasswd,PASSWD_LEN);
	sprintf( this->sqlstr, " update %s set  passwd='%s' where adminid=%u " ,
			this->get_table_name(), mysql_passwd, adminid );
	STD_SET_RETURN(this->sqlstr,adminid, USER_ID_NOFIND_ERR );	
}

int Cadmin_all::update_flag(userid_t adminid ,const char * flag_type  , uint32_t  flag)
{
	sprintf( this->sqlstr, "update %s set %s =%u where adminid=%u " ,
		this->get_table_name(), flag_type,flag,adminid );
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}


int Cadmin_all::get_flag(userid_t adminid ,const char * flag_type   ,  uint32_t * flag)
{
	sprintf( this->sqlstr, "select  %s from %s where adminid=%u ", 
		flag_type, this->get_table_name(),adminid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
			*flag=atoi_safe(NEXT_FIELD);
	STD_QUERY_ONE_END();
}


int Cadmin_all::set_flag( userid_t adminid  ,const  char * flag_type  ,  uint32_t flag_bit ,  bool is_true )
{
	uint32_t flag;
	int ret;
	bool old_is_true;
	if ((ret =this->get_flag(adminid, flag_type ,&flag))!=SUCC){
			return ret;
	}	
	DEBUG_LOG("---flag[%u]--bit[%u]--&[%u]-",flag,flag_bit,flag & flag_bit );
	if ( (flag & flag_bit) == flag_bit ) old_is_true=true;
	else old_is_true =false; 

	if (old_is_true==is_true){
		//已经设置了
		return FLAY_ALREADY_SET_ERR;	
	}
		
	if (is_true ) flag+=flag_bit;  		
	else flag-=flag_bit;
	return this->update_flag(adminid, flag_type ,flag);
}

