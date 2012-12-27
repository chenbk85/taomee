/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_gen_vip.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/08/2009 09:21:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
extern "C" {
#include <time.h>
}

#include "Croominfo_work.h"


Croominfo_work :: Croominfo_work(mysql_interface *db)
	              : CtableRoute10x10(db, "ROOMINFO", "t_roominfo_work", "userid")
{

}



int Croominfo_work :: get_week_id(time_t now, uint32_t *p_week_id)
{
    now += 86400 * 3;
    char buffer[10];
    strftime(buffer, 10, "%W", localtime(&now));
    *p_week_id = atoll(buffer);
	return SUCC;
} 

int Croominfo_work :: update(userid_t userid, uint32_t type)
{
	uint32_t week_id = 0;
	time_t now = time(NULL);
	this->get_week_id(now, &week_id);
	char str[10];
	if (type == 1) {
		strcpy(str, "tom");
	} else {
		strcpy(str, "nick");
	}
	sprintf(this->sqlstr, "update %s set time_%s = time_%s + 1 where userid = %u and week_id = %u",
			this->get_table_name(userid),
			str,
			str,
			userid,
			week_id
			);
	STD_SET_RETURN_EX (this->sqlstr, DB_ERR);
}


int Croominfo_work :: insert(userid_t userid, uint32_t type)
{
	uint32_t week_id = 0;
	time_t now = time(NULL);
	this->get_week_id(now, &week_id);
	uint32_t tom = 0, nick = 0;
	if (type == 1) {
		tom = 1;
	} else {
		nick = 1;
	}
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			week_id,
			tom, 
			nick
		   );
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}


int Croominfo_work :: set_value(userid_t userid, int32_t type)
{
	uint32_t ret = this->update(userid, type);
	if (ret != SUCC) {
		ret = this->insert(userid, type);
		if (ret != SUCC) {
			return ret;
		}
	}	
	return SUCC;
}

int Croominfo_work :: get_value_last(userid_t userid, uint32_t *p_tom, uint32_t *p_nick) 
{
	uint32_t week_id = 0;
	time_t now  = time(NULL) - 86400 * 7;
	this->get_week_id(now, &week_id);
	sprintf(this->sqlstr, "select time_tom, time_nick from %s where userid = %u and  week_id = %u",
			this->get_table_name(userid),
			userid,
			week_id
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(*p_tom);
		INT_CPY_NEXT_FIELD(*p_nick);
    STD_QUERY_ONE_END();
}

int Croominfo_work :: get_value_this(userid_t userid, uint32_t *p_tom, uint32_t *p_nick) 
{
	uint32_t week_id = 0;
	time_t now  = time(NULL);
	this->get_week_id(now, &week_id);
	sprintf(this->sqlstr, "select time_tom, time_nick from %s where userid = %u and  week_id = %u",
			this->get_table_name(userid),
			userid,
			week_id
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(*p_tom);
		INT_CPY_NEXT_FIELD(*p_nick);
    STD_QUERY_ONE_END();
}


