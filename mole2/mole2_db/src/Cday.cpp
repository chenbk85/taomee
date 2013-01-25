#include "Cday.h"

#define STR_UID		"userid"
#define STR_SSID	"ssid"
#define STR_TOTAL	"total"
#define STR_COUNT	"count"
#define STR_TODAY	"today"
#define IS_WEEK_CLEAR_TYPE(id_) ((id_)>=65000 && (id_)<=65999)
#define IS_2WEEK_CLEAR_TYPE(id_) ((id_)>=66000 && (id_)<=66999)
#define IS_MONTH_CLEAR_TYPE(id_) ((id_)>=67000 && (id_)<=67999)
#define IS_NORMAL_TYPE(id_) ((id_)<65000 || (id_)>68000)
uint32_t Cday::get_next_sunday(time_t t){
    struct tm tm;
    localtime_r(&t, &tm) ;
    tm.tm_hour=0;
    tm.tm_min=0;
    tm.tm_sec=0;
    uint32_t opt_date=tm.tm_wday>0 ?tm.tm_wday:7;
    tm.tm_mday=tm.tm_mday-opt_date+7+7;
	return mktime(&tm);
}

uint32_t Cday::get_next_next_friday(time_t t){
    struct tm tm;
    localtime_r(&t, &tm) ;
    tm.tm_hour=0;
    tm.tm_min=0;
    tm.tm_sec=0;
    uint32_t opt_date=tm.tm_wday>0 ?tm.tm_wday:7;
    tm.tm_mday=tm.tm_mday+7-opt_date+5+7;
	return mktime(&tm);
}

uint32_t Cday::get_next_month(time_t t){
    struct tm tm;
    localtime_r(&t, &tm) ;
    tm.tm_hour=0;
    tm.tm_min=0;
    tm.tm_sec=0;
    tm.tm_mday=tm.tm_mday+30;
	return mktime(&tm);
}

Cday::Cday(mysql_interface* db):
	CtableRoute(db, "MOLE2_USER", "t_day", "userid","ssid")
{
}

int Cday::insert(uint32_t userid, uint32_t ssid,uint32_t count)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u,%u,%u,%u,%u)",
		this->get_table_name(userid),userid,ssid,count,count,today());
	return this->exec_insert_sql(this->sqlstr, MOLE2_KEY_EXIST_ERR);
}

int Cday::get(uint32_t userid, uint32_t ssid,uint32_t *total,uint32_t* count,uint32_t *day)
{
	GEN_SQLSTR(this->sqlstr, "select total,count,today from %s where userid=%u and ssid=%u",
        this->get_table_name(userid),userid,ssid);

	STD_QUERY_ONE_BEGIN(this->sqlstr,MOLE2_KEY_NOT_EXIST_ERR);
	INT_CPY_NEXT_FIELD(*total);
	INT_CPY_NEXT_FIELD(*count);
	INT_CPY_NEXT_FIELD(*day);
	STD_QUERY_ONE_END();
}

int Cday::set(uint32_t userid, uint32_t ssid,uint32_t total,uint32_t count)
{
	GEN_SQLSTR(this->sqlstr, "update %s set total=%u,count=%u,today=%u where userid=%u and ssid=%u",
		this->get_table_name(userid),total,count,today(),userid,ssid);

	return this->exec_update_sql(this->sqlstr, MOLE2_KEY_EXIST_ERR);
}

int Cday::set_field_value(uint32_t userid, su_mole2_set_field_value_in* p_in){
	char mysql_value[mysql_str_len(sizeof(p_in->value))];
	set_mysql_string(mysql_value, p_in->value, sizeof(p_in->value));

	GEN_SQLSTR(this->sqlstr, "update %s set %s = '%s' where %s = %u and %s = %u", 
				this->get_table_name(userid), p_in->field,	mysql_value, 
				STR_UID,	userid,
				STR_SSID, p_in->opt_id);
	return this->exec_update_sql(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
}
int Cday::get_list(userid_t userid, mole2_user_get_day_list_out_item** pp_item, uint32_t* p_count)
{
	uint32_t day=0;
	uint32_t date = today();
	GEN_SQLSTR(this->sqlstr, "select ssid,total,count,today from %s where %s=%u",
		this->get_table_name(userid),STR_UID,userid);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_item, p_count);
		INT_CPY_NEXT_FIELD((*pp_item+i)->ssid);
		INT_CPY_NEXT_FIELD((*pp_item+i)->total);
		INT_CPY_NEXT_FIELD((*pp_item+i)->count);
		INT_CPY_NEXT_FIELD(day);
		if((IS_WEEK_CLEAR_TYPE((*pp_item+i)->ssid) && date >= get_next_sunday(day)) ||
			   	(IS_2WEEK_CLEAR_TYPE((*pp_item+i)->ssid) && date >= get_next_next_friday(day)) ||  
			   	(IS_MONTH_CLEAR_TYPE((*pp_item+i)->ssid) && date >= get_next_month(day)) ||  
				(IS_NORMAL_TYPE((*pp_item+i)->ssid) && day != date )){
			(*pp_item+i)->count=0;
		}
	STD_QUERY_WHILE_END();
}

int Cday::get_limits(userid_t userid, std::vector<stru_day_limit> &limits,uint32_t min,uint32_t max)
{
	uint32_t day=0;
	uint32_t date = today();
	GEN_SQLSTR(this->sqlstr, "select "
		"ssid,total,count,today "
		"from %s where userid=%u and ssid >= %u and ssid <= %u",
		this->get_table_name(userid),userid,min,max);
	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, limits);
		INT_CPY_NEXT_FIELD(item.ssid);
		INT_CPY_NEXT_FIELD(item.total);
		INT_CPY_NEXT_FIELD(item.count);
		INT_CPY_NEXT_FIELD(day);
		if((IS_WEEK_CLEAR_TYPE(item.ssid) && date >= get_next_sunday(day)) ||
			   	(IS_2WEEK_CLEAR_TYPE(item.ssid) && date >= get_next_next_friday(day)) ||  
			   	(IS_MONTH_CLEAR_TYPE(item.ssid) && date >= get_next_month(day)) ||  
				(IS_NORMAL_TYPE(item.ssid) && day != date )){
			item.count = 0;
		}
	STD_QUERY_WHILE_END_NEW();
}


