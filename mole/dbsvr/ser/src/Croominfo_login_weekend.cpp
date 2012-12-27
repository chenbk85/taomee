/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_login_weekend.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/10/2011 02:35:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croominfo_login_weekend.h"

static date_week_info  date_weeks[] = {
	{20111021, 34},
	{20111022, 34},
	{20111023, 34},
	{20111028, 35},
	{20111029, 35},
	{20111030, 35},
	{20111104, 36},
    {20111105, 36},
    {20111106, 36},
    {20111111, 37},
    {20111112, 37},
    {20111113, 37},
    {20111118, 38},
    {20111119, 38},
    {20111120, 38},
    {20111125, 39},
    {20111126, 39},
    {20111127, 39},
    {20111202, 40},
    {20111203, 40},
    {20111204, 40},
    {20111209, 41},
    {20111210, 41},
    {20111211, 41},
    {20111216, 42},
    {20111217, 42},
    {20111218, 42},
    {20111223, 43},
    {20111224, 43},
    {20111225, 43},
    {20111230, 44},
    {20111231, 44},
    {20120101, 44},
    {20120106, 45},
    {20120107, 45},
    {20120108, 45},
    {20120113, 46},
    {20120114, 46},
    {20120115, 46},
    {20120120, 47},
    {20120121, 47},
    {20120122, 47},
    {20120127, 48},
    {20120128, 48},
    {20120129, 48},
    {20120203, 49},
    {20120204, 49},
    {20120205, 49},
    {20120210, 50},
    {20120211, 50},
    {20120212, 50},
    {20120217, 51},
    {20120218, 51},
    {20120219, 51},
    {20120224, 52},
    {20120225, 52},
    {20120226, 52},
};

Croominfo_login_weekend::Croominfo_login_weekend(mysql_interface *db):
	CtableRoute10x10(db, "ROOMINFO","t_roominfo_login_weekend", 
			"userid")
{

}

/*
 * @brief 插入记录
 */
int Croominfo_login_weekend::insert(userid_t userid, uint32_t week_count, uint32_t last_week)
{

	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u)",
			this->get_table_name(userid), userid, week_count, last_week);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

/*
 * @brief 更新记录 
 */
int Croominfo_login_weekend::update(userid_t userid, uint32_t week_count, uint32_t last_week)
{	
	sprintf(this->sqlstr, "update %s set week_count = %u, last_week = %u  where userid = %u",
			this->get_table_name(userid), week_count, last_week, userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

/*
 * @brief 查询记录 
 */
int Croominfo_login_weekend::query(userid_t userid, uint32_t* week_count, uint32_t* last_week)
{
	sprintf(this->sqlstr, "select  week_count, last_week  from %s where userid = %u",
			this->get_table_name(userid), userid);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*week_count);
		INT_CPY_NEXT_FIELD(*last_week);
	STD_QUERY_ONE_END();
}

int Croominfo_login_weekend::update_week_count(userid_t userid)
{
	uint32_t today_date = get_date(time(NULL));
	int index = -1;
	uint32_t len = sizeof(date_weeks)/sizeof(date_week_info);
	uint32_t i = 0;
	for (i = 0; i < len ; i++)
	{
		if (today_date == date_weeks[i].date)
		{
			index = i;
			break;
		}
	}
	
	if (index < 0)
	{
		return SUCC;
	}

	uint32_t week_count = 0;
	uint32_t last_week = 0;
	query(userid, &week_count, &last_week);
	int value = date_weeks[index].week - last_week;
	if (value <= 0)
	{
		return SUCC;
	}
	else if (value == 1)
	{
		week_count = week_count + 1;
		uint32_t result = update(userid, week_count, date_weeks[index].week);
		if (result == USER_ID_NOFIND_ERR)
		{
			result = insert(userid, week_count, date_weeks[index].week);
		}
	}
	else
	{
		week_count = 1;
		uint32_t result = update(userid, week_count, date_weeks[index].week);
		if (result == USER_ID_NOFIND_ERR)
		{									                                           
			result = insert(userid, week_count, date_weeks[index].week);												        
		} 
	}

	return SUCC;

}

