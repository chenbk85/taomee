/*
 * =====================================================================================
 *
 *       Filename:  Cuser_self_games.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/18/2012 05:15:35 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_self_games.h"

struct game_score_level{
    uint32_t gameid;
    uint32_t level[5];
};

game_score_level game_score[] = {
    {47, { 300, 600, 800, 1000, 1200}},
    { 42,{3000, 10000, 18000, 24000, 29000}},
    { 1,{4000, 8000, 10000, 12000, 13000}},
    { 33,{3000, 7000, 10000, 12000, 14000}},
    { 48,{3000, 6000, 8000, 10000, 11000}},
    { 9,{3000, 7000, 10000, 12000, 13000}},
    { 67,{20, 30, 40, 50, 60}},
    { 24,{2000, 4000, 5000, 6000, 7000}},
    { 20,{1000, 3000, 4000, 5000, 6000}},
    { 85,{600, 1200, 1600, 2000, 2200}},
    { 49,{400, 700, 900, 1100, 1200}},
    { 36,{6000, 18000, 26000, 32000, 35000}},
    { 41,{4000, 12000, 18000, 22000, 25000}},
    { 95,{2000, 6000, 8000, 9000, 10000}},
    { 59,{2000, 5000, 8000, 10000, 12000}},
    { 46,{1000, 4000, 6000, 7500, 8500}},
    { 56,{300, 800, 1000, 1200, 1300}},
    { 26,{800, 1600, 2200, 2600, 2800}},
    { 86,{1000, 3000, 5000, 6000, 7000}},
    { 25,{1000, 2500, 3500, 4000, 4500}},
    { 55,{200, 400, 600, 700, 800}},
    { 8,{2000, 4000, 6000, 7000, 8000}},
    { 19,{2000, 4000, 6000, 7000, 8000}},
    { 16,{3000, 6000, 8000, 10000, 12000}},
};



Cuser_self_games::Cuser_self_games(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_self_games", "userid")
{

}

int Cuser_self_games::insert(userid_t userid, uint32_t gameid, uint32_t score)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u)",
			this->get_table_name(userid),
			userid,
			gameid,
			score
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}


int Cuser_self_games::update(userid_t userid, uint32_t gameid, uint32_t score)
{
	sprintf(this->sqlstr, "update %s set score = %u where userid = %u and gameid = %u",
			this->get_table_name(userid),
			score,
			userid,
			gameid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);

}


int Cuser_self_games::add(userid_t userid, uint32_t gameid, uint32_t score)
{
	int m_ret = update(userid, gameid, score);
	if(m_ret != SUCC){
		m_ret = insert(userid, gameid, score);
	}
	return m_ret;
}

int Cuser_self_games::get_all(userid_t userid, user_get_games_info_out_item **pp_list, uint32_t* p_count)
{
	sprintf(this->sqlstr, "select gameid, score from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list+i)->gameid);
		INT_CPY_NEXT_FIELD((*pp_list+i)->exp);
	STD_QUERY_WHILE_END();	
}

int Cuser_self_games::get_score_by_gameid(userid_t userid, uint32_t gameid, uint32_t *score)
{
	sprintf(this->sqlstr, "select score from %s where userid = %u and gameid = %u",
			this->get_table_name(userid),
			userid,
			gameid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*score);
	STD_QUERY_ONE_END();	
}

int Cuser_self_games::cal_exp(userid_t userid, uint32_t gameid, uint32_t score, uint32_t *exp)
{
	*exp = 0;
	uint32_t old_score = 0;
	int m_ret = get_score_by_gameid(userid, gameid, &old_score);

	uint32_t new_score = score;
	uint32_t exp_inc[] = { 100, 200, 300, 400, 1000};	
	uint32_t i = 0; 
	for(;i < sizeof(game_score)/sizeof(game_score_level); ++i){
		if(gameid == game_score[i].gameid){
			break;
		}	
	}
	if(i >= sizeof(game_score)/sizeof(game_score_level)){
		return -1;
	}

	if(new_score > old_score){
		uint32_t j = 0; 
		for(;j < 5; ++j){
			if(old_score < game_score[i].level[j]){
				break;
			}
		}

		uint32_t k = 0;
		for(;k < 5; ++k){
			if(new_score < game_score[i].level[k]){
				break;
			}
		}
		
		if( k > j){
			for(uint32_t m = j; m < k; ++m){
				*exp += exp_inc[m];
			}
		}

		m_ret = add(userid, gameid, new_score);

	}//if

	DEBUG_LOG("new_score: %u, old_score: %u", new_score, old_score);

	return m_ret;
}
