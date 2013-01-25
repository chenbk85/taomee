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

#include "Crelation.h"
#include <time.h>
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"
#include <algorithm>
#include "Ccommon.h"

Crelation::Crelation(mysql_interface * db ) 
	:CtableRoute100x10(db, "MOLE2_USER" , "t_relation" , "userid")
{

}

int Crelation::get_relation_info(uint32_t userid,mole2_get_relation_out *info)
{
	int count = 0;
	stru_relation_item item;
	priv_relation_t buf[5]={{0}};

	GEN_SQLSTR(this->sqlstr,"select flag,exp,graduation,total_val,count,relations from %s where userid=%u FOR UPDATE",
		this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(info->flag);
		INT_CPY_NEXT_FIELD(info->exp);
		INT_CPY_NEXT_FIELD(info->graduation);
		INT_CPY_NEXT_FIELD(info->total_val);
		INT_CPY_NEXT_FIELD(count);
		BIN_CPY_NEXT_FIELD(buf,sizeof(buf));
		for(int i = 0; i < count; i++) {
			item.uid = buf[i].uid;
			item.day = buf[i].day;
			item.val = buf[i].val;
			info->relations.push_back(item);
		}
	STD_QUERY_ONE_END();
}

int Crelation::set_relation_info(uint32_t userid,mole2_get_relation_out *info)
{
	priv_relation_t buf[5]={{0}};
	char buff_mysql[mysql_str_len(sizeof(buf))];

	for(uint32_t i = 0; i < info->relations.size();i++) {
		buf[i].uid = info->relations[i].uid;
		buf[i].day = info->relations[i].day;
		buf[i].val = info->relations[i].val;
	}

	set_mysql_string(buff_mysql, (char*)(buf), sizeof(buf));
	GEN_SQLSTR(this->sqlstr,"insert into %s values(%u,%u,%u,%u,%u,%u,'%s') "
		"ON DUPLICATE KEY UPDATE flag=%u,exp=%u,graduation=%u,total_val=%u,count=%u,relations='%s'",
		this->get_table_name(userid),userid,info->flag,info->exp,info->graduation,info->total_val,(uint32_t)info->relations.size(),buff_mysql,
		info->flag,info->exp,info->graduation,info->total_val,(uint32_t)info->relations.size(),buff_mysql);
	return this->exec_update_sql(this->sqlstr, SUCC);
}

