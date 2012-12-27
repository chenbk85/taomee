/*
 * =====================================================================================
 *
 *       Filename:  Cqa.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年02月09日 11时36分17秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include "Cqa.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"
//create
Cqa::Cqa(mysql_interface * db ) :Ctable( db, "MSGBOARD_DB","t_qa" )
{ 

}


int Cqa::get_count(uint32_t *p_count){
	sprintf( this->sqlstr, "select count(1) from %s  ", 
			this->get_table_name());
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr,DB_ERR);
			INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}
int Cqa::insert( qa_msg_item *p_item )
{
	char q_mysql[mysql_str_len(sizeof (p_item->question))];
	char a_mysql[mysql_str_len(sizeof (p_item->answer))];
	p_item->answer[sizeof(p_item->answer)-1]='\0';
	p_item->question[sizeof(p_item->question)-1]='\0';
	uint32_t msgid;

	set_mysql_string(q_mysql ,(p_item->question),strlen(p_item->question)); 
	set_mysql_string(a_mysql ,(p_item->answer),strlen(p_item->answer)); 
	
	sprintf( this->sqlstr, "insert into %s values (0,'%s','%s' )", 
			this->get_table_name(), 
			q_mysql,
			a_mysql
	);

	STD_INSERT_GET_ID (this->sqlstr, KEY_EXISTED_ERR ,msgid );	
}


int Cqa::getmsglist( uint32_t start, uint32_t count, 
				uint32_t *p_count, qa_msg_item ** pp_list  )
{

	sprintf( this->sqlstr,"select   question,answer  from %s \
		 order by msgid desc limit %u,%u" ,
		this->get_table_name(), start,count ); 
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, p_count ) ;
		BIN_CPY_NEXT_FIELD((*pp_list+i)->question,sizeof((*pp_list+i)->question));
		BIN_CPY_NEXT_FIELD((*pp_list+i)->answer,sizeof((*pp_list+i)->answer));
	STD_QUERY_WHILE_END();

}

int Cqa::getmsglist_ex( uint32_t start, uint32_t count, 
				uint32_t *p_count, msgboard_qa_get_list_ex_out_item ** pp_list  )
{

	sprintf( this->sqlstr,"select  msgid, question,answer  from %s \
		 order by msgid desc limit %u,%u" ,
		this->get_table_name(), start,count ); 
	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_list, p_count ) ;
		INT_CPY_NEXT_FIELD( (*pp_list+i)->msgid);
		BIN_CPY_NEXT_FIELD((*pp_list+i)->question,sizeof((*pp_list+i)->question));
		BIN_CPY_NEXT_FIELD((*pp_list+i)->answer,sizeof((*pp_list+i)->answer));
	STD_QUERY_WHILE_END();

}

int Cqa::del(uint32_t msgid )
{
	sprintf( this->sqlstr, "delete from %s where msgid=%u " ,
			this->get_table_name(),  msgid ); 
	STD_REMOVE_RETURN_EX (this->sqlstr,  KEY_NOFIND_ERR);
}


