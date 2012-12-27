/*
 * =====================================================================================
 *
 *       Filename:  Angel_battle.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/26/2011 04:06:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include <sstream>
#include <algorithm>
#include "proto.h"
#include "Angel_battle.h"

uint32_t level_th[] = {
    50, 110, 210, 330, 480, 680, 910, 1160, 1460, 1810, 2210, 2660, 3210, 3810, 4510,
        5360, 6360, 7560, 9060, 10860, 12960, 15460, 18360, 21660, 25660, 30060, 34860,
              40060, 45660, 51660, 58060, 64860, 72060, 79660, 88260, 97260, 106660,
                     116460, 126660, 137260
};


int get_level(uint32_t exp, uint32_t *level)
{
	uint32_t k = 0;
    for(; k < sizeof(level_th)/sizeof(uint32_t); ++k){
        if(exp < level_th[k]){
            *level = k + 1;
			return 0;
        }
    }
    if(k >= sizeof(level_th)/sizeof(uint32_t)){
        *level = 40;
    }
    return 0;
}
Angel_battle::Angel_battle(mysql_interface *db):
	Ctable(db, "DININGROOM", "t_angel_battle")
{

}

int Angel_battle::add_exp(userid_t userid, uint32_t exp)
{
	int ret = update(userid, exp);
	if(ret == USER_ID_NOFIND_ERR){
		ret = insert(userid, exp);
	}
	return ret;
}

int Angel_battle::insert(userid_t userid, uint32_t exp)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, 0x00000000, 0, 0, 0)",
			this->get_table_name(),
			userid,
			exp
			);

	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Angel_battle::insert(userid_t userid, uint32_t exp, bat_arg_event_t *events)
{

	char my_events[mysql_str_len(sizeof(bat_arg_event_t))];
	memset(my_events, 0, sizeof(my_events));
	set_mysql_string(my_events, (char*)events, sizeof(bat_arg_event_t));
	sprintf(this->sqlstr, "insert into %s vaules(%u, %u, '%s', 0, 0, 0)",
			this->get_table_name(),
			userid,
			exp,
			my_events
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Angel_battle::update(userid_t userid, uint32_t exp)
{
	sprintf(this->sqlstr, "update %s set exp = %u where userid = %u",
			this->get_table_name(),
			exp,
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Angel_battle::select_friend_exp(userid_t userid, uint32_t in_count,
	   	angel_battle_get_friends_exp_in_item *p_in_item, uint32_t *out_count, 
		 angel_battle_get_friends_exp_out_item **p_out_item)
{
	std::ostringstream in_str;
	for(uint32_t i = 0; i < in_count; ++i){
		in_str << (p_in_item + i)->friend_id;
		if(i < in_count - 1){
			in_str << ',';
		}
	}
	sprintf(this->sqlstr, "select userid, exp from %s where userid  in (%s)",
			this->get_table_name(),
			in_str.str().c_str()
		   );

	STD_QUERY_WHILE_BEGIN(this->sqlstr, p_out_item, out_count);
		INT_CPY_NEXT_FIELD((*p_out_item + i)->friend_id);
		INT_CPY_NEXT_FIELD((*p_out_item + i)->exp);
		get_level((*p_out_item + i)->exp, &(*p_out_item + i)->level);
	STD_QUERY_WHILE_END();
}

int Angel_battle::get_variety_events(userid_t userid, user_battle_get_friend_event_info_in_item* p_in_item, 
		uint32_t in_count, user_battle_get_friend_event_info_out_item **p_out_item,  uint32_t *out_count)
{
	std::ostringstream in_str;
	for(uint32_t i = 0; i < in_count; ++i){
		in_str << (p_in_item + i)->friend_id;
		if(i < in_count - 1){
			in_str << ',';
		}
	}
	sprintf(this->sqlstr, "select userid, event, wish_count, datetime from %s where userid  in (%s)",
			this->get_table_name(),
			in_str.str().c_str()
		   );

	uint32_t now = time(0);
	uint32_t datetime = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, p_out_item, out_count);
		INT_CPY_NEXT_FIELD((*p_out_item + i)->friend_id);
        BIN_CPY_NEXT_FIELD ((char*)(&(*p_out_item + i)->events), sizeof(bat_arg_event_t) );
		INT_CPY_NEXT_FIELD((*p_out_item + i)->wish_count);
		INT_CPY_NEXT_FIELD(datetime);
		if((now - datetime) / (24 *3600) >= 10){
			(*p_out_item + i)->is_summon = 1;
		}
	STD_QUERY_WHILE_END();
}
int Angel_battle::get_prentice_level_and_master(userid_t userid, user_battle_get_prentice_market_in_item* p_in_item,
		uint32_t in_count,  user_battle_get_prentice_market_out_item** p_out_item, uint32_t *out_count)
{
	std::ostringstream in_str;
	for(uint32_t i = 0; i < in_count; ++i){
		in_str << (p_in_item + i)->userid;
		if(i < in_count - 1){
			in_str << ',';
		}
	}
	sprintf(this->sqlstr, "select userid, exp, master from %s where userid in (%s)",
			this->get_table_name(),
			in_str.str().c_str()
		   );
	STD_QUERY_WHILE_BEGIN(this->sqlstr, p_out_item, out_count);
		INT_CPY_NEXT_FIELD((*p_out_item + i)->userid);
		INT_CPY_NEXT_FIELD((*p_out_item + i)->exp);
		get_level((*p_out_item + i)->exp, &(*p_out_item + i)->level);
		INT_CPY_NEXT_FIELD((*p_out_item + i)->master);
	STD_QUERY_WHILE_END();

}

int Angel_battle::get_master_level(user_battle_get_prentice_market_out_item* p_out_item, uint32_t out_count)
{
	int master_cnt = 0;
	std::ostringstream in_str;
    for(uint32_t i = 0; i < out_count; ++i) {
		if ((p_out_item + i)->master != 0) {
			if(master_cnt == 0){
				in_str << (p_out_item + i)->master;
			}
			else{
				in_str << ','<<(p_out_item + i)->master;
			}
			master_cnt++;
		}
    }
	if (master_cnt == 0) {
		return 0;
	}
	DEBUG_LOG("get_master_level: [%s] [%u]", in_str.str().c_str(), master_cnt);
	sprintf(this->sqlstr, "select userid, exp from %s where userid in (%s)",
			this->get_table_name(),
			in_str.str().c_str()
			);
	uint32_t exp = 0, level = 0;
	uint32_t inner_cnt = 0;
	uint32_t *p_temp = 0;
	user_battle_get_prentice_market_out_item other = { 0 };
	user_battle_get_prentice_market_out_item *iter = 0;	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, &p_temp, &inner_cnt);
		INT_CPY_NEXT_FIELD(other.master);
		INT_CPY_NEXT_FIELD(exp);
		get_level(exp, &level);
		iter = std::find(p_out_item, (p_out_item) + out_count, other);
		if(iter != p_out_item + out_count){
			iter->m_level = level;
		}
	STD_QUERY_WHILE_END_WITHOUT_RETURN();
	if(p_temp != 0){
		free(p_temp);
	}
	return 0;
}

int Angel_battle::update_events(userid_t userid, bat_arg_event_t *events)
{
	char my_events[mysql_str_len(sizeof(bat_arg_event_t))];
	memset(my_events, 0, sizeof(my_events));
	set_mysql_string(my_events, (char*)events, sizeof(bat_arg_event_t));
	sprintf(this->sqlstr, "update %s set event = '%s' where userid = %u",
			this->get_table_name(),
			my_events,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Angel_battle::get_event(userid_t userid, bat_arg_event_t *events)
{
	sprintf(this->sqlstr, "select event from %s where userid = %u",
			this->get_table_name(),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        BIN_CPY_NEXT_FIELD ((char*)events, sizeof(bat_arg_event_t) );
	STD_QUERY_ONE_END();
}

int Angel_battle::get_wish_count(userid_t userid, uint32_t *wish_count)
{
	sprintf(this->sqlstr, "select wish_count from %s where userid = %u",
			this->get_table_name(),
			userid
		   );

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*wish_count);
	STD_QUERY_ONE_END();
}

int Angel_battle::set_wish_count(userid_t userid, uint32_t wish_count)
{
	sprintf(this->sqlstr, "update %s set wish_count = %u where userid = %u",
			this->get_table_name(),
			wish_count,
			userid
		   );

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Angel_battle::get_last_login_time(userid_t userid, uint32_t *datetime)
{
	sprintf(this->sqlstr, "select datetime from %s where userid = %u",
			this->get_table_name(),
			userid
		   );

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*datetime);
	STD_QUERY_ONE_END();

}

int Angel_battle::set_last_login_time(userid_t userid, uint32_t datetime)
{
	sprintf(this->sqlstr, "update %s set datetime = %u where userid = %u",
			this->get_table_name(),
			datetime,
			userid
		   );

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Angel_battle::get_one_colum(userid_t userid, const char* col, uint32_t *value)
{
	sprintf(this->sqlstr, "select %s from %s where userid = %u",
			col,
			this->get_table_name(),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*value);
	STD_QUERY_ONE_END();
}

int Angel_battle::update_one_column(userid_t userid, const char* col, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u",
			this->get_table_name(),
			col,
			value,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Angel_battle::get_two_column(userid_t userid, const char* col_1, const char* col_2, uint32_t* value_1, uint32_t* value_2)
{
	sprintf(this->sqlstr, "select %s, %s from %s where userid = %u",
			col_1,
			col_2,
			this->get_table_name(),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*value_1);
		INT_CPY_NEXT_FIELD(*value_2);
	STD_QUERY_ONE_END();
}
