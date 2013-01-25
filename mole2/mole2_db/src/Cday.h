#ifndef  CDAY_INC
#define  CDAY_INC

#include "CtableRoute.h"
#include "proto.h"
#include "db_error.h"
#include "Ccommon.h"

class Cday:public CtableRoute
{
public:
	Cday(mysql_interface * db);
	int insert(uint32_t userid, uint32_t ssid,uint32_t count);
	int get(uint32_t userid, uint32_t ssid,uint32_t *total,uint32_t* count,uint32_t *day);
	int set(uint32_t userid, uint32_t ssid,uint32_t total,uint32_t count);
	int get_list(userid_t userid, mole2_user_get_day_list_out_item** p_item, uint32_t* p_count);
	int get_limits(userid_t userid, std::vector<stru_day_limit> &limits,uint32_t min,uint32_t max);
	int set_field_value(uint32_t userid, su_mole2_set_field_value_in* p_in);
private:
	uint32_t get_next_sunday(time_t t);
	uint32_t get_next_month(time_t t);
	uint32_t get_next_next_friday(time_t t);
};

#endif

