/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_fight_record.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/13/2011 09:24:42 AM
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
#include "Cuser_battle_fight_record.h"

Cuser_battle_fight_record::Cuser_battle_fight_record(mysql_interface *db):
	CtableRoute100x10(db, "USER","t_user_battle_fight_record","userid")
{

}

int Cuser_battle_fight_record::get_all_info(userid_t userid, user_battle_gefight_over_info_out_item **pp_list,
	   	uint32_t *p_count)
{
	sprintf(this->sqlstr, "select rival_id, is_initial, is_win, exp_inc, fight_time, msg_type from %s \
			where userid = %u limit 50",
			this->get_table_name(userid),
			userid
			);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD ((*pp_list + i)->rival_id);
		INT_CPY_NEXT_FIELD ((*pp_list + i)->is_initial);
		INT_CPY_NEXT_FIELD ((*pp_list + i)->is_win);
		INT_CPY_NEXT_FIELD ((*pp_list + i)->exp_inc);
		INT_CPY_NEXT_FIELD ((*pp_list + i)->fight_time);
		INT_CPY_NEXT_FIELD ((*pp_list + i)->msg_type);
	STD_QUERY_WHILE_END();

}

int Cuser_battle_fight_record::get_rival(userid_t userid, uint32_t **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select rival_id from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD (*(*pp_list + i));
	STD_QUERY_WHILE_END();

}

int Cuser_battle_fight_record::insert(userid_t userid, uint32_t rival_id, uint32_t is_initial, 
		uint32_t is_win, uint32_t exp_inc, uint32_t fight_tim, uint32_t msg_type)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			rival_id,
			is_initial,
			is_win,
			exp_inc,
			fight_tim,
			msg_type
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}
int Cuser_battle_fight_record::get_oldest(userid_t userid, uint32_t *datetime)
{
	sprintf(this->sqlstr, "select MIN(fight_time) from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*datetime);
	STD_QUERY_ONE_END();
}

int Cuser_battle_fight_record::del_friend_info(userid_t userid, uint32_t friend_id)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and rival_id = %u",
			this->get_table_name(userid),
			userid,
			friend_id
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_battle_fight_record::update(userid_t userid, user_battle_gefight_over_info_out_item *para, uint32_t datetime)
{
	sprintf(this->sqlstr, "update %s set fight_time = %u, rival_id = %u, is_initial = %u, is_win = %u, \
			exp_inc = %u , msg_type = %u where userid = %u and fight_time = %u",
			this->get_table_name(userid),
			para->fight_time,
			para->rival_id,
			para->is_initial,
			para->is_win,
			para->exp_inc,
			para->msg_type,
			userid,
			datetime
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_fight_record::get_record_cnt(userid_t userid, uint32_t &cnt)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (cnt);
	STD_QUERY_ONE_END();
}

int Cuser_battle_fight_record::update_records(userid_t userid, user_battle_gefight_over_info_out_item *para)
{
	uint32_t r_cnt = 0;
	get_record_cnt(userid, r_cnt);
	DEBUG_LOG("!!!!!!!!!!!!!!!!!!!!!!!!  r_cnt: %u",r_cnt);
	if(r_cnt < 50){
		insert(userid, para->rival_id, para->is_initial, para->is_win, para->exp_inc, para->fight_time,
				para->msg_type);
	}
	else{
		uint32_t datetime = 0;
		get_oldest(userid, &datetime);
		update(userid, para, datetime);
	}
	return 0;
}
