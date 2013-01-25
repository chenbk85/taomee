/*
 * =====================================================================================
 * 
 *       Filename:  common.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月02日 18时33分18秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  COMMON_func_INC
#define  COMMON_func_INC
#include <stdint.h>
#include <stdio.h>
#include <time.h> 
#include "benchapi.h"

#define	DAY_TO_SECOND	(24 * 60 * 60)

enum repeat_type_t {
	REPEAT_DAY = 1,
	REPEAT_WEEK = 2,
	REPEAT_MONTH = 3, 
};

#define BAN_14DAYS	0x00800000
#define BAN_7DAYS	0x01000000
#define BAN_24HOURS	0x02000000
#define BAN_FOREVER	0x04000000

class Ccommon 
{
public:
	static inline uint32_t add_limit32(uint32_t base, int32_t addition, 
			uint32_t min, uint32_t max, int32_t* p_change_val = NULL)
	{
		if(min > max)	
		{
			if (p_change_val)
				*p_change_val = 0;
			return base;
		}

		uint32_t ret = base + addition;

		if(addition < 0 && ret > base)
			ret = min;
		else if(addition > 0 && ret < base)
			ret = max;

		ret = ret < min ? min : ret;
		ret = ret > max ? max : ret;

		if (p_change_val)
			*p_change_val = ret - base;

		return ret;
	}

	static inline int check_same_time(time_t optdate, time_t date, repeat_type_t type)
	{
		struct tm optinfo, tminfo;
		
		localtime_r(&optdate, &optinfo);
		localtime_r(&date, &tminfo);
		switch(type)
		{
			case REPEAT_DAY:
				return optinfo.tm_yday == tminfo.tm_yday && optinfo.tm_year == tminfo.tm_year;
			case REPEAT_WEEK:
				return (optdate >= date &&((optinfo.tm_year == tminfo.tm_year && optinfo.tm_yday - tminfo.tm_yday < 7 
					&& optinfo.tm_wday >= tminfo.tm_wday) || (optinfo.tm_year - tminfo.tm_year == 1
					&& optinfo.tm_wday > tminfo.tm_wday && 31 - optinfo.tm_mday + tminfo.tm_mday - 1 < 7)))
					|| (optdate < date &&((optinfo.tm_year == tminfo.tm_year && tminfo.tm_yday - optinfo.tm_yday < 7 
					&& optinfo.tm_wday < tminfo.tm_wday) || (tminfo.tm_year - optinfo.tm_year == 1
					&& optinfo.tm_wday < tminfo.tm_wday && 31 - tminfo.tm_mday + optinfo.tm_mday - 1 < 7)));
			case REPEAT_MONTH:
				return optinfo.tm_mon == tminfo.tm_mon && optinfo.tm_year == tminfo.tm_year; 
		}
		return 0;
	}
};

#define ADD_LIMIT(o_, a_, limit_, max_, err_) \
{	\
	uint32_t tmp = (o_) + (a_);	\
	if ((a_) > 0){		\
		if ((limit_) && (tmp < (o_)))	\
			return (err_);	\
		else if(tmp < (o_))	\
			(o_) = (max_);	\
		else (o_) = tmp;	\
	} else {	\
		if ((limit_) && tmp > (o_))		\
			return (err_);	\
		else if (tmp < (o_))	\
			(o_) = 0;	\
		else (o_) = tmp;	\
	}	\
}

#define LIMIT_ADD(o_, a_, min_, max_, min_err_, max_err_) \
{	\
	(o_) = (o_) + (a_);	\
	if ((o_) < (min_)) {	\
		DEBUG_LOG("count exception, count = %d", o_);	\
		return min_err_;	\
	} else if ((o_) > (max_)) {	\
		DEBUG_LOG("count exception, count = %d", o_);	\
		return max_err_;	\
	}	\
}

static inline void log_buf(char* str, uint8_t* buf, int len)
{
	char out[4096];
	int i, j = 0;

	for (i = 0; i < len; i ++) {
		j += sprintf (out + j, "%02x ", buf[i]);
	}

	out[j] = '\0';

	DEBUG_LOG("%s:%s", str, out);
}

static inline int today()
{
	time_t seconds = time(NULL);
	return seconds - (seconds + 8 * 3600) % (24 * 3600);
}

static inline int time_day_diff(uint32_t dst,uint32_t src)
{
	return ((dst - (dst + 8 * 3600) % (24 * 3600)) - (src - (src + 8 * 3600) % (24 * 3600))) / (24 * 3600);
}

#endif   /* ----- #ifndef COMMON_INC  ----- */

