/*
 * =====================================================================================
 *
 *       Filename:  Cuser_dig_treasure_info.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/18/2011 03:10:32 PM
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
#include "Cuser_dig_treasure_info.h"

Cuser_dig_treasure_info::Cuser_dig_treasure_info(mysql_interface *db):
    CtableRoute100x10(db, "USER", "t_user_dig_treasure_info","userid")
{

}
uint32_t level_exp_boundary [] ={
	48, 138, 268, 454, 712, 1058, 1508, 2078, 2784, 3642, 4668, 5878, 
				7288, 8914, 10772, 12878, 15248, 17898, 20844
};
//, 20844

int Cuser_dig_treasure_info::get_level(uint32_t exp, uint32_t &level)
{
	uint32_t k = 0;
	for(; k < sizeof(level_exp_boundary)/sizeof(uint32_t); ++k){
		if(exp < level_exp_boundary[k]){
			level = k + 1;
			break;
		}
	}	
	if(k >=  sizeof(level_exp_boundary)/sizeof(uint32_t)){
		level = 20;
	}
	return 0;	
}

int Cuser_dig_treasure_info::get_hp_boundary(uint32_t level, uint32_t &hp_boundary)
{
	hp_boundary = 36;
	for(uint32_t i = 2; i <= level; ++i){
		hp_boundary += 6;
	}
	return 0;
}

int Cuser_dig_treasure_info::get_info_game_begin (userid_t user_id, uint32_t *health,
                                                  uint32_t *exp, uint32_t *lasttime,
												  uint32_t *dig_counts)
{
    snprintf(this->sqlstr, sizeof(this->sqlstr), "select  health, exp, last_time, dig_counts \
			from %s where userid = %u ",
            this->get_table_name(user_id),
		   	user_id
			);

    STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*health);
        INT_CPY_NEXT_FIELD(*exp);
        INT_CPY_NEXT_FIELD(*lasttime);
        INT_CPY_NEXT_FIELD(*dig_counts);
    STD_QUERY_ONE_END();

}

int Cuser_dig_treasure_info::get_hp (userid_t user_id, uint32_t &health)
{
    snprintf(this->sqlstr, sizeof(this->sqlstr), "select  health\
			from %s where userid = %u ",
            this->get_table_name(user_id),
		   	user_id
			);

    STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(health);
    STD_QUERY_ONE_END();

}

int Cuser_dig_treasure_info::get_lasttime (userid_t user_id, uint32_t &tim)
{
    snprintf(this->sqlstr, sizeof(this->sqlstr), "select  last_time\
			from %s where userid = %u ",
            this->get_table_name(user_id),
		   	user_id
			);

    STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(tim);
    STD_QUERY_ONE_END();

}
int Cuser_dig_treasure_info::get_exp (userid_t user_id, uint32_t &exp)
{
    snprintf(this->sqlstr, sizeof(this->sqlstr), "select  exp\
			from %s where userid = %u ",
            this->get_table_name(user_id),
		   	user_id
			);

    STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(exp);
    STD_QUERY_ONE_END();

}
int Cuser_dig_treasure_info::init_user(userid_t user_id)
{
	uint32_t tim = (uint32_t)time(0);
    snprintf(this->sqlstr, sizeof(this->sqlstr), "insert into %s values('%u', 100, 0, '%u',0)",
            this->get_table_name(user_id),
		   	user_id,
			tim
			);

     STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_dig_treasure_info::update(userid_t user_id, uint32_t hp, uint32_t exp, uint32_t tim, uint32_t dig_counts)
{
	snprintf(this->sqlstr, sizeof(this->sqlstr), "update %s set health = %u, exp = %u, last_time = %u, dig_counts = %u where\
			userid = %u",
			this->get_table_name(user_id),
			hp,
			exp,
			tim,
			dig_counts,
			user_id
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_dig_treasure_info::update_hp(userid_t user_id, uint32_t value, uint32_t tim)
{
	snprintf(this->sqlstr, sizeof(this->sqlstr), "update %s set health = %u, last_time = %u where\
			userid = %u",
			this->get_table_name(user_id),
			value,
			tim,
			user_id
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_dig_treasure_info::update_dig_counts(userid_t userid, uint32_t dig_counts)
{
	snprintf(this->sqlstr, sizeof(this->sqlstr), "update %s set dig_counts = %u where userid = %u",
			this->get_table_name(userid),
			dig_counts,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
