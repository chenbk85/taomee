/*
 * =====================================================================================
 *
 *       Filename:  Cuser_dig_treasure.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/18/2011 03:08:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include <time.h> 
#include <algorithm>
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "common.h"
#include "Cuser_dig_treasure.h"
 
Cuser_dig_treasure::Cuser_dig_treasure(mysql_interface *db):
    CtableRoute100x10(db, "USER", "t_user_dig_treasure","userid")
{

}

int Cuser_dig_treasure::get_info_game_begin (userid_t user_id, uint32_t map_id, uint32_t event_id ,
		uint32_t *count, uint32_t *date)
{
    snprintf(this->sqlstr, sizeof(this->sqlstr), "select  count, date from %s where \
			userid = %u and mapid = %u and eventid = %u",
            this->get_table_name(user_id), 
			user_id,
			map_id,
		   	event_id
			);

    STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*count);
        INT_CPY_NEXT_FIELD(*date);
    STD_QUERY_ONE_END();        

}

int Cuser_dig_treasure::get_event_happen_date(userid_t user_id, uint32_t map_id, uint32_t event_id,
	   	uint32_t *date)
{
    snprintf(this->sqlstr, sizeof(this->sqlstr), "select date from %s where \
			userid = %u and mapid = %u and eventid = %u",
            this->get_table_name(user_id), 
			user_id,
			map_id,
		   	event_id
			);

    STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*date);
    STD_QUERY_ONE_END();        

}


int Cuser_dig_treasure::get_random_event_counts(userid_t user_id, uint32_t map_id,
	   	user_get_dig_treasure_begin_out_item **pp_list, uint32_t *count)
{
	uint32_t today = (uint32_t)get_date(time(0));
    snprintf(this->sqlstr, sizeof(this->sqlstr), "select eventid, count from %s where \
			userid = %u and mapid = %u and date = %u",
            this->get_table_name(user_id),
		   	user_id,
			map_id,
			today
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->element_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->count);
	STD_QUERY_WHILE_END();	

}

int Cuser_dig_treasure::add_user_dig_map(userid_t user_id, uint32_t map_id,  uint32_t event_id,
		 uint32_t count)
{
	uint32_t today = (uint32_t)get_date(time(0));
    snprintf(this->sqlstr, sizeof(this->sqlstr), "insert into %s values('%u', '%u', '%u',\
			'%u', '%u')",
            this->get_table_name(user_id), 
			user_id, 
			map_id,
			event_id,
			count,
			today
			);

     STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_dig_treasure::update_map_entern_count (userid_t user_id, uint32_t map_id, uint32_t event_id,
		uint32_t count)
{
	//uint32_t date = 0;
	//get_event_happen_date(user_id, map_id, event_id, &date);
	uint32_t today = (uint32_t)get_date(time(0));
	snprintf(this->sqlstr, sizeof(this->sqlstr), "update %s set count = %u  \
		,date = %u where userid = %u and mapid = %u and eventid = %u",
		this->get_table_name(user_id), 
		count,
		today,
		user_id,
		map_id,
		event_id
		);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
