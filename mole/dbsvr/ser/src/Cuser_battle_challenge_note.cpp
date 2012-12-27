/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_challenge_note.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/23/2011 10:18:05 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_battle_challenge_note.h"
#include <string.h>
enum LEVEL_TYPE{
	LT_NORMAL   = 0,
	LT_BOSS,
	LT_BOSS_TIME,
	LT_BOSS_VIP,
	LT_BOSS_COST,
	LT_FRIEND,
	LT_GET_APPRENTICE,      //收徒弟
	LT_GRAB_APPRENTICE,     //抢徒弟
	LT_PK_MASTER,           //挑战师傅
	LT_PK_APPRENTICE,       //挑战徒弟
	LT_TRAIN,               //切磋武艺
	LT_PK, //PVP
	LT_BOSS_ACTIVE,
};

Cuser_battle_challenge_note::Cuser_battle_challenge_note(mysql_interface *db)
	:CtableRoute100x10(db, "USER", "t_user_battle_challenge_note", "userid")
{

}

int Cuser_battle_challenge_note::insert(userid_t userid, uint32_t rival, uint32_t count,
	   	uint32_t threshhold, uint32_t vip_cnt, uint32_t time_cnt, uint32_t pvp_cnt, uint32_t active_cnt)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u, %u, %u, %u, %u)",
			this->get_table_name(userid),
			userid,
			rival,
			count,
			threshhold,
			(uint32_t)get_date(time(0)),
			vip_cnt,
			time_cnt,
			pvp_cnt,
			active_cnt
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_battle_challenge_note::update(userid_t userid, uint32_t rival, const char* col, 
		uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u and rival= %u",
			this->get_table_name(userid),
			col,
			value,
			userid,
			rival
			);	

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_challenge_note::update_inc(userid_t userid, uint32_t rival, const char* col, 
		uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %s + %u where userid = %u and rival= %u",
			this->get_table_name(userid),
			col,
			col,
			value,
			userid,
			rival
			);	

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_challenge_note::update_info(userid_t userid, uint32_t rival, uint32_t count,
		uint32_t limit, uint32_t date)
{
	sprintf(this->sqlstr, "update %s set count = %u, threshold = %u, date = %u  where userid = %u \
			and rival= %u",
			this->get_table_name(userid),
			count,
			limit,
			date,
			userid,
			rival
			);	

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_battle_challenge_note::clear_challenge_cnt(userid_t userid, uint32_t rival, uint32_t date)
{
	sprintf(this->sqlstr, "update %s set count = 0, vip_cnt = 0, time_cnt = 0, pvp_cnt = 0, active_cnt = 0,  date = %u  where \
			userid = %u and rival = %u",
			this->get_table_name(userid),
			date,
			userid,
			rival
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_challenge_note::get_challenge_cnt(userid_t userid, uint32_t rival, uint32_t *nor_cnt,
		uint32_t *vip_cnt, uint32_t *time_cnt, uint32_t *active_cnt, uint32_t *date)
{
	sprintf(this->sqlstr, "select count, vip_cnt, time_cnt, active_cnt, date from %s where userid = %u and rival = %u",
			this->get_table_name(userid),
			userid,
			rival
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*nor_cnt);
		INT_CPY_NEXT_FIELD (*vip_cnt);
		INT_CPY_NEXT_FIELD (*time_cnt);
		INT_CPY_NEXT_FIELD (*active_cnt);
		INT_CPY_NEXT_FIELD (*date);
	STD_QUERY_ONE_END();
}

int Cuser_battle_challenge_note::update_two_column(userid_t userid, uint32_t rival, 
		const char* col_1, const char* col_2, uint32_t value_1, uint32_t value_2)
{
	sprintf(this->sqlstr, "update %s set %s = %u, %s = %u where userid = %u and rival = %u",
			this->get_table_name(userid),
			col_1,
			value_1,
			col_2,
			value_2,
			userid,
			rival
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_battle_challenge_note::get(userid_t userid, uint32_t rival, const char* col, uint32_t *value)
{
	sprintf(this->sqlstr, "select %s from %s where userid = %u and rival = %u",
			col,
			this->get_table_name(userid),
			userid,
			rival
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*value);
	STD_QUERY_ONE_END();
}

int Cuser_battle_challenge_note::get_two_col(userid_t userid, uint32_t rival, const char* col_1,
	   const char* col_2, uint32_t *value_1, uint32_t *value_2)
{
	sprintf(this->sqlstr, "select %s, %s from %s where userid = %u and rival = %u",
			col_1,
			col_2,
			this->get_table_name(userid),
			userid,
			rival
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*value_1);
		INT_CPY_NEXT_FIELD (*value_2);
	STD_QUERY_ONE_END();
}

int Cuser_battle_challenge_note::get_info(userid_t userid, uint32_t rival, uint32_t *cnt, 
		uint32_t *threshold, uint32_t *date)
{
	sprintf(this->sqlstr, "select count, threshold, date from %s where userid = %u and rival = %u",
			this->get_table_name(userid),
			userid,
			rival
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*cnt);
		INT_CPY_NEXT_FIELD (*threshold);
		INT_CPY_NEXT_FIELD (*date);
	STD_QUERY_ONE_END();
}

int Cuser_battle_challenge_note::update_challenge_times(userid_t userid, uint32_t type)
{
	char *col = (char*)"";
    char *p_tmp = (char*)"";

    if(type == LT_BOSS){
        col = (char*)"count";
    }
    else if(type == LT_BOSS_VIP){
        col = (char*)"vip_cnt";
    }
    else if(type == LT_BOSS_TIME){
        col = (char*)"time_cnt";
    }
	else if(type == LT_PK){
		col = (char*)"pvp_cnt";
	}
	else if(type == LT_BOSS_ACTIVE){
		col = (char*)"active_cnt";
	}
    if(strcmp(col, p_tmp) != 0){
		int rets = update_inc(userid, 2, col, 1);
		if(rets == USER_ID_NOFIND_ERR){
			if(type == LT_BOSS){
				insert(userid, 2, 1, 10, 0, 0, 0, 0);
			}
			else if( type == LT_BOSS_VIP){
				insert(userid, 2, 0, 10, 1, 0, 0, 0);
			}
			else if( type == LT_BOSS_TIME){
				insert(userid, 2, 0, 10, 0, 1, 0, 0);
			}
			else if(type == LT_PK){
				insert(userid, 2, 0, 10, 0, 0, 1, 0);
			}
			else if(type == LT_BOSS_ACTIVE){
				insert(userid, 2, 0, 10, 0, 0, 0, 1);
			}
		}
	}//if

	return 0;
}

int Cuser_battle_challenge_note::check_challenge_boss(userid_t userid, uint32_t type, uint32_t *ret)
{
	char *col = (char*)"";
	char *p_tmp = (char*)"";

	if(type == LT_BOSS){
		col = (char*)"count";
	}
	else if(type == LT_BOSS_VIP){
		col = (char*)"vip_cnt";
	}
	else if(type == LT_BOSS_TIME){
		col = (char*)"time_cnt";
	}
	else if(type == LT_PK){
		col = (char*)"pvp_cnt";
	}
	else if(type == LT_BOSS_ACTIVE){
		col = (char*)"active_cnt";
	}
	if(strcmp(col, p_tmp) != 0){
		uint32_t cnt = 0, date = 0;
		int rets = get_two_col(userid, 2, col, "date", &cnt, &date);
		if(rets == USER_ID_NOFIND_ERR){
			*ret = 1;
		}
		else{
			uint32_t today = get_date(time(0));
			if(date == today){
				if(type != LT_PK){
					if(cnt >= 10){
						*ret = 0;
					}
					else{
						*ret = 1;
					}
				}
				else{//PVP
					if(cnt > 10){
						*ret = 0;
					}
					else{
						*ret = 1;
					}
				}
				
			}
			else{
				clear_challenge_cnt(userid, 2, today);
				*ret = 1;
			}
		}//else
	}
	return 0;
}
