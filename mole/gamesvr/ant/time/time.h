#ifndef ANT_TIME_H_
#define ANT_TIME_H_

#include <assert.h>
#include <time.h>

#include <glib.h>

/**
 * mk_integral_tm_hr - cal an integral hour time using @tm_cur
 *
 * return: integral hour time
 */
static inline time_t
mk_integral_tm_hr(struct tm tm_cur)
{
	tm_cur.tm_min  = 0;
	tm_cur.tm_sec  = 0;

	return mktime(&tm_cur);
}

/**
 * mk_integral_tm_day - cal an integral day time at @mday using @tm_cur offset by @month
 *
 * return: integral day time at @mday
 */
static inline time_t
mk_integral_tm_day(struct tm tm_cur, int mday, int month)
{
	assert( (mday > 0) && (mday < 32) );

	tm_cur.tm_hour = 0;
	tm_cur.tm_min  = 0;
	tm_cur.tm_sec  = 0;

	GDate* gdate1 = g_date_new_dmy(tm_cur.tm_mday, tm_cur.tm_mon + 1, tm_cur.tm_year);
	GDate* gdate2 = g_date_new_dmy(mday, tm_cur.tm_mon + 1, tm_cur.tm_year);

	if (month > 0) {
		g_date_add_months(gdate2, month);
	} else if (month < 0) {
		g_date_subtract_months(gdate2, -month);
	}

	int days = g_date_days_between(gdate1, gdate2);

	g_date_free(gdate1);
	g_date_free(gdate2);
	return mktime(&tm_cur) + days * 86400;
}

#endif // ANT_TIME_H_
