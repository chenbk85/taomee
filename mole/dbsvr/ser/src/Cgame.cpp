/*
 * =====================================================================================
 *
 *       Filename:  Cgame.cpp
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
#include "Cgame.h"

/*
 * @brief 构造函数，一个库100个表
 */
Cgame::Cgame(mysql_interface *db) : CtableRoute100(db, "MOLE_GAME_DB", "t_game", "gameid")
{

}

int Cgame::insert(stru_game_score *p_in )
{
	sprintf(this->sqlstr, "insert into %s values(%u,%u)",
							this->get_table_name(p_in->gameid),
							p_in->userid,
							p_in->score);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}

int Cgame::get(uint32_t gameid,uint32_t userid , uint32_t *p_score){
	int ret;
	ret=this->get_db(gameid,userid,p_score );
	if(ret==USER_ID_NOFIND_ERR){
		*p_score=0;	
		ret=SUCC;
	}
	return ret;	

}
int Cgame::get_db(uint32_t gameid,uint32_t userid , uint32_t *p_score)
{
	sprintf( this->sqlstr, "select  score  from %s where userid=%u ", 
		 this->get_table_name(gameid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_score );
	STD_QUERY_ONE_END();

}

int Cgame::set(stru_game_score *p_in )
{
	uint32_t db_score = 0;
	int ret;
	ret=this->get(p_in->gameid, p_in->userid, &db_score);
	if (ret!=SUCC) return ret;

	if (db_score>p_in->score){
		return USER_GAME_NOT_MAX_ERR ;
	}
		
	return this->update(p_in);
}

int Cgame::update(stru_game_score *p_in ){
	int ret;	
	ret=this->update_db( p_in);
	if (ret!=SUCC){
		return this->insert(p_in);
	}
	return SUCC;
}
int Cgame::update_db(stru_game_score *p_in )
{

	sprintf(this->sqlstr, "update %s set score=%u where userid=%u ",
							this->get_table_name(p_in->gameid),
							p_in->score,
							p_in->userid
							);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}

int  Cgame:: get_list_by_idlist(uint32_t gameid  , 
		stru_idlist * p_attireidlist, uint32_t *count, game_user_get_list_out_item ** pp_list   )	
{
	char formatstr[6000]={};
	char valuestr[100]={};
	if (p_attireidlist->count>200 || p_attireidlist->count<1 ) return VALUE_OUT_OF_RANGE_ERR;  
	for(uint32_t i=0; i<p_attireidlist->count;i++) {
		sprintf (valuestr,"%d,",p_attireidlist->item[i] );
		strcat(formatstr,valuestr );
	}
	//去掉最后一个","
	formatstr[strlen(formatstr)-1]='\0';

	sprintf( this->sqlstr,  "select userid,score from %s \
		 	where userid in (%s) order by score desc limit 10" , 
		this->get_table_name( gameid),formatstr ); 
		
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, count ) ;
		INT_CPY_NEXT_FIELD(	(*pp_list+i)->id); 
		INT_CPY_NEXT_FIELD((*pp_list+i)->score); 
	STD_QUERY_WHILE_END();
}

