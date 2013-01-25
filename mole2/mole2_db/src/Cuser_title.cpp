/*
 * =====================================================================================
 *
 *       Filename:  Cuser_title.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/16/2010 11:22:17 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include "Cuser_title.h"

#define STR_USERID	"userid"
#define STR_TITLEID	"titleid"
#define STR_TIME	"time"


Cuser_title::Cuser_title(mysql_interface * db)
	: CtableRoute(db, "MOLE2_USER", "t_user_title", "userid")
{
}

int Cuser_title::insert(uint32_t userid,
	uint32_t titleid, uint32_t time)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u, %u, %u)", 
			this->get_table_name(userid),
			userid,
			titleid,
			time);
	if( this->exec_insert_sql( this->sqlstr, USER_TITLE_TITLEID_EXISTED_ERR) == USER_TITLE_TITLEID_EXISTED_ERR);
	{
		GEN_SQLSTR(this->sqlstr, "update %s set %s = %u where %s = %u and %s = %u", 
			this->get_table_name(userid),
			STR_TIME,
			time ,
			STR_USERID,
			userid,
			STR_TITLEID,
			titleid);
		return this->exec_update_sql(this->sqlstr, USER_TITLE_TITLEID_NOFIND_ERR);
	}
	return 0;
}

int Cuser_title::delete_title(uint32_t userid, uint32_t titleid)
{
	GEN_SQLSTR(this->sqlstr, "delete from %s where userid=%u and titleid=%u", 
			this->get_table_name(userid),
			userid,
			titleid);
	return this->exec_update_sql(this->sqlstr, USER_TITLE_TITLEID_NOFIND_ERR);
}

int Cuser_title::title_set(uint32_t userid, uint32_t titleid_old, uint32_t titleid_new)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s = %u where %s = %u and %s = %u", 
			this->get_table_name(userid), 
			STR_TITLEID, titleid_new,
			STR_USERID,		userid, 
			STR_TITLEID,	titleid_old);
	return this->exec_update_sql(this->sqlstr, USER_TITLE_TITLEID_NOFIND_ERR);
}

int Cuser_title::title_list_get(userid_t userid, 
		stru_mole2_user_title_info** pp_out_item, uint32_t* p_count,
		uint32_t honorid, bool* p_use_flag )
{
	uint32_t date=today();
	uint32_t day=0;
	*p_use_flag=false;
	GEN_SQLSTR(this->sqlstr, "select %s,%s from %s where %s = %u",
		STR_TITLEID,
		STR_TIME,
		this->get_table_name(userid),
		STR_USERID, userid);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_item, p_count);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->titleid);
		INT_CPY_NEXT_FIELD(day);
		if( day!=0 && date >= day  )	
		{
			DEBUG_LOG("clear weekly title [id=%u]",(*pp_out_item + i)->titleid);
			(*pp_out_item + i)->titleid=0;
			continue;
		}
		if (honorid==(*pp_out_item + i)->titleid ) {
				*p_use_flag=true;
		}
	STD_QUERY_WHILE_END();
}

int Cuser_title::del(uint32_t userid, uint32_t titleid)
{
	GEN_SQLSTR(this->sqlstr, "delete from %s where %s = %u and %s = %u", 
			this->get_table_name(userid), 
			STR_USERID,		userid, 
			STR_TITLEID,	titleid);
	return this->exec_update_sql(this->sqlstr, USER_TITLE_TITLEID_NOFIND_ERR);
}

int Cuser_title::get_titles(uint32_t userid, std::vector<uint32_t> &titles)
{
	uint32_t date=today();
	uint32_t day=0;
	GEN_SQLSTR(this->sqlstr, "select titleid,time  from %s where %s = %u",
		this->get_table_name(userid),STR_USERID, userid);
	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, titles);
		INT_CPY_NEXT_FIELD(item);
		INT_CPY_NEXT_FIELD(day);
		if(date >= day)	
		{
			DEBUG_LOG("clear weekly title [id=%u]",item);
			continue;	
		}
	STD_QUERY_WHILE_END_NEW();
}

int Cuser_title::get_titles_ex(uint32_t userid, std::vector<title_t> &titles)
{
	GEN_SQLSTR(this->sqlstr, "select titleid,time from %s where userid = %u",
		this->get_table_name(userid), userid);
	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, titles);
		INT_CPY_NEXT_FIELD(item.titleid);
		INT_CPY_NEXT_FIELD(item.gen_time);
	STD_QUERY_WHILE_END_NEW();
}


