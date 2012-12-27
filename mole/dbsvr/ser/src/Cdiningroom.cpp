/*
 * =====================================================================================
 *
 *       Filename: 	Cdiningroom.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年03月30日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  tommychen@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include "Cdiningroom.h"
#include "common.h"
#include "proto.h"
#include "benchapi.h"

Cdiningroom::Cdiningroom(mysql_interface * db ):Ctable(db, "DININGROOM","t_diningroom" )
{ 
}

int Cdiningroom::insert(userid_t userid, diningroom_user_create_diningroom_in *p_in, diningroom_user_create_diningroom_out& out)
{
	char room_name_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(room_name_mysql,p_in->room_name,NICK_LEN);
	sprintf( this->sqlstr, "insert into %s (roomid,userid,room_name,style_id,type_id) values(NULL,%u,'%s',%u,%u)",
		this->get_table_name(),
		userid,
   		room_name_mysql,
		p_in->style_id,
		p_in->type_id );
	STD_INSERT_GET_ID(this->sqlstr, DB_ERR, out.roomid);	
}


int Cdiningroom::set_room_name(diningroom_user_set_room_name_in *p_in)
{
	char room_name_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(room_name_mysql,p_in->room_name,NICK_LEN);
	sprintf( this->sqlstr, "update %s set room_name = '%s' where roomid = %u",
		this->get_table_name(),
   		room_name_mysql,
		p_in->roomid );
	STD_SET_RETURN_EX(this->sqlstr, ROOMID_NOFIND_ERR);	
}


int Cdiningroom::set_room_style(diningroom_user_set_room_style_in *p_in)
{
	sprintf( this->sqlstr, "update %s set style_id = %u where roomid = %u",
		this->get_table_name(),
   		p_in->style_id,
		p_in->roomid );
	STD_SET_RETURN_EX(this->sqlstr, ROOMID_NOFIND_ERR);	
}



int Cdiningroom::get_frame_diningrooms(uint32_t frameid , 		
		uint32_t *p_count, diningroom_user_get_frame_diningroom_out_item** pp_list)	
{
	uint32_t roomid_end = frameid*4;
	uint32_t roomid_begin = roomid_end - 3;
	sprintf( this->sqlstr, "select roomid, userid, room_name, style_id, type_id from %s \
			where roomid >= %u and roomid <= %u", 
			this->get_table_name(), roomid_begin, roomid_end);
	STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_list, p_count ) ;
		INT_CPY_NEXT_FIELD((*pp_list+i)->roomid);
		INT_CPY_NEXT_FIELD((*pp_list+i)->userid);
		BIN_CPY_NEXT_FIELD((*pp_list+i)->room_name, NICK_LEN);			
		INT_CPY_NEXT_FIELD((*pp_list+i)->style_id);
		INT_CPY_NEXT_FIELD((*pp_list+i)->type_id);
	STD_QUERY_WHILE_END();
}

int Cdiningroom::get_diningroom_count(uint32_t& count)	
{
	sprintf( this->sqlstr, "select max(roomid) as count  from %s where type_id = 31 ", 
			this->get_table_name());
	STD_QUERY_ONE_BEGIN( this->sqlstr, DB_ERR) ;
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}

int Cdiningroom::get_diningroom_min_frame(uint32_t& frame_id)	
{
	sprintf( this->sqlstr, "select min(roomid) as count  from %s where type_id = 31 ", 
			this->get_table_name());
	STD_QUERY_ONE_BEGIN( this->sqlstr, DB_ERR) ;
		INT_CPY_NEXT_FIELD(frame_id);
	STD_QUERY_ONE_END();
}

int Cdiningroom::get_diningroom_roomid(userid_t userid,  uint32_t& roomid)	
{
	sprintf( this->sqlstr, "select roomid  from %s where userid = %u ", 
			this->get_table_name(),
			userid);
	STD_QUERY_ONE_BEGIN( this->sqlstr, DB_ERR) ;
		INT_CPY_NEXT_FIELD(roomid);
	STD_QUERY_ONE_END();
}


