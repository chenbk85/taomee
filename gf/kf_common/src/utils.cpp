/*
 * =====================================================================================
 *
 *       Filename:  utils.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/01/2012 10:50:24 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
#include "utils.hpp"

bool is_same_day(time_t tm_1, time_t tm_2)
{
	struct tm  t1;
	struct tm  t2;

	localtime_r(&tm_1, &t1);
	localtime_r(&tm_2, &t2);
	return t1.tm_year == t2.tm_year 
		&& t1.tm_mon == t2.tm_mon
		&& t1.tm_mday == t2.tm_mday ; 
}

int day_interval(time_t tm_1, time_t tm_2)
{
	struct tm  t1;
	struct tm  t2;

	if (tm_1 < tm_2) {
		time_t tmp = tm_1;
		tm_1 = tm_2;
		tm_2 = tmp;
	}


	localtime_r(&tm_1, &t1);
	localtime_r(&tm_2, &t2);

	t1.tm_hour = 1;
	t1.tm_mon = 0;
	t1.tm_sec = 0;

	t2.tm_hour = 1;
	t2.tm_mon = 0;
	t2.tm_sec = 0;

	return (mktime(&t1) - mktime(&t2)) / (24 * 3600);
}


uint32_t get_today_last_sec()
{
	time_t now = time(NULL);
	struct tm t1;
	localtime_r(&now, &t1);
	t1.tm_hour = 23;
	t1.tm_min = 59;
	t1.tm_sec = 59;
	return (mktime(&t1));

}


