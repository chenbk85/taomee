
#include "Cday.h"

Cday::Cday(mysql_interface* db):
	CtableRoute100x10(db, "POP", "t_day", "userid","itemid")
{
}

int Cday::insert(uint32_t userid, uint32_t itemid,uint32_t count)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u,%u,%u,%u,%u)",
		this->get_table_name(userid),userid,itemid,count,count,today());
	return this->exec_insert_sql(this->sqlstr, ITEM_IS_EXISTED_ERR);
}

int Cday::get(uint32_t userid, uint32_t itemid,uint32_t& total,uint32_t& count,uint32_t& day)
{
	GEN_SQLSTR(this->sqlstr, "select total,daycnt,optday from %s where userid=%u and itemid=%u",
        this->get_table_name(userid),userid,itemid);

	STD_QUERY_ONE_BEGIN(this->sqlstr,ITEM_NOFIND_ERR);
	INT_CPY_NEXT_FIELD(total);
	INT_CPY_NEXT_FIELD(count);
	INT_CPY_NEXT_FIELD(day);
	uint32_t tday = today();
	if (day != tday) {
		count = 0;
		if (itemid == 5 && tday != day + 86400) 
			total = 0;
	}
	STD_QUERY_ONE_END();
}

int Cday::set(uint32_t userid, uint32_t itemid,uint32_t total,uint32_t count)
{
	int day = today();
	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u,%u,%u,%u,%u) ON DUPLICATE KEY UPDATE total=%u, daycnt=%u, optday=%u",
		this->get_table_name(userid), userid, itemid, total, count, day, total,count, day);

	return this->exec_update_sql(this->sqlstr, 0);
}

int Cday::change_count(userid_t userid, uint32_t itemid, int change_count, uint32_t add_day)
{
	if (!change_count) return 0;

	uint32_t total_cnt = 0;
	uint32_t count = 0;
	uint32_t day = today();
	
	int ret = this->get(userid, itemid, total_cnt, count, day);
	if (change_count < 0) {
		if (ret == ITEM_NOFIND_ERR || total_cnt < uint32_t (-change_count))
			return ITEM_NOENOUGH_ERR;
		total_cnt += change_count;
	} else {
		total_cnt += change_count;
		count += add_day;
	}

	ret = set(userid, itemid, total_cnt, count);

	return ret;
}

int Cday::get_list(userid_t userid, std::vector<item_day_limit_t> &limits)
{
	int day=0;
	int date = today();
	GEN_SQLSTR(this->sqlstr, "select itemid,total,daycnt,optday "
		"from %s where userid=%u", this->get_table_name(userid),userid);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, limits);
		INT_CPY_NEXT_FIELD(item.itemid);
		INT_CPY_NEXT_FIELD(item.total);
		INT_CPY_NEXT_FIELD(item.daycnt);
		INT_CPY_NEXT_FIELD(day);
		if(day != date) {
			item.daycnt = 0;
			if (item.itemid == 5 && date != day + 86400) 
				item.total = 0;
		}
	STD_QUERY_WHILE_END_NEW();
}

int Cday::del(userid_t userid, uint32_t itemid)
{
	GEN_SQLSTR(this->sqlstr, "delete from %s where userid=%u and itemid=%u",
		this->get_table_name(userid), userid, itemid);

	return this->exec_update_sql(this->sqlstr, 0);
}

int Cday::back_day(userid_t userid, uint32_t itemid)
{
	GEN_SQLSTR(this->sqlstr, "update %s set optday=optday-86400 where userid=%u and itemid=%u",
		this->get_table_name(userid), userid, itemid);

	return this->exec_update_sql(this->sqlstr, 0);
}
