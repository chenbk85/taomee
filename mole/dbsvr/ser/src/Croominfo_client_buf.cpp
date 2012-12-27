/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_client_buf.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/12/2010 10:41:18 AM
 *       Revision:  none
 *       Compiler:  gcc -Wall -Wextra Croominfo_client_buf.cpp
 *
 *         Author:  tommychen
 *        Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#include "Croominfo_client_buf.h"

Croominfo_client_buf::Croominfo_client_buf(mysql_interface * db )
	:CtableRoute10x10( db, "ROOMINFO","t_roominfo_client_buf","userid")
{ 
}

int Croominfo_client_buf::insert_client_buf(userid_t userid, roominfo_set_client_buf_in* p_in)
{
	char mysql_client_buf[mysql_str_len(p_in->length)];
	set_mysql_string(mysql_client_buf, (char *)p_in->client_buf, p_in->length);
	sprintf(this->sqlstr, "insert into %s values(%u, %u, '%s')",
		this->get_table_name(userid),
		userid,
		p_in->type_id,
		mysql_client_buf
	   	);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}
  

int Croominfo_client_buf::get_client_buf(userid_t userid, uint32_t type_id,  roominfo_get_client_buf_out* p_out)
{
	sprintf( this->sqlstr, "select  client_buf \
			from %s where userid=%u and typeid = %u", 
			this->get_table_name(userid),
			userid,
			type_id);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD (p_out->client_buf, sizeof(p_out->client_buf) );
	STD_QUERY_ONE_END();
}


int Croominfo_client_buf::update_client_buf( userid_t userid, roominfo_set_client_buf_in* p_in) 
{
	char mysql_client_buf[mysql_str_len(p_in->length)];
	set_mysql_string(mysql_client_buf, (char *)p_in->client_buf, p_in->length);
	sprintf( this->sqlstr, " update %s set client_buf ='%s' \
		where  userid=%u and typeid = %u " ,
		this->get_table_name(userid), 
		mysql_client_buf, 
		userid,
	    p_in->type_id);
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}


