#include <algorithm>
#include "Cgf_summon.h"
#include "db_error.h"

using namespace std;
//加入
Cgf_summon::Cgf_summon(mysql_interface * db ) 
	:CtableRoute( db,"GF" ,"t_gf_summon_monster","userid")
{ 

}

int Cgf_summon::get_summon_list(userid_t userid, uint32_t usertm, uint32_t* p_count, gf_get_summon_list_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select \
		mon_tm, mon_type,mon_nick, exp, lv, fight_value, status, attr_per \
        from %s where userid=%u and usertm=%u order by mon_tm;",
		this->get_table_name(userid),userid, usertm);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->mon_tm );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->mon_type );
		BIN_CPY_NEXT_FIELD( (*pp_list+i)->mon_nick, sizeof((*pp_list+i)->mon_nick) );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->exp );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->lv );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->fight_value );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->status );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->add_per);
		//BIN_CPY_NEXT_FIELD( (*pp_list+i)->skill_id, sizeof((*pp_list+i)->skill_id) );
	STD_QUERY_WHILE_END();
}

int Cgf_summon::add_summon(userid_t userid, uint32_t usertm, uint32_t mon_tm, uint32_t mon_type, uint32_t status, char* mon_nick)
{
    char nick_mysql[mysql_str_len(NICK_LEN)];
    set_mysql_string(nick_mysql, mon_nick, NICK_LEN);
	GEN_SQLSTR( this->sqlstr, "insert into %s (userid,usertm,mon_tm,mon_type,mon_nick,\
        fight_value,status) values (%u,%u,%u,%u, '%s', %u, %u);" ,
		this->get_table_name(userid), userid, usertm, mon_tm, mon_type, nick_mysql, 100, status);
	return this->exec_insert_sql (this->sqlstr, GF_SUMMON_EXISTED_ERR);	
}

int Cgf_summon::del_summon(userid_t userid,uint32_t usertm, uint32_t mon_tm)
{
    GEN_SQLSTR(this->sqlstr, "delete from %s where userid=%u and usertm=%u and mon_tm=%u",
        this->get_table_name(userid), userid, usertm, mon_tm);

	return this->exec_update_sql(this->sqlstr, GF_SUMMON_NOFIND_ERR);
}

int Cgf_summon::update_summon_nick(userid_t userid , uint32_t usertm, uint32_t mon_tm,char* nick)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql, nick, NICK_LEN);
	GEN_SQLSTR(this->sqlstr,"update %s set mon_nick='%s' \
			where userid=%u and usertm=%u and mon_tm=%u;",
			this->get_table_name(userid), nick_mysql, userid, usertm, mon_tm);
	return this->exec_update_sql( this->sqlstr, GF_SUMMON_NOFIND_ERR);

}

int Cgf_summon::get_int_value(userid_t userid, uint32_t usertm, uint32_t mon_tm, const char* column_type, uint32_t* db_num)
{
    GEN_SQLSTR(this->sqlstr, "select %s from %s where userid=%u and usertm=%u and mon_tm=%u",
        column_type, this->get_table_name(userid), userid, usertm, mon_tm);
    STD_QUERY_ONE_BEGIN(this-> sqlstr, GF_SUMMON_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*db_num );
    STD_QUERY_ONE_END();
}

int Cgf_summon::get_summon_cnt(userid_t userid, uint32_t usertm, const char* str_where, uint32_t* db_num)
{
    GEN_SQLSTR(this->sqlstr, "select count(*) from %s where userid=%u and usertm=%u and mon_type in (%s)",
        this->get_table_name(userid), userid, usertm, str_where);
    STD_QUERY_ONE_BEGIN(this-> sqlstr, GF_SUMMON_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*db_num );
    STD_QUERY_ONE_END();
}


