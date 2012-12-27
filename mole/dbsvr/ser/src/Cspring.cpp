/*
 * =====================================================================================
 *
 *       Filename:  Ccup.cpp
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
#include "Cspring.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
//#define INIT_SHM_FLAG  -100 
//create
Cspring::Cspring (mysql_interface * db ) :CtableRoute100( db, "SPRING_DB","t_spring","userid" )
{ 

}

int Cspring::get_value_db(userid_t userid, spring_stru * p_spring  )
{
	sprintf( this->sqlstr, "select flag,gold,gold_logdate,gold_count_today,\
			silver,silver_logdate,silver_count_today,msg\
			from %s where userid=%u ", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_spring->flag);
		INT_CPY_NEXT_FIELD(p_spring->gold);
		INT_CPY_NEXT_FIELD(p_spring->gold_logdate);
		INT_CPY_NEXT_FIELD(p_spring->gold_count_today);
		INT_CPY_NEXT_FIELD(p_spring->silver);
		INT_CPY_NEXT_FIELD(p_spring->silver_logdate);
		INT_CPY_NEXT_FIELD(p_spring->silver_count_today);
		BIN_CPY_NEXT_FIELD(p_spring->spring_msg,sizeof(p_spring->spring_msg));
	STD_QUERY_ONE_END();
}

int Cspring::update_count(userid_t userid, spring_stru *p_in )
{
	sprintf( this->sqlstr, " update %s set \
		gold='%u',\
		gold_logdate='%u',\
		gold_count_today='%u',\
		silver='%u',\
		silver_logdate='%u',\
		silver_count_today='%u'\
		where userid=%u " ,
		this->get_table_name(userid), 
		p_in->gold,
		p_in->gold_logdate,
		p_in->gold_count_today,
		p_in->silver,
		p_in->silver_logdate,
		p_in->silver_count_today,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cspring::change_count(userid_t userid, spring_change_value_in * p_in,
	  spring_change_value_out *p_out	)
{
	spring_stru spring;
	int ret;
	ret=this->get_value(userid,&spring);
	if(ret!=SUCC) return ret;
	if (spring.flag==0) 
		return SPRING_GOLD_SILVER_NOT_INIT_ERR;

	uint32_t now=get_date(time(NULL));
	//检查金牌		
	if (p_in->change_gold>0){//add
		ret=day_add_do_count(&(spring.gold_logdate),&(spring.gold_count_today),
			 &(spring.gold), now, p_in->change_gold, 10);
		if (ret!=SUCC){
			return SPRING_GOLD_MAX_A_DAY_ERR; 
		}
		
	}else if (p_in->change_gold<0) {
		if ((int)spring.gold+p_in->change_gold<0){
			return  SPRING_GOLD_NOENOUGH_ERR;
		}
		spring.gold=(int)spring.gold+p_in->change_gold;
	}		

	//检查银牌
	if (p_in->change_silver>0){//add
		ret=day_add_do_count(&(spring.silver_logdate),&(spring.silver_count_today),
			 &(spring.silver),now, p_in->change_silver, 20);
		if (ret!=SUCC){
			return SPRING_SILVER_MAX_A_DAY_ERR; 
		}
		
	}else if (p_in->change_silver<0) {
		if ((int)spring.silver+p_in->change_silver<0){
			return  SPRING_SILVER_NOENOUGH_ERR;
		}
		spring.silver=(int)spring.silver+p_in->change_silver;
	}		
	p_out->gold=spring.gold;
	p_out->silver=spring.gold;
	return this->update_count(userid, &spring);
}


int Cspring::get_value(userid_t userid, spring_stru * p_spring )
{
	int ret;
	ret=this->get_value_db(userid,p_spring );
	if (ret!=SUCC){
		memset(p_spring,0,sizeof(*p_spring) );
		return this->insert(userid,p_spring);
	}
	return SUCC;
}

int Cspring::get_value_ex(userid_t userid,spring_info* p_out)
{
	spring_stru spring;
	int ret;
	ret=this->get_value(userid,&spring);
	if(ret!=SUCC) return ret;
	p_out->flag=spring.flag;
	p_out->gold=spring.gold;
	p_out->silver=spring.silver;
	memcpy( p_out->spring_msg,spring.spring_msg,sizeof(p_out->spring_msg));
	return SUCC;
}



int Cspring::insert(userid_t userid, spring_stru * p_spring )
{
	char mysql_msg[mysql_str_len(sizeof(p_spring->spring_msg))];
	set_mysql_string(mysql_msg, p_spring->spring_msg ,sizeof(p_spring->spring_msg));

	sprintf( this->sqlstr, "insert into %s values (%u,%u, %u,%u,%u, %u,%u,%u,'%s' )",
	this->get_table_name(userid),userid,
		p_spring->flag,
		p_spring->gold,
		p_spring->gold_logdate,
		p_spring->gold_count_today,
		p_spring->silver,
		p_spring->silver_logdate,
		p_spring->silver_count_today,
		mysql_msg
		);
	STD_INSERT_RETURN(this->sqlstr, USER_ID_EXISTED_ERR);	
}

int Cspring::update_msg_db(userid_t userid, spring_set_msg_in *p_in )
{
	char mysql_msg[mysql_str_len(sizeof(p_in->spring_msg))];
	set_mysql_string(mysql_msg, p_in->spring_msg ,sizeof(p_in->spring_msg));

	sprintf( this->sqlstr, " update %s set \
		msg='%s'\
		where userid=%u " ,
		this->get_table_name(userid), 
		mysql_msg,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}
int Cspring::update_flag(userid_t userid, uint32_t flag )
{
	sprintf( this->sqlstr, " update %s set \
		flag=%u\
		where userid=%u " ,
		this->get_table_name(userid),flag,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Cspring::update_msg(userid_t userid, spring_set_msg_in *p_in )
{
	int ret;
	ret=this->update_msg_db(userid, p_in );
	if (ret!=SUCC){ 
		spring_stru spring={};
		memcpy(spring.spring_msg,
				p_in->spring_msg ,
				sizeof(spring.spring_msg ));
		ret=this->insert( userid, &spring );
	}
	return ret;
}

int Cspring::set_init(userid_t userid)
{
	spring_stru spring;
	int ret;
	ret=this->get_value(userid,&spring);
	if(ret!=SUCC) return ret;
	if (spring.flag!=0) 
		return SPRING_GOLD_SILVER_INITED_ERR;
	return this->update_flag(userid,1);
}

