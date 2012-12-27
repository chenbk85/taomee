/*
 * =====================================================================================
 *
 *       Filename:  Cuser_game_pk.cpp
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
#include "Cuser_game_pk.h"

/*
 * @brief 构造函数，一个库100个表
 */
Cuser_game_pk::Cuser_game_pk(mysql_interface *db) : CtableRoute100(db, "MOLE_GAME_DB", "t_user_game_pk", "userid")
{

}
int Cuser_game_pk::insert(userid_t userid,stru_game_pk *p_in )
{
	sprintf(this->sqlstr, "insert into %s values(%u,%u,%u)",
							this->get_table_name(userid),
							userid,
							p_in->win_count,
							p_in->lose_count);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}

int Cuser_game_pk::add_win_lose(userid_t userid,bool win_flag )
{
	int ret;	
	ret=this->update_db( userid,win_flag );
	if (ret!=SUCC){
		stru_game_pk item={ };
		if(win_flag){
			item.win_count=1;
		}else{
			item.lose_count=1;
		}
		return this->insert(userid,&item);
	}
	return SUCC;
}

int Cuser_game_pk::update_db(userid_t userid,bool win_flag )
{

	if (win_flag ){
		sprintf(this->sqlstr, "update %s set win_count=win_count+1 where userid=%u",
							this->get_table_name(userid), userid);
	}else{
		sprintf(this->sqlstr, "update %s set lose_count=lose_count+1 where userid=%u",
							this->get_table_name(userid),userid);
	}
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}

int Cuser_game_pk::get_db(userid_t userid, stru_game_pk *p_out)
{
	sprintf( this->sqlstr, "select  win_count,lose_count from %s where userid=%u ", 
		 this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->win_count);
		INT_CPY_NEXT_FIELD(p_out->lose_count);
	STD_QUERY_ONE_END();

}



int Cuser_game_pk::get(userid_t userid ,stru_game_pk *p_out)
{
	int ret;
	ret=this->get_db(userid,p_out);
	if(ret==USER_ID_NOFIND_ERR){
		p_out->lose_count=0;
		p_out->win_count=0;
		ret=SUCC;
	}
	return ret;	

}
