/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_friend_event.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/22/2011 05:01:11 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#include <ctime> 
#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "common.h"
#include <map>
#include <sstream>
#include "Cuser_battle_friend_event.h"

Cuser_battle_friend_event::Cuser_battle_friend_event(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_battle_friend_event", "userid")
{

}

int Cuser_battle_friend_event::insert(userid_t userid, uint32_t friend_id, bat_arg_event_t *events)
{
	char my_events[mysql_str_len(sizeof(bat_arg_event_t))];
	memset(my_events, 0, sizeof(my_events));
	set_mysql_string(my_events, (char*)events, sizeof(bat_arg_event_t));
	sprintf(this->sqlstr, "insert into %s values(%u, %u, '%s', 0)",
			this->get_table_name(userid),
			userid,
			friend_id,
			my_events
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_battle_friend_event::insert(userid_t userid, uint32_t friend_id )
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, 0x00000000, 1)",
			this->get_table_name(userid),
			userid,
			friend_id
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}


int Cuser_battle_friend_event::update(userid_t userid, uint32_t friend_id, bat_arg_event_t *events)
{
	char my_events[mysql_str_len(sizeof(bat_arg_event_t))];
	memset(my_events, 0, sizeof(my_events));
	set_mysql_string(my_events, (char*)events, sizeof(bat_arg_event_t));
	sprintf(this->sqlstr, "update %s set event = '%s' where userid = %u and friend_id = %u",
			this->get_table_name(userid),
			my_events,
			userid,
			friend_id
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_friend_event::get_friend_event(userid_t userid, uint32_t friend_id, bat_arg_event_t *events)
{
	sprintf(this->sqlstr, "select event from %s where userid = %u and friend_id = %u",
			this->get_table_name(userid),
			userid,
			friend_id
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
        BIN_CPY_NEXT_FIELD ((char*)events, sizeof(bat_arg_event_t) );
	STD_QUERY_ONE_END();
}

int Cuser_battle_friend_event::get_friend_fight_count(userid_t userid, uint32_t friend_id, uint32_t *fight_count)
{
	sprintf(this->sqlstr, "select fight_count from %s where userid = %u and friend_id = %u",
			this->get_table_name(userid),
			userid,
			friend_id
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		*fight_count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}
int Cuser_battle_friend_event::update_fight_count_inc(userid_t userid, uint32_t friend_id, uint32_t fight_count)
{
	sprintf(this->sqlstr, "update %s set fight_count = fight_count + %u where userid = %u and friend_id = %u",
			this->get_table_name(userid),
			fight_count,
			userid,
			friend_id
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_friend_event::clear_fight_count(userid_t userid)
{
	sprintf(this->sqlstr, "update %s set fight_count = 0  where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_SET_RETURN_WITH_NO_AFFECTED( sqlstr);
}

int Cuser_battle_friend_event::get_friend_fight_list(userid_t userid, friend_fight_t **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select friend_id, fight_count from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list,p_count ) ;
		(*pp_list+i)->friend_id = atoi_safe(NEXT_FIELD); 
		(*pp_list+i)->fight_count = atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}

int Cuser_battle_friend_event::get_rival_count(userid_t userid,  uint32_t *rival_cnt)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		*rival_cnt=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

int Cuser_battle_friend_event::get_friend_local_events(userid_t userid, user_get_self_events_info_in_item* p_in_item, 
		uint32_t in_count, user_get_self_events_info_out_item **p_out_item, uint32_t *out_count)
{
	std::ostringstream in_str;
	for(uint32_t i = 0; i < in_count; ++i){
		in_str << (p_in_item + i)->friend_id;
		if(i < in_count - 1){
			in_str << ',';
		}
	}
	sprintf(this->sqlstr, "select friend_id, event, fight_count  from %s where userid = %u and friend_id in (%s)",
			this->get_table_name(userid),
			userid,
			in_str.str().c_str()
		   );

	STD_QUERY_WHILE_BEGIN(this->sqlstr, p_out_item, out_count);
		INT_CPY_NEXT_FIELD((*p_out_item + i)->friend_id);
        BIN_CPY_NEXT_FIELD ((char*)(&(*p_out_item + i)->events), sizeof(bat_arg_event_t) );
		INT_CPY_NEXT_FIELD((*p_out_item + i)->fight_count);
	STD_QUERY_WHILE_END();

}
