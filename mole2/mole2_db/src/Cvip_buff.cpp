/*
 * =====================================================================================
 *
 *       Filename:  Cuser.cpp
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
 *  common.h
 * =====================================================================================
 */

#include "Cvip_buff.h"
#include <time.h>
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"
#include <algorithm>
#include "Ccommon.h"

Cvip_buff::Cvip_buff(mysql_interface * db ) 
	:CtableRoute100x10(db, "MOLE2_USER" , "vip_buff" , "userid", "petid")
{

}

int Cvip_buff::del_vip_buff(userid_t userid,uint32_t petid)
{
	GEN_SQLSTR(this->sqlstr, "delete from %s where userid = %u and petid = %u",
		this->get_table_name(userid),userid,petid);
	return this->exec_update_sql(this->sqlstr, MOLE2_KEY_NOT_EXIST_ERR);
}

int Cvip_buff::set_vip_buff(userid_t userid,stru_vip_buff *parg)
{
	int ret;
	char buff_mysql[mysql_str_len(sizeof(parg->buff))];
	set_mysql_string(buff_mysql, (char*)(parg->buff), sizeof(parg->buff));

	GEN_SQLSTR(this->sqlstr, "update %s set buff = '%s' where userid = %u and petid=%u",
		this->get_table_name(userid),buff_mysql,userid,parg->petid);
	ret = this->exec_update_sql(this->sqlstr, MOLE2_KEY_NOT_EXIST_ERR);
	if(ret == MOLE2_KEY_NOT_EXIST_ERR) {
		GEN_SQLSTR(this->sqlstr, "insert into %s(userid,petid,buff) values(%u,%u,'%s')",
			this->get_table_name(userid),userid,parg->petid,buff_mysql);
		ret = this->exec_insert_sql(this->sqlstr, MOLE2_KEY_EXIST_ERR);
	}

	return ret;
}

int Cvip_buff::get_vip_buff_list(userid_t userid,mole2_get_vip_buff_in *p_in,mole2_get_vip_buff_out *p_out)
{
	GEN_SQLSTR(this->sqlstr, "select petid,buff from %s where userid=%u",
		this->get_table_name(userid),userid);

	int count = p_in->petid.size();
	if(count > 0) {
		int len = strlen(this->sqlstr);
		len += sprintf(this->sqlstr + len," and petid in(");
		do {
			len += sprintf(this->sqlstr + len,"%d,",p_in->petid[--count]);
		} while(count);
		this->sqlstr[len - 1] = ')';
	}

	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr,p_out->petlist);
		INT_CPY_NEXT_FIELD(item.petid);
		BIN_CPY_NEXT_FIELD(item.buff,sizeof(item.buff));
	STD_QUERY_WHILE_END_NEW();
}

int Cvip_buff::get_buffs(userid_t userid,std::vector<stru_vip_buff> &buffs)
{
	GEN_SQLSTR(this->sqlstr, "select petid,buff from %s where userid=%u",
		this->get_table_name(userid),userid);
	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr,buffs);
		INT_CPY_NEXT_FIELD(item.petid);
		BIN_CPY_NEXT_FIELD(item.buff,sizeof(item.buff));
	STD_QUERY_WHILE_END_NEW();
}

