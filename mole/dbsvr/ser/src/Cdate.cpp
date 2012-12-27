/*
 * =====================================================================================
 *
 *       Filename:  Cdate.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/10/2009 10:42:26 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include "Cdate.h"

Date::Date()
{
	now = time(NULL);
}

Date::Date(time_t time)
{
	now = time;
}

uint32_t Date:: get_yymmdd()
{
	struct tm tm_tmp;
	localtime_r(&now, &tm_tmp) ;
	return (tm_tmp.tm_year+1900)*10000+(tm_tmp.tm_mon+1)*100+tm_tmp.tm_mday;
}

uint32_t Date::get_yymm()
{
	struct tm tm_tmp;
	localtime_r(&now, &tm_tmp) ;
	return (tm_tmp.tm_year+1900)*100+tm_tmp.tm_mon+1;
}
