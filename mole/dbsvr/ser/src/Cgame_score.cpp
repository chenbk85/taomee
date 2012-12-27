/*
 * =====================================================================================
 *
 *       Filename:  Cgame_score.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include "Cgame_score.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"

//create
Cgame_score::Cgame_score(mysql_interface * db ) 
	:Ctable( db,"GAME_SCORE_DB","t_game_score")
{ 
	
}

int Cgame_score::update_score(uint32_t gameid, userid_t userid, 
		uint32_t score,char * nick )
{	
	uint32_t id; 	
	uint32_t t_score; 	
	int ret; 	
	//查看该用户原有的记录
	ret=this->get_score(gameid,userid,&id, &t_score );		
	if (ret==GAME_SCORE_NOFIND_ERR){ //no find
		//得到该游戏最小积分
		ret=this->get_game_min_score(gameid,&id,&t_score);
		if (ret==DB_SUCC){
			if (t_score<=score ){
				//更新之
				return this->change_user(gameid,id, userid,score,nick );
			}else{
				return  GAME_SCORE_NONEED_SET_ERR; 
			}
		}else{
			return ret;
		}
	}else if (ret==SUCC ){
		if (t_score<score ){
			//更新自己原有的
			return this->change_user(gameid,id,userid,score,nick );
		}else{
			return  GAME_SCORE_NONEED_SET_ERR; 
		}
	}else {
		return ret;
	}
}

int Cgame_score::delete_list(uint32_t gameid)
{	
	sprintf( this->sqlstr, "delete from %s where gameid=%u ", 
			this->get_table_name(),gameid);
	STD_REMOVE_RETURN_EX(this->sqlstr,SUCC);
}


int Cgame_score::get_score(uint32_t gameid, userid_t userid,uint32_t * p_id ,uint32_t *score )
{	
	sprintf( this->sqlstr, 
			"select id, score from %s where gameid=%u and userid=%u ", 
			this->get_table_name(),gameid, userid );
	STD_QUERY_ONE_BEGIN(this-> sqlstr, GAME_SCORE_NOFIND_ERR );
			*p_id=atoi_safe(NEXT_FIELD );
			*score=atoi_safe(NEXT_FIELD );
	STD_QUERY_ONE_END();
}

int Cgame_score::get_game_min_score(uint32_t gameid, 
		uint32_t *id, uint32_t *score )
{
	sprintf( this->sqlstr, "select id ,score from %s \
			where gameid=%u order by score asc LIMIT 0,1", 
			this->get_table_name(),gameid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, GAME_SCORE_NOFIND_ERR );
			*id=atoi_safe(NEXT_FIELD);
			*score=atoi_safe(NEXT_FIELD);
	STD_QUERY_ONE_END();
}

int Cgame_score:: change_score(uint32_t gameid, uint32_t userid, uint32_t score)
{
	sprintf( this->sqlstr, " update %s set score=%u \
		   			where gameid=%u and userid=%u" ,
				this->get_table_name(),  score ,
				 gameid,userid);
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}

int Cgame_score:: change_user(uint32_t gameid, uint32_t oldid,
				userid_t userid, uint32_t score,char * nick )
{
	char mysql_nick[mysql_str_len(NICK_LEN)];
	set_mysql_string(mysql_nick,nick , NICK_LEN);

	sprintf( this->sqlstr, " update %s set \
					userid=%u, score=%u,nick='%s' \
		   			where gameid=%u and id=%u" ,
				this->get_table_name(), userid, score ,mysql_nick,
				 gameid,oldid);
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}


int Cgame_score::get_score_list(uint32_t gameid,uint32_t *p_count, 
		score_list_item ** pp_item ){
	sprintf( this->sqlstr, "select userid,score,nick from %s \
			where gameid=%u  \
			order by score desc ", 
			this->get_table_name(), gameid); 

	STD_QUERY_WHILE_BEGIN(this->sqlstr,pp_item,p_count );
			(*pp_item+i)->userid=atoi_safe(NEXT_FIELD); 
			(*pp_item+i)->score=atoi_safe(NEXT_FIELD); 
			strncpy((*pp_item+i)->nick, NEXT_FIELD ,NICK_LEN);
	STD_QUERY_WHILE_END();
}

int Cgame_score::init_score_list(uint32_t gameid,uint32_t count )
{
	char nick[NICK_LEN];
	memset(nick, 0,NICK_LEN );
	uint32_t i;
	this->delete_list(gameid);	
	for (i=0;i<count;i++){
		this->insert(gameid,i,0,nick );		
	}
	return	SUCC;		
}

int Cgame_score::insert(uint32_t gameid,userid_t userid, 
		uint32_t score ,char * nick )
{	
	char mysql_nick[mysql_str_len(NICK_LEN)];
	set_mysql_string(mysql_nick,nick , NICK_LEN);
	sprintf( this->sqlstr, "insert into %s values (0,%u,%u,%u,'%s' )", 
			this->get_table_name() , gameid, userid,score,mysql_nick );
	STD_INSERT_RETURN	(this->sqlstr,DB_ERR);	
}
