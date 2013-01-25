
#include "Cmax.h"

#define STR_UID		"userid"
#define STR_TYPE	"type"
#define STR_ID		"id"
#define STR_COUNT	"count"
#define STR_TODAY	"today"

Cmax::Cmax(mysql_interface* db):
	CtableRoute(db, "MOLE2_USER", "t_max", "userid")
{
}

int Cmax::del(uint32_t userid, uint32_t type,uint32_t id)
{
	GEN_SQLSTR(this->sqlstr, "delete from %s where %s=%u and %s=%u and %s=%u",
		this->get_table_name(userid), 
		STR_UID,userid,
		STR_TYPE,type,
		STR_ID,id);
	return this->exec_update_sql(this->sqlstr, MOLE2_KEY_NOT_EXIST_ERR);
}

int Cmax::insert(uint32_t userid, uint32_t type,uint32_t id,int count)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u,%u,%u,%d,%u)",
		this->get_table_name(userid),userid,type,id,count,today());
	return this->exec_insert_sql(this->sqlstr, MOLE2_KEY_EXIST_ERR);
}

int Cmax::set(uint32_t userid, uint32_t type,uint32_t id,uint32_t count,uint32_t day)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u,%s=%u where %s=%u and %s=%u and %s=%u",
        this->get_table_name(userid),
		STR_COUNT,count,
		STR_TODAY,day,
		STR_UID,userid,
		STR_TYPE,type,
		STR_ID,id);
	return this->exec_update_sql(this->sqlstr, MOLE2_KEY_NOT_EXIST_ERR);
}

int Cmax::get(uint32_t userid, uint32_t type,uint32_t id,uint32_t *count,uint32_t *day)
{
	GEN_SQLSTR(this->sqlstr, "select %s,%s from %s where %s=%u and %s=%u and %s=%u",
		STR_COUNT,
		STR_TODAY,
        this->get_table_name(userid),
		STR_UID,userid,
		STR_TYPE,type,
		STR_ID,id);
	STD_QUERY_ONE_BEGIN(this->sqlstr,MOLE2_KEY_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(*count);
		INT_CPY_NEXT_FIELD(*day);
	STD_QUERY_ONE_END();
}

int Cmax::add(uint32_t userid, uint32_t type,uint32_t id,int count,uint32_t *cur_cnt)
{
	int ret;
	uint32_t day;
	uint32_t cnt;

	ret = this->get(userid,type,id,&cnt,&day);
	if(ret != SUCC) {
		if(count < 0) {
			return VALUE_NOT_ENOUGH_ERR;
		}
		return this->insert(userid,type,id,count);
	}

	uint32_t date = today();
	switch((type >> 28) & 0x7) {
		default:// day
			if(day == date) {
				cnt += count;
			} else {
				cnt = count;
			}
			break;
		case 2:
			if(day + 7 * 24 * 3600 < date) {
				date = day;
				cnt += count;
			} else {
				cnt = count;
			}
			break;
		case 3: // month
			if(day + 30 * 24 * 3600 < date) {
				date = day;
				cnt += count;
			} else {
				cnt = count;
			}
			break;
		case 4: // year
			if(day + 365 * 24 * 3600 < date) {
				date = day;
				cnt += count;
			} else {
				cnt = count;
			}
			break;
		case 5://forever
			date = day;
			cnt += count;
			break;
		case 6://once per day
			if(date != day) {
				cnt += count;
			} else {
				return MOLE2_KEY_EXIST_ERR;
			}
			break;
		case 7://once
			return MOLE2_KEY_EXIST_ERR;
	}

	if((int)cnt < 0) {
		return VALUE_NOT_ENOUGH_ERR;
	}

	if(cur_cnt) *cur_cnt = cnt;
	return this->set(userid,type,id, cnt,date);
}

