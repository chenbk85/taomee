#include <algorithm>
#include "Cgf_team.h"
#include "db_error.h"

using namespace std;
//加入
Cgf_team::Cgf_team(mysql_interface * db ) 
	:Ctable( db, "GF_OTHER", "t_gf_team")
{ 

}

int Cgf_team::create_team(userid_t userid, uint32_t usertm, const char* team_name, const char* cp_name
	, uint32_t* p_team_id)
{
	char bind_mysql[mysql_str_len(ITEM_BIND_LEN)];

	set_mysql_string(bind_mysql, team_name, ITEM_BIND_LEN);

	char cp_mysql[mysql_str_len(ITEM_BIND_LEN)];
	set_mysql_string(cp_mysql, cp_name, ITEM_BIND_LEN);
	
	GEN_SQLSTR(this->sqlstr,"insert into %s (team_id, captain_id,captain_tm,name,captain_name,\
		tm, count) values (null, %u, %u, '%s','%s', unix_timestamp(now()), 1);",
		this->get_table_name(),userid, usertm, bind_mysql, cp_mysql);
	
	return exec_insert_sql_get_auto_increment_id(this->sqlstr,  GF_ATTIREID_EXISTED_ERR, p_team_id);
}

int Cgf_team::del_team(userid_t userid, uint32_t usertm, uint32_t teamid)
{
	GEN_SQLSTR(this->sqlstr,"delete from %s where team_id=%u and captain_id=%u and captain_tm=%u and count<2;",
		this->get_table_name(), teamid, userid, usertm);
	return this->exec_update_sql (this->sqlstr, GF_TASKID_NOFIND_ERR);
}

int Cgf_team::increase_score(uint32_t teamid, uint32_t score)
{
	GEN_SQLSTR(sqlstr, "update %s set score=score+%u where team_id=%u ",
							get_table_name(), score, teamid);
	return exec_update_sql(sqlstr, GF_ATTIREID_NOFIND_ERR);
}

int Cgf_team::reduce_score(uint32_t teamid, uint32_t score)
{
	GEN_SQLSTR(sqlstr, "update %s set score=score-%u where team_id=%u and score>1",
							get_table_name(), score, teamid);
	return exec_update_sql(sqlstr, GF_ATTIREID_NOFIND_ERR);
}

int Cgf_team::increase_active_score(uint32_t teamid, uint32_t flag, uint32_t score)
{
	GEN_SQLSTR(sqlstr, "update %s set active_score=active_score+%u where team_id=%u and active_flag=%u",
							get_table_name(), score, teamid, flag);
	return exec_update_sql(sqlstr, GF_ATTIREID_NOFIND_ERR);
}

int Cgf_team::reduce_active_score(uint32_t teamid, uint32_t flag, uint32_t score)
{
	GEN_SQLSTR(sqlstr, "update %s set active_score=active_score-%u \
        where team_id=%u and active_flag=%u and active_score>0",
							get_table_name(), score, teamid, flag);
	return exec_update_sql(sqlstr, GF_ATTIREID_NOFIND_ERR);
}

int Cgf_team::increase_team_member_count(uint32_t teamid)
{
	GEN_SQLSTR(sqlstr, "update %s set count=count+1 where team_id=%u and count<25",
						get_table_name(), teamid);
	return exec_update_sql(sqlstr, GF_TEAM_MEMBER_FULL_ERR);

}

int Cgf_team::reduce_team_member_count(uint32_t teamid)
{
	GEN_SQLSTR(sqlstr, "update %s set count=count-1 where team_id=%u and count>0",
						get_table_name(), teamid);
	return exec_update_sql(sqlstr, GF_ATTIREID_NOFIND_ERR);

}

int Cgf_team::get_team_info(uint32_t teamid, gf_get_team_info_out_header* p_out)
{
	GEN_SQLSTR(sqlstr, "select captain_id,captain_tm, captain_name, name, mcast,coin,active_flag,active_score,score_index, score, team_exp, last_tax_time from %s where team_id=%u ",
						get_table_name(), teamid);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->captain_uid);
		INT_CPY_NEXT_FIELD(p_out->captain_tm);
		BIN_CPY_NEXT_FIELD(p_out->captain_nick, 16);
		BIN_CPY_NEXT_FIELD(p_out->team_name, 16);
		BIN_CPY_NEXT_FIELD(p_out->team_mcast, 240);
		INT_CPY_NEXT_FIELD(p_out->team_coin);
		INT_CPY_NEXT_FIELD(p_out->active_flag);
		INT_CPY_NEXT_FIELD(p_out->active_score);
		INT_CPY_NEXT_FIELD(p_out->score_index);
		INT_CPY_NEXT_FIELD(p_out->score);
		INT_CPY_NEXT_FIELD(p_out->team_exp);
		INT_CPY_NEXT_FIELD(p_out->last_tax_time);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	return SUCC;
}

int Cgf_team::get_team_top100(uint32_t* p_count, gf_get_team_top100_out_item** pp_list)
{
	GEN_SQLSTR(sqlstr, "select team_id,name, captain_id,captain_tm,captain_name,count,score from %s \
		order by score desc limit 100;",
							get_table_name());
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->teamid );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->captain_uid );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->captain_role_tm );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->score );
	STD_QUERY_WHILE_END();
}



