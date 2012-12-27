#ifndef  CDAY_INC
#define  CDAY_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "db_error.h"

class Cday:public CtableRoute100x10
{
	int insert(uint32_t userid, uint32_t itemid,uint32_t count);
	int get(uint32_t userid, uint32_t itemid,uint32_t& total, uint32_t& count,uint32_t& day);
	int set(uint32_t userid, uint32_t itemid,uint32_t total,uint32_t count);
public:
	Cday(mysql_interface * db);

	int get_list(userid_t userid, std::vector<item_day_limit_t> &limits);
	int change_count(userid_t userid, uint32_t itemid, int change_total, uint32_t add_day = 0);
	int del(userid_t userid, uint32_t itemid);
	int back_day(userid_t userid, uint32_t itemid);

	inline uint32_t today()
	{
	    time_t seconds = time(NULL);
	    return seconds - (seconds + 8 * 3600) % (24 * 3600);
	}
};

#endif

