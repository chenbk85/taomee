/*
 * ===============
 *
 *       Filename:  Cuser_sn.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年03月03日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  tommychen, tommychen@taomee.com
 *        Company:  TAOMEE
 *
 * ==============
 */
#include "Cuser_sn.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"

//create
Cuser_sn::Cuser_sn(mysql_interface * db ):Ctable(db,"NOAH","t_user_sn")
{ 
}

int Cuser_sn::insert(char* sn,userid_t userid )
{
	char sn_mysql[mysql_str_len(NOAH_SN_LEN)];
	set_mysql_string(sn_mysql, sn, NOAH_SN_LEN);
	sprintf( this->sqlstr, "insert into %s (sn, userid) values ('%s',%u)", 
			this->get_table_name(), 
			sn_mysql,
			userid
	);
	STD_INSERT_RETURN(this->sqlstr, SN_EXISTED_ERR);	
}


int Cuser_sn::getuserid(char* sn,userid_t * userid )
{
	char sn_mysql[mysql_str_len(NOAH_SN_LEN)];
	set_mysql_string(sn_mysql, sn, NOAH_SN_LEN);
	sprintf( this->sqlstr, "select userid  from %s where sn='%s' ", 
			this->get_table_name(),sn_mysql );
	STD_QUERY_ONE_BEGIN(this->sqlstr, SN_NOFIND_ERR);
			*userid=atoi_safe(NEXT_FIELD );
	STD_QUERY_ONE_END();
}

int Cuser_sn::remove(char* sn,userid_t userid)
{
	char sn_mysql[mysql_str_len(NOAH_SN_LEN)];
	set_mysql_string(sn_mysql, sn, NOAH_SN_LEN);
	sprintf( this->sqlstr, "delete  from %s where sn='%s' and userid=%u ", 
			this->get_table_name(),sn_mysql,userid );
	STD_SET_RETURN_EX (this->sqlstr, SN_NOFIND_ERR);
}



