/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_fip_card_info.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/01/2011 02:06:05 PM
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
#include "Cuser_battle_flip_card_info.h"

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
};  
Cuser_battle_flip_card_info::Cuser_battle_flip_card_info(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_battle_flip_card", "userid")
{

}


int Cuser_battle_flip_card_info::get_info(userid_t userid, user_battle_get_flip_info_out_header *p_out,
		flip_cd_bin_t *other)
{
	sprintf(this->sqlstr, "select barrier_id, is_win, level, exp_inc, level_type,items from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	//uint32_t level_type = 0;
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (p_out->barrier_id);
		INT_CPY_NEXT_FIELD (p_out->is_win);
		INT_CPY_NEXT_FIELD (p_out->level);
		INT_CPY_NEXT_FIELD (p_out->exp_inc);
		//INT_CPY_NEXT_FIELD (level_type);
		//if(level_type == LT_GET_APPRENTICE || level_type == LT_GRAB_APPRENTICE){
			//p_out->fight_type = 1;
		//}
		//else if(level_type == LT_TRAIN){
			//p_out->fight_type = 2;
		//}
		//else if(level_type == LT_PK){
			//p_out->fight_type = 3;
		//}
		//else{
			//p_out->fight_type = 0;
		//}
		INT_CPY_NEXT_FIELD (p_out->fight_type);
        BIN_CPY_NEXT_FIELD ((char*)other, sizeof(flip_cd_bin_t) );
		if(p_out->fight_type == LT_GET_APPRENTICE){
			p_out->vs_userid = other->remainder[4];
			p_out->vs_master_id = 0;
		}
		else if(p_out->fight_type == LT_GRAB_APPRENTICE){
			p_out->vs_master_id = other->remainder[4];
			p_out->vs_userid = other->remainder[2];
		}
	STD_QUERY_ONE_END();
}
int Cuser_battle_flip_card_info::get_battle_over_info(userid_t userid, game_over_cal_info_t *out,
	   	flip_cd_bin_t *other)
{
	sprintf(this->sqlstr, "select barrier_id, is_win, exp_inc, comp_degree, level_type, items from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (out->barrier_id);
		INT_CPY_NEXT_FIELD (out->is_win);
		INT_CPY_NEXT_FIELD (out->exp_inc);
		INT_CPY_NEXT_FIELD (out->comp_degree);
		INT_CPY_NEXT_FIELD (out->level_type);
        BIN_CPY_NEXT_FIELD ((char*)other, sizeof(flip_cd_bin_t) );
	STD_QUERY_ONE_END();
}
int Cuser_battle_flip_card_info::insert(userid_t userid, user_battle_game_over_caculate_in *p_in)
{
	uint32_t remainder[5] = {
		p_in->is_energy, p_in->consume_point, p_in->cardid, p_in->keyid, p_in->rival_id
	};
	DEBUG_LOG("****************is_energy: %u, point: %u, card: %u,  key: %u, rival: %u",
			remainder[0], remainder[1], remainder[2], remainder[3], remainder[4]);
	char mysql_ar[mysql_str_len(sizeof(remainder))];
	memset(mysql_ar, 0, sizeof(mysql_ar));
	set_mysql_string(mysql_ar, (char*)remainder, sizeof(remainder));
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u, %u, %u, '%s')",
			this->get_table_name(userid),
			userid,
			p_in->barrierid,
			p_in->is_sucess,
			p_in->rival_level,
			p_in->exp_inc,
			p_in->comp_degree,
			p_in->level_type,
			mysql_ar
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}
int Cuser_battle_flip_card_info::remove(userid_t userid)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}