int Cgf_team::get_team_top10(uint32_t* p_count, gf_get_team_top10_out_item** pp_list)
{
	GEN_SQLSTR(sqlstr, "select a.team_id,name, captain_id,captain_tm,captain_name,count,score, team_exp from \
			%s a inner join (select team_id from %s order by team_exp desc,team_id limit 100) b on a.team_id=b.team_id;",
			get_table_name(), get_table_name());

//	GEN_SQLSTR(sqlstr, "select team_id,name, captain_id,captain_tm,captain_name,count,score, team_exp from %s 
//		order by team_exp desc,team_id limit 100;",
//							get_table_name());
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->teamid );
		BIN_CPY_NEXT_FIELD( (*pp_list+i)->team_name, 16 );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->captain_uid );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->captain_role_tm );
		BIN_CPY_NEXT_FIELD( (*pp_list+i)->captain_name, 16 );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->member_cnt );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->score );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->team_exp);
	STD_QUERY_WHILE_END();
}

int Cgf_team::get_team_active_top100(uint32_t flag, uint32_t* p_count, gf_get_team_active_top100_out_item** pp_list)
{
    GEN_SQLSTR(sqlstr, "select team_id,name, captain_id,captain_tm,captain_name, count,active_score, score, team_exp from %s \
		where active_flag=%u and active_score>0 order by active_score desc,team_id limit 100;",
							get_table_name(), flag);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->id );
		BIN_CPY_NEXT_FIELD( (*pp_list+i)->name, 16 );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->captain_id );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->captain_tm );
		BIN_CPY_NEXT_FIELD( (*pp_list+i)->captain_name, 16 );
//		INT_CPY_NEXT_FIELD( (*pp_list+i)->team_lv);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->member_cnt );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->active_score );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->score);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->team_exp);
	STD_QUERY_WHILE_END();
}

int Cgf_team::get_team_int_value(uint32_t teamid, const char* column_type, uint32_t* p_count)
{
    GEN_SQLSTR(sqlstr, "select %s from %s where team_id=%u", 
        column_type, get_table_name(), teamid);

    STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD( *p_count );
    STD_QUERY_ONE_END_WITHOUT_RETURN();

    return SUCC;
}

int Cgf_team::set_team_int_value(uint32_t teamid, const char* column_type, uint32_t value)
{
    GEN_SQLSTR(sqlstr, "update %s set %s=%u where team_id=%u",
						get_table_name(), column_type, value, teamid);
	return exec_update_sql(sqlstr, USER_ID_NOFIND_ERR);
}

int Cgf_team::increase_team_int_value(uint32_t teamid, const char* column_type, uint32_t value, uint32_t* p_left)
{
    uint32_t db_num=0;
	int ret = this->get_team_int_value(teamid, column_type, &db_num);
	if (ret != SUCC	)
		return ret;

    if (db_num + value > 100000000)
        return GF_TEAM_MEMBER_COINS_LIMIT;

	ret=this->set_team_int_value(teamid, column_type,(db_num + value));
	if (ret!=SUCC) {
		return ret;
	}
	
	if (p_left)
		*p_left = db_num + value;
	return ret;	
}

int Cgf_team::reduce_team_int_value(uint32_t teamid, uint32_t errid, const char* column_type, uint32_t value, uint32_t* p_left)
{
    uint32_t db_num=0;
	int ret = this->get_team_int_value(teamid, column_type, &db_num);
	if (ret != SUCC	)
		return ret;
    
    if (db_num < value) {
        return errid;
    } else {
        if (value > 0) {
            ret=this->set_team_int_value(teamid, column_type,(db_num - value));
            if (ret!=SUCC) {
                return ret;
            }
        }
    }

    *p_left = db_num - value;
    return ret;
}

int Cgf_team::change_team_name(gf_change_team_name_in * p_in)
{
	char name_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(name_mysql, p_in->team_name, NICK_LEN);
	GEN_SQLSTR(sqlstr, "update %s set name = '%s' where  team_id = %u",
			this->get_table_name(), name_mysql, p_in->team_id);

	return exec_update_list_sql(this->sqlstr, SUCC);
}

int Cgf_team::change_team_mcast(gf_change_team_mcast_in * p_in)
{
	char mcast_mysql[mysql_str_len(MCAST_LEN)];
	set_mysql_string(mcast_mysql, p_in->mcast_info, MCAST_LEN);
	GEN_SQLSTR(sqlstr, "update %s set mcast = '%s' where  team_id = %u",
			this->get_table_name(), mcast_mysql, p_in->team_id);
	return exec_update_list_sql(this->sqlstr, SUCC);
}

int Cgf_team::search_team_info(uint32_t team_id, uint32_t captain_id, uint32_t * p_count, gf_search_team_info_out_item ** pp_list)
{
	GEN_SQLSTR(this->sqlstr, "select team_id, name, captain_id,captain_tm,captain_name,count,score, team_exp from %s \
			where team_id=%u or captain_id=%u", this->get_table_name(), team_id, captain_id);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->teamid );
		BIN_CPY_NEXT_FIELD( (*pp_list+i)->team_name, 16 );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->captain_uid );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->captain_role_tm );
		BIN_CPY_NEXT_FIELD( (*pp_list+i)->captain_name, 16 );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->member_cnt );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->score );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->team_exp);
	STD_QUERY_WHILE_END();
}

int Cgf_team::get_team_last_tax_info(uint32_t team_id, uint32_t * last_time, uint32_t * team_coins)
{
	GEN_SQLSTR(this->sqlstr, "select coin, last_tax_time from %s where team_id=%u",
			this->get_table_name(), team_id);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*team_coins);
		INT_CPY_NEXT_FIELD(*last_time);
	STD_QUERY_ONE_END_WITHOUT_RETURN();
	return 0;
}

int Cgf_team::reduce_team_tax_coin(uint32_t team_id, uint32_t left_coin, uint32_t last_tax_time)
{
	GEN_SQLSTR(this->sqlstr, "update %s set coin=%u, last_tax_time=%u \
			where team_id=%u", this->get_table_name(), left_coin, last_tax_time,  team_id);

	return exec_update_list_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}