int Cgf_summon::get_role_summon_cnt(userid_t userid, uint32_t usertm, gf_get_player_community_info_out* p_out)
{
    GEN_SQLSTR(this->sqlstr, "select count(*) from %s where userid=%u and usertm=%u",
        this->get_table_name(userid), userid, usertm);
    STD_QUERY_ONE_BEGIN(this-> sqlstr, GF_SUMMON_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(p_out->summon_cnt );
    STD_QUERY_ONE_END();
}

int Cgf_summon::set_int_value(userid_t userid, uint32_t usertm, uint32_t mon_tm, const char* column_type, uint32_t value)
{
	GEN_SQLSTR( this->sqlstr, "update %s set %s=%u \
        where userid=%u and usertm=%u and mon_tm=%u" ,
		this->get_table_name(userid), column_type, value, userid, usertm, mon_tm);
	return this->exec_update_sql(this->sqlstr, GF_SUMMON_NOFIND_ERR);	
}

int Cgf_summon::increase_int_value(userid_t userid, uint32_t usertm, uint32_t mon_tm,const char* column_type, uint32_t value, uint32_t max_val, uint32_t* p_left)
{
	uint32_t db_num=0;
	uint32_t ret = this->get_int_value(userid, usertm, mon_tm, column_type, &db_num);
	if (ret != SUCC	)
		return ret;
    
    uint32_t total_val = db_num + value;
    if ( total_val > max_val) {
        total_val = max_val;
    }

	ret=this->set_int_value(userid, usertm, mon_tm, column_type, total_val);
	if (ret!=SUCC)
	{
		return ret;
	}
	
	*p_left = total_val;
	return ret;	
}

int Cgf_summon::reduce_int_value(userid_t userid, uint32_t usertm, uint32_t mon_tm,uint32_t err_id,const char* column_type, uint32_t value, uint32_t* p_left)
{
	uint32_t db_num=0;
	uint32_t ret = this->get_int_value(userid, userid, mon_tm, column_type, &db_num);
	if (ret != SUCC	)
	{
		return ret;
	}
	if (db_num < value)
	{
        DEBUG_LOG("ERROR reduce value:%u db_num:%u", value, db_num);
		return err_id;
	}
	else
	{
		if (value)
		{
			ret=this->set_int_value(userid, userid, mon_tm,column_type, (db_num-value));
			if (ret!=SUCC)
			{
				return ret;
			}
		}
	}
	if (p_left)
		*p_left = db_num-value;
	return ret;
}

int Cgf_summon::get_fight_summon(userid_t userid, uint32_t usertm, uint32_t* mon_tm)
{
    GEN_SQLSTR(this->sqlstr, "select mon_tm from %s where userid=%u and usertm=%u and (status =1 or status = 2)",
        this->get_table_name(userid), userid, usertm);
    STD_QUERY_ONE_BEGIN(this-> sqlstr, GF_SUMMON_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*mon_tm);
    STD_QUERY_ONE_END();
}

int Cgf_summon::set_summon_status(userid_t userid, uint32_t usertm, uint32_t mon_tm, uint32_t status)
{
    GEN_SQLSTR(this->sqlstr, "update %s set status=%u where userid=%u and usertm=%u and mon_tm=%u",
        this->get_table_name(userid), status, userid, usertm, mon_tm);

	return this->exec_update_sql(this->sqlstr, GF_SUMMON_NOFIND_ERR);	
}

int Cgf_summon::callback_fight_summon(userid_t userid, uint32_t usertm)
{
    GEN_SQLSTR(this->sqlstr, "update %s set status=0 where userid=%u and usertm=%u and (status = 1 or status = 2)",
        this->get_table_name(userid), userid, usertm);

	return this->exec_update_list_sql(this->sqlstr, SUCC);
}

int Cgf_summon::set_summon_property(userid_t userid,uint32_t usertm,gf_set_summon_property_in* p_in)
{
    GEN_SQLSTR(this->sqlstr, "update %s set lv=%u, exp=%u, fight_value=%u \
        where userid=%u and usertm=%u and mon_tm=%u",
        this->get_table_name(userid), p_in->lv, p_in->exp, p_in->fight_val,
        userid, usertm, p_in->mon_tm);

	return this->exec_update_sql(this->sqlstr, GF_SUMMON_NOFIND_ERR);
}

int Cgf_summon::clear_role_summon(userid_t userid,uint32_t usertm)
{
    GEN_SQLSTR(this->sqlstr, "delete from %s where userid=%u and usertm=%u",
        this->get_table_name(userid), userid, usertm);

	return this->exec_update_sql(this->sqlstr, GF_SUMMON_NOFIND_ERR);
}
int Cgf_summon::set_summon_exp_lv(userid_t userid,uint32_t usertm, uint32_t mon_tm,uint32_t add_exp, uint32_t lv)
{
    GEN_SQLSTR(this->sqlstr, "update %s set lv=%u, exp=exp+%u \
        where userid=%u and usertm=%u and mon_tm=%u",
        this->get_table_name(userid), lv, add_exp, userid, usertm, mon_tm);

	return this->exec_update_sql(this->sqlstr, GF_SUMMON_NOFIND_ERR);
}

int Cgf_summon::fresh_summon_attr_add(userid_t userid, uint32_t usertm, uint32_t mon_tm,
				uint32_t mon_type, int attr_per) 
{
	GEN_SQLSTR(this->sqlstr, "update %s set attr_per=%d, mon_type=%u, lv=1, exp=0 \
			where userid=%u and usertm=%u and mon_tm=%u",
			this->get_table_name(userid), attr_per, mon_type, userid, usertm, mon_tm);
	return this->exec_update_sql(this->sqlstr, GF_SUMMON_NOFIND_ERR);
}

int Cgf_summon::fresh_summon_attr_add_ex(userid_t userid, uint32_t usertm, uint32_t mon_tm,
				uint32_t mon_lv, uint32_t mon_exp, int attr_per) 
{
	GEN_SQLSTR(this->sqlstr, "update %s set attr_per=%d, lv=%u, exp=%u \
			where userid=%u and usertm=%u and mon_tm=%u",
			this->get_table_name(userid), attr_per, mon_lv, mon_exp, userid, usertm, mon_tm);
	return this->exec_update_sql(this->sqlstr, GF_SUMMON_NOFIND_ERR);
}

int Cgf_summon::reset_off_summon(userid_t userid, uint32_t usertm) 
{
	GEN_SQLSTR(this->sqlstr, "update %s set status=3 where userid=%u and usertm=%u and status=4",
			this->get_table_name(userid), userid, usertm);
	return this->exec_update_sql(this->sqlstr, GF_SUMMON_NOFIND_ERR);
}

int Cgf_summon::get_summon_nick_list(userid_t userid, uint32_t usertm, uint32_t* p_count, gf_get_summon_nick_list_out_item** pp_list)
{
	if (usertm) {
		GEN_SQLSTR(this->sqlstr,"select mon_nick from %s where userid=%u and usertm=%u order by mon_tm desc limit 1;",
			this->get_table_name(userid),userid, usertm);
	} else {
		GEN_SQLSTR(this->sqlstr,"select mon_nick from %s where userid=%u  order by mon_tm desc limit 1;",
			this->get_table_name(userid),userid);
	}
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		BIN_CPY_NEXT_FIELD( (*pp_list+i)->nick, sizeof((*pp_list+i)->nick) );
	STD_QUERY_WHILE_END();
}


int Cgf_summon::get_last_summon_nick(userid_t userid, uint32_t usertm, char* nick) 
{
	gf_get_summon_nick_list_out_item* p_item = 0;
	uint32_t count = 0;
	int ret = get_summon_nick_list(userid, usertm, &count, &p_item);
	if (ret != SUCC) {
		return ret;
	}
	if (count) {
		strncpy(nick, p_item[0].nick, sizeof(nick));
	}
	if (p_item) {
		free (p_item);
		p_item = 0;
	}
	return SUCC;
}

int Cgf_summon::clear_role_info(userid_t userid, uint32_t role_time)
{
	sprintf( this->sqlstr, "delete from %s where userid=%u and role_regtime=%u;" ,
		this->get_table_name(userid), userid, role_time );
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}

int Cgf_summon::get_offline_summon(userid_t userid, uint32_t usertm, uint32_t * mon_tm)
{
	GEN_SQLSTR(this->sqlstr,"select mon_tm from %s where userid=%u and usertm=%u and status=4 limit 1;",
			this->get_table_name(userid),userid, usertm);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, GF_SUMMON_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*mon_tm);
	STD_QUERY_ONE_END();
}

