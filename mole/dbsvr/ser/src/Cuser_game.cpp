/*
 * =====================================================================================
 *
 *       Filename:  Cuser_game.cpp
 *
 *    Description: 把密码修改信息存入数据库 
 *
 *        Version:  1.0
 *        Created:  04/08/2009 12:03:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include "Cuser_game.h"

uint32_t game_thread_limit[] = {500,   0,     0,     0,     0,     0,     0,     1000,  600,   0,
								0,     0,     0,     3000,  0,     0,     0,     15000, 800,   300,
								0,     0,     0,     1000,  900,   250,   200,   0,     0,     0,
								0,     0,     0,     0,     300,   2000,  0,     0,     0,     0,
								2400,  0,     0,     0,     0,     600,   150,   3000,  100,   0,
								0,     0,     0,     0,     100,   300,   0,     0,     100

};

#define GAME_NUM sizeof(game_thread_limit) / sizeof(game_thread_limit[0])




/*
 * @brief 构造函数，一个库100个表
 */
Cuser_game::Cuser_game(mysql_interface *db) : CtableRoute100(db, "MOLE_GAME_DB", "t_user_game", "userid")
{

}
int Cuser_game::insert(stru_game_score *p_in,uint32_t pk_flag  )
{
	sprintf(this->sqlstr, "insert into %s values(%u,%u,%u,%u)",
							this->get_table_name(p_in->userid),
							p_in->userid,
							p_in->gameid,
							p_in->score,pk_flag);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}
int Cuser_game::get_db(uint32_t gameid,uint32_t userid , uint32_t *p_score)
{
	sprintf( this->sqlstr, "select  score  from %s where userid=%u and gameid=%u ", 
		 this->get_table_name(userid),userid,gameid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_score );
	STD_QUERY_ONE_END();

}



int Cuser_game::get(uint32_t gameid,uint32_t userid , uint32_t *p_score)
{
	int ret;
	ret=this->get_db(gameid,userid,p_score );
	if(ret==USER_ID_NOFIND_ERR){
		*p_score=0;	
		ret=SUCC;
	}
	return ret;	

}

int Cuser_game::set(stru_game_score *p_in )
{
	uint32_t db_score = 0;
	int ret;
	ret=this->get(p_in->gameid,p_in->userid,&db_score);
	if (ret!=SUCC) return ret;

	if (db_score > p_in->score){
		return USER_GAME_NOT_MAX_ERR ;
	}
	return this->update(p_in );		
}

int Cuser_game::update(stru_game_score *p_in ){
	int ret;	
	ret=this->update_db( p_in);
	if (ret!=SUCC){
		return this->insert(p_in,0);
	}
	return SUCC;
}
int Cuser_game::update_db(stru_game_score *p_in )
{

	sprintf(this->sqlstr, "update %s set score=%u where userid=%u and gameid=%u ",
							this->get_table_name(p_in->userid),
							p_in->score,
							p_in->userid,
							p_in->gameid
							);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}

int Cuser_game::get_list(userid_t userid,  
		uint32_t *p_count, game_get_list_out_item **pp_out_item )
{
	sprintf( this->sqlstr, "select gameid, score, pkflag from %s \
			where userid=%u  ", 
			this->get_table_name(userid),userid); 
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_out_item, p_count ) ;
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->gameid); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->score); 
		INT_CPY_NEXT_FIELD((*pp_out_item+i)->pkflag); 
	STD_QUERY_WHILE_END();
}

int Cuser_game::set_pkflag_db(uint32_t gameid,uint32_t userid,uint32_t pk_flag  )
{
	sprintf(this->sqlstr, "update %s set pkflag=pkflag|%u where userid=%u and gameid=%u ",
							this->get_table_name(userid),
							pk_flag,
							userid,
							gameid
							);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}

int Cuser_game::set_pkflag(uint32_t gameid,uint32_t userid,uint32_t pk_flag  )
{
	int ret;
	ret=this->set_pkflag_db(gameid,userid,pk_flag );
	if(ret!=SUCC){
		stru_game_score item={ };
		return this->insert(&item,pk_flag);
	}

	return ret;
}

int Cuser_game::get_game_thread(uint32_t gameid, uint32_t *p_game_limit)
{
	uint32_t temp_id = gameid - 1;
	if (temp_id > GAME_NUM)	{
		return VALUE_OUT_OF_RANGE_ERR;
	}
	*p_game_limit = game_thread_limit[temp_id];
	return SUCC;
}
