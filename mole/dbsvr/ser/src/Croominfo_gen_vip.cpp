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

#include "Croominfo_gen_vip.h"

Croominfo_gen_vip :: Croominfo_gen_vip(mysql_interface *db)
	              : CtableRoute10x10(db, "ROOMINFO", "t_gen_vip", "userid")
{

}


int Croominfo_gen_vip :: update_time(userid_t userid, uint32_t count, uint32_t consume)
{
	uint32_t now = time(0);
	sprintf(this->sqlstr, "update %s set time = %u, count = %u, consume = %u where userid = %u",
			this->get_table_name(userid),
			now,
			count,
			consume,
			userid
			);
	STD_SET_RETURN_EX (this->sqlstr, DB_ERR);
}


int Croominfo_gen_vip :: insert(userid_t userid)
{
	sprintf(this->sqlstr, "insert into %s values(%u, 0, 0, 0)",
			this->get_table_name(userid),
			userid
			);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}

int Croominfo_gen_vip::del(userid_t userid )
{
	sprintf(this->sqlstr, "delete from %s where userid = %u ",
			this->get_table_name(userid), userid
			);
	STD_SET_RETURN_EX(this->sqlstr, DB_SUCC);
}

int Croominfo_gen_vip :: set_get_time(userid_t userid, uint32_t *p_oldtime, uint32_t *p_count, uint32_t *p_consume)
{
	int ret = this->get_time(userid, p_oldtime, p_count, p_consume);
	if (ret != SUCC) {
		ret = this->insert(userid);
		if (ret != SUCC) {
			return ret;
		}
		*p_oldtime = 0;
		*p_count = 0;
		*p_consume = 0;
	}
	return SUCC;
}

int Croominfo_gen_vip :: get_time(userid_t userid, uint32_t *p_oldtime, uint32_t *p_count, uint32_t *p_consume)
{
	sprintf(this->sqlstr, "select time, count, consume from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	 STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
	 	INT_CPY_NEXT_FIELD(*p_oldtime);
	 	INT_CPY_NEXT_FIELD(*p_count);
		INT_CPY_NEXT_FIELD(*p_consume);
	 STD_QUERY_ONE_END();
}


int Croominfo_gen_vip :: day_between(const time_t pre_time)
{
    struct tm *yymmdd;
    time_t old_time;
    time_t now_time;

    yymmdd = localtime(&pre_time);
    yymmdd->tm_sec = 0;
    yymmdd->tm_min = 0;
    yymmdd->tm_hour = 0;
    old_time = mktime(yymmdd);

    now_time = time(NULL);
    yymmdd = localtime(&now_time);
    yymmdd->tm_sec = 0;
    yymmdd->tm_min = 0;
    yymmdd->tm_hour = 0;
    now_time = mktime(yymmdd);
    return ((now_time - old_time) / (60 * 60 * 24));
}

int Croominfo_gen_vip :: gen_vip(userid_t userid, uint32_t *p_flag)
{
	uint32_t old_time = 0;
	uint32_t count = 0;
	uint32_t consume = 0;
	int ret = this->set_get_time(userid, &old_time, &count, &consume);
	if (ret != SUCC) {
		return ret;
	}

	//if (count >= 2 && consume >= 0) {
	//	return OUT_OF_MAX_COUNT_ERR;
	//}
	*p_flag = 0;
	int day = day_between(old_time);
	if (day > 0) {
		if (rand() % 100 < 50) {
			if(consume > 0) {
				ret = this->update_time(userid, count, consume);
				if (ret != SUCC) {
					return ret;
				}
				return YOU_LOST_THIS_TIME_ERR;
			}
			if (rand() % 100 < 40) {
				*p_flag = 2;
				consume++;
			}
			DEBUG_LOG("consume %u", userid);
		} else {
			/*if (count >= 2) {
				ret = this->update_time(userid, count, consume);
				if (ret != SUCC) {
					return ret;
				}
				return YOU_LOST_THIS_TIME_ERR;
			}*/
			if ((rand() % 100) < 2) {
				*p_flag = 1;
			}
		}
	} else {
		return HAVE_DONE_THIS_TODAY_ERR;
	}
	ret = this->update_time(userid, count, consume);
	if (ret != SUCC) {
		return ret;
	}
	return SUCC;
}

int Croominfo_gen_vip :: check_done_today(userid_t userid, uint32_t *p_flag)
{
	uint32_t old_time;
	uint32_t count;
	uint32_t consume;
	*p_flag = 0;
	int ret = this->set_get_time(userid, &old_time, &count, &consume);
	if (ret != SUCC) {
		return ret;
	}
	if (count >= 2 && consume >= 0) {
		*p_flag = 1;
		return OUT_OF_MAX_COUNT_ERR;
	}
	int day = day_between(old_time);
	if (day == 0) {
		*p_flag = 1;
		return HAVE_DONE_THIS_TODAY_ERR;
	}
	return SUCC;
}
