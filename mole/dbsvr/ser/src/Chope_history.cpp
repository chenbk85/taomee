/*
 * =====================================================================================
 *
 *       Filename:  Chope_history.cpp
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
#include "Chope_history.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
//create
Chope_history::Chope_history(mysql_interface * db ) :Ctable( db, "HOPE_DB","t_hope_history" )
{ 
}

int Chope_history::get_hope(userid_t send_id,  userid_t recv_id , uint32_t hopedate,
	  	hope_set_hope_used_out * p_out )
{
	sprintf( this->sqlstr, "select    recv_id , recv_type  from %s\
		   	where send_id=%u and  recv_id=%u and hopedate=%u  and useflag=0 ", 
			this->get_table_name(), send_id, recv_id, hopedate  );
	STD_QUERY_ONE_BEGIN( this->sqlstr,HOPE_NOFIND_ERR  ) ;
		p_out->recv_id=atoi_safe(NEXT_FIELD); 
		p_out->recv_type=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

int Chope_history::get_hope_list( userid_t recv_id , 		
		uint32_t *p_count, hope_get_hope_list_out_item ** pp_list   )	
{
	uint32_t now=time(NULL);
	uint32_t today;
	today=get_date(now);

	sprintf( this->sqlstr, "select hopedate   ,  \
		 	send_id,send_nick, recv_type  from %s \
			where recv_id=%u and useflag=0 and \
			(recv_type>0 and hopedate<%u) ", 
			this->get_table_name(), recv_id ,today);
	STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_list, p_count ) ;
		INT_CPY_NEXT_FIELD( (*pp_list+i)->hopedate);
		INT_CPY_NEXT_FIELD((*pp_list+i)->send_id );
		BIN_CPY_NEXT_FIELD((*pp_list+i)->send_nick, NICK_LEN);		
		INT_CPY_NEXT_FIELD((*pp_list+i)->recv_type);
	STD_QUERY_WHILE_END();
}

int Chope_history::get_hope_list_ex( userid_t recv_id , 		
		uint32_t *p_count, hope_get_hope_all_out_item ** pp_list   )	
{
	sprintf( this->sqlstr, "select hopedate   ,  \
		 	send_id,send_nick, recv_type,recv_type_name, useflag  from %s \
			where recv_id=%u limit 0,64 ", 
			this->get_table_name(), recv_id );
	STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_list, p_count ) ;
		INT_CPY_NEXT_FIELD( (*pp_list+i)->hopedate);
		INT_CPY_NEXT_FIELD((*pp_list+i)->send_id );
		BIN_CPY_NEXT_FIELD((*pp_list+i)->send_nick, NICK_LEN);		
		INT_CPY_NEXT_FIELD((*pp_list+i)->recv_type);
		BIN_CPY_NEXT_FIELD((*pp_list+i)->recv_type_name,
			   	sizeof((*pp_list+i)->recv_type_name));	
		INT_CPY_NEXT_FIELD((*pp_list+i)->useflag);
	STD_QUERY_WHILE_END();
}



int Chope_history::get_hope_list_by_date(uint32_t hopedate,uint32_t index  ,
		uint32_t *p_count, hope_get_hope_by_date_out_item ** pp_list   )	
{
	sprintf( this->sqlstr, "select recv_id, send_id,send_nick, recv_type,recv_type_name  \
		   	from %s where hopedate=%u and useflag=0 limit %u ,50", 
			this->get_table_name(),  hopedate, index*50);
	DEBUG_LOG("sql:%s",this->sqlstr);
	STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_list, p_count ) ;
		INT_CPY_NEXT_FIELD((*pp_list+i)->recv_id);
		INT_CPY_NEXT_FIELD((*pp_list+i)->send_id);
		BIN_CPY_NEXT_FIELD((*pp_list+i)->send_nick, NICK_LEN);		
		INT_CPY_NEXT_FIELD((*pp_list+i)->recv_type);
		BIN_CPY_NEXT_FIELD((*pp_list+i)->recv_type_name, sizeof((*pp_list+i)->recv_type_name));		
	STD_QUERY_WHILE_END();
}

int Chope_history::set_ex(userid_t send_id,  userid_t recv_id , uint32_t old_hopedate,
	   	uint32_t new_hopedate, uint32_t recv_type )
{
	sprintf( this->sqlstr, " update %s set  hopedate=%u , recv_type=%u \
		where send_id=%u  and recv_id=%u and hopedate=%u " ,
		this->get_table_name(),new_hopedate,recv_type ,  send_id, recv_id , old_hopedate );
	STD_SET_RETURN_EX(this->sqlstr,HOPE_NOFIND_ERR );	
}


int Chope_history::set_useflag(userid_t send_id, userid_t recv_id,
		uint32_t hope_date )
{
	sprintf( this->sqlstr, " update %s set  useflag=1 \
		where send_id=%u  and recv_id=%u and hopedate=%u " ,
		this->get_table_name(), send_id, recv_id , hope_date );
	STD_SET_RETURN_EX(this->sqlstr,HOPE_NOFIND_ERR );	
}


int Chope_history::check_today_hope(userid_t send_id )
{
    sprintf( this->sqlstr, "select send_id from %s where hopedate=%u  and  send_id=%u ",
            this->get_table_name(), get_date(time(NULL)) ,send_id);
    STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
    STD_QUERY_ONE_END();
}

int Chope_history::insert( hope_add_hope_in *p_in  )
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	char recv_type_name_mysql [mysql_str_len(sizeof(p_in->recv_type_name) ) ];
	set_mysql_string(nick_mysql,p_in->send_nick,NICK_LEN);
	set_mysql_string(recv_type_name_mysql ,p_in->recv_type_name,sizeof(p_in->recv_type_name));

	sprintf( this->sqlstr, "insert into %s values(%u,%u,'%s',%u,%u,'%s',0 )",
	this->get_table_name(),get_date(time(NULL)),p_in->send_id, nick_mysql,
	p_in->recv_id,p_in->recv_type,recv_type_name_mysql );
	STD_INSERT_RETURN(this->sqlstr, HOPE_IS_EXISTED_ERR );	
}

