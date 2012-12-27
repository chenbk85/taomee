/*
 * =====================================================================================
 *
 *       Filename:  Cgf_hunter_top.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/04/2011 01:37:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Saga (), saga@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#include <algorithm>
#include "db_error.h"
#include "Cgf_hunter_top.h"

#define MAX_TOP 1000

Cgf_hunter_top::Cgf_hunter_top(mysql_interface *db)
		:Ctable(db, "GF_OTHER", "t_gf_top_hunter")
{
}

int Cgf_hunter_top::get_over_tower_info(uint32_t userid, uint32_t role_tm, uint32_t* layer, uint32_t* interval, uint32_t* place_tm, uint32_t reward_flg)
{
	if (reward_flg) {
	    GEN_SQLSTR(sqlstr, "select layer, time_interval, place_tm from %s \
			where userid=%u and role_regtime=%u and reward_flg>0",
	        this->get_table_name(), userid, role_tm);
	} else {
	    GEN_SQLSTR(sqlstr, "select layer, time_interval, place_tm from %s \
			where userid=%u and role_regtime=%u and reward_flg=0",
	        this->get_table_name(), userid, role_tm);
	}
    STD_QUERY_ONE_BEGIN(sqlstr, GF_HUNTER_TOP_ERR);
        INT_CPY_NEXT_FIELD(*layer);
        INT_CPY_NEXT_FIELD(*interval);
		INT_CPY_NEXT_FIELD(*place_tm);
    STD_QUERY_ONE_END();
}

int Cgf_hunter_top::get_one_over_tower_info(uint32_t role_type, uint32_t* p_userid, uint32_t* p_role_tm)
{
    GEN_SQLSTR(sqlstr, "select userid, role_regtime from %s \
		where reward_flg=0 and role_type=%u order by layer, time_interval desc, place_tm desc limit 1",
        this->get_table_name(), role_type);
    STD_QUERY_ONE_BEGIN(sqlstr, GF_HUNTER_TOP_ERR);
        INT_CPY_NEXT_FIELD(*p_userid);
        INT_CPY_NEXT_FIELD(*p_role_tm);
    STD_QUERY_ONE_END();
}

int Cgf_hunter_top::get_last_info(uint32_t* p_layer, uint32_t* p_tm, uint32_t* p_place_tm, uint32_t role_type)
{
    GEN_SQLSTR(sqlstr, "select layer, time_interval from %s \
		where reward_flg=0 and role_type=%u order by layer, time_interval desc, place_tm desc limit 1",
        this->get_table_name(), role_type);
    STD_QUERY_ONE_BEGIN(sqlstr, GF_HUNTER_TOP_ERR);
        INT_CPY_NEXT_FIELD(*p_layer);
        INT_CPY_NEXT_FIELD(*p_tm);
		INT_CPY_NEXT_FIELD(*p_place_tm);
    STD_QUERY_ONE_END();
}

int Cgf_hunter_top::set_hunter_top_info(uint32_t userid, uint32_t role_tm, gf_set_hunter_top_info_in* p_in)
{
    uint32_t c_layer = 0, c_interval = 0, place_tm = 0;

   get_over_tower_info(userid, role_tm, &c_layer, &c_interval, &place_tm);
    if (p_in->layer < c_layer) {
        //do nothing ...
        return SUCC;
    } 
	if (p_in->layer == c_layer && p_in->interval >= c_interval) {
		return SUCC;
	}
	
    char nick_mysql[mysql_str_len(16)];
    set_mysql_string(nick_mysql, p_in->nick, 16);
    GEN_SQLSTR(sqlstr, "replace into %s (userid,role_regtime,reward_flg,role_type,role_nick,layer,time_interval,userlv,place_tm) \
         values (%u, %u, 0, %u, '%s', %u, %u, %u, unix_timestamp());", this->get_table_name(), userid, role_tm, p_in->role_type,
         nick_mysql, p_in->layer, p_in->interval, p_in->userlv);
    return this->exec_update_sql(this->sqlstr, GF_HUNTER_TOP_ERR);

}

int Cgf_hunter_top::del_overflowed_info(uint32_t userid, uint32_t role_tm)
{
	GEN_SQLSTR(sqlstr, "delete from %s where userid=%u and role_regtime=%u and reward_flg=0", this->get_table_name(), userid, role_tm);
	return this->exec_update_sql(this->sqlstr, GF_HUNTER_TOP_ERR);
}

int Cgf_hunter_top::cut_hunter_top(uint32_t role_type)
{
	uint32_t c_uid = 0, c_roletm = 0;
	uint32_t hunter_cnt = 0;
	get_top_cnt(&hunter_cnt, role_type);
	if (hunter_cnt > 100) {
		get_one_over_tower_info(role_type, &c_uid, &c_roletm);
		del_overflowed_info(c_uid, c_roletm);
	}
	return 0;
}

int Cgf_hunter_top::get_top_cnt(uint32_t* p_count, uint32_t role_type)
{
	sprintf( this->sqlstr, "select count(*) from %s  where reward_flg=0 and role_type=%u",
		this->get_table_name(), role_type);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR );
	INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}
int Cgf_hunter_top::get_hunter_order(uint32_t role_type, uint32_t layer, uint32_t interval, uint32_t place_tm, uint32_t* order, uint32_t reward_flg)
{
	if (reward_flg) {
		GEN_SQLSTR(sqlstr, "select count(*) from %s \
			where  reward_flg>0 and role_type=%u \
		   	and (layer>%u or (layer = %u and time_interval<%u) or \
			(layer = %u and time_interval=%u and place_tm < %u) )",
	        this->get_table_name(), role_type, layer, layer, interval, layer, interval, place_tm);
	} else {
	    GEN_SQLSTR(sqlstr, "select count(*) from %s \
			where  reward_flg=0 and role_type=%u \
			and (layer>%u or (layer = %u and time_interval<%u) or \
			(layer = %u and time_interval=%u and place_tm < %u) )",
	        this->get_table_name(), role_type, layer, layer, interval, layer, interval, place_tm);
	}
    STD_QUERY_ONE_BEGIN(sqlstr, GF_HUNTER_TOP_ERR);
        INT_CPY_NEXT_FIELD(*order);
    STD_QUERY_ONE_END();
}

int Cgf_hunter_top::get_hunter_top_info(uint32_t role_type, uint32_t top_num, uint32_t* count, gf_get_hunter_top_info_out_item** pout)
{
	GEN_SQLSTR(sqlstr, "select userid,role_regtime,role_type,role_nick,layer,time_interval,userlv \
        from %s where reward_flg=0 and role_type=%u order by layer desc,time_interval,place_tm limit %u", 
        this->get_table_name(), role_type, top_num);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pout, count);
		INT_CPY_NEXT_FIELD( (*pout+i)->userid );
		INT_CPY_NEXT_FIELD( (*pout+i)->roletm );
		INT_CPY_NEXT_FIELD( (*pout+i)->role_type );
		BIN_CPY_NEXT_FIELD( (*pout+i)->nick, sizeof((*pout+i)->nick) );
		INT_CPY_NEXT_FIELD( (*pout+i)->layer );
		INT_CPY_NEXT_FIELD( (*pout+i)->interval );
		INT_CPY_NEXT_FIELD( (*pout+i)->userlv );
	STD_QUERY_WHILE_END();
}

int Cgf_hunter_top::get_hunter_top_info(uint32_t role_type, uint32_t top_num, uint32_t* count, gf_get_hunter_top_info_for_web_out_item** pout)
{
	GEN_SQLSTR(sqlstr, "select userid,role_regtime,role_type,role_nick,layer,time_interval,userlv \
        from %s where reward_flg=0 and role_type=%u order by layer desc,time_interval,place_tm limit %u", 
        this->get_table_name(), role_type, top_num);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pout, count);
		INT_CPY_NEXT_FIELD( (*pout+i)->userid );
		INT_CPY_NEXT_FIELD( (*pout+i)->roletm );
		INT_CPY_NEXT_FIELD( (*pout+i)->role_type );
		BIN_CPY_NEXT_FIELD( (*pout+i)->nick, sizeof((*pout+i)->nick) );
		INT_CPY_NEXT_FIELD( (*pout+i)->layer );
		INT_CPY_NEXT_FIELD( (*pout+i)->interval );
		INT_CPY_NEXT_FIELD( (*pout+i)->userlv );
	STD_QUERY_WHILE_END();
}

int Cgf_hunter_top::get_hunter_old_topXX(uint32_t top_num, uint32_t role_type, uint32_t* count, gf_get_hunter_old_top100_out_item** pout)
{
	if (role_type == 0) {	
		GEN_SQLSTR(sqlstr, "select userid,role_regtime,reward_flg \
    	    from %s where reward_flg>0 order by layer desc,time_interval limit %u", this->get_table_name(), top_num);
	} else {
		GEN_SQLSTR(sqlstr, "select userid,role_regtime,reward_flg \
    	    from %s where reward_flg>0 and role_type=%u order by layer desc,time_interval limit %u", this->get_table_name(), role_type, top_num);
	}
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pout, count);
		INT_CPY_NEXT_FIELD( (*pout+i)->userid );
		INT_CPY_NEXT_FIELD( (*pout+i)->roletm );
		INT_CPY_NEXT_FIELD( (*pout+i)->reward_flg );
	STD_QUERY_WHILE_END();
}

int Cgf_hunter_top::get_role_type(uint32_t userid, uint32_t role_tm, uint32_t* p_role_type)
{
    GEN_SQLSTR(sqlstr, "select role_type from %s \
		where  userid=%u and role_regtime=%u limit 1;",
        this->get_table_name(), userid, role_tm);
    STD_QUERY_ONE_BEGIN(sqlstr, GF_HUNTER_TOP_ERR);
        INT_CPY_NEXT_FIELD(*p_role_type);
    STD_QUERY_ONE_END();

}

int Cgf_hunter_top::get_hunter_old_place(uint32_t userid, uint32_t role_tm, uint32_t role_type)
{
	gf_get_hunter_old_top100_out_item* p_item = 0;
	uint32_t count = 0;
	uint32_t old_place = 0;
	int ret = get_hunter_old_topXX(10, role_type, &count, &p_item);
	if (ret != SUCC) 
	{
		if (p_item) {
			free(p_item);
			p_item = NULL;
		}
		return 0;
	}
	for (uint32_t i = 0; i < count; i++)
	{
		if (p_item[i].userid == userid && p_item[i].roletm == role_tm)
		{
			old_place = p_item[i].reward_flg;
			if (p_item) {
				free(p_item);
				p_item = NULL;
			}
			return old_place;
		}
	}
	if (p_item) {
		free(p_item);
		p_item = NULL;
	}
	return 0;
}

int Cgf_hunter_top::del_player_place_in_old_top(uint32_t userid, uint32_t role_tm)
{
	GEN_SQLSTR(sqlstr, "update %s set reward_flg=2 where userid=%u and role_regtime=%u and reward_flg>0", this->get_table_name(), userid, role_tm);
		return this->exec_update_sql(this->sqlstr, GF_HUNTER_TOP_ERR);
}

