/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_type_buf.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/12/2010 10:41:18 AM
 *       Revision:  none
 *       Compiler:  gcc -Wall -Wextra
 *
 *         Author:  tommychen
 *        Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#include "Csysarg_type_buf.h"

Csysarg_type_buf::Csysarg_type_buf(mysql_interface * db )
	:Ctable( db, "SYSARG_DB","t_sysarg_type_buf")
{ 
}

int Csysarg_type_buf::insert_type_buf(sysarg_set_type_buf_in* p_in)
{
	char mysql_type_buf[mysql_str_len(p_in->length)];
	set_mysql_string(mysql_type_buf, (char *)p_in->type_buf, p_in->length);
	sprintf(this->sqlstr, "insert into %s values(%u, '%s')",
		this->get_table_name(),
		p_in->type,
		mysql_type_buf
	   	);
	STD_SET_RETURN_EX (this->sqlstr, DB_ERR);
}
  

int Csysarg_type_buf::get_type_buf(uint32_t type, sysarg_get_type_buf_out* p_out)
{
	sprintf( this->sqlstr, "select  type_buf \
			from %s where type = %u",
			this->get_table_name(),
			type);
	STD_QUERY_ONE_BEGIN(this->sqlstr, TYPE_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD (p_out->type_buf, sizeof(p_out->type_buf) );
	STD_QUERY_ONE_END();
}


int Csysarg_type_buf::update_type_buf(sysarg_set_type_buf_in* p_in) 
{
	char mysql_type_buf[mysql_str_len(p_in->length)];
	set_mysql_string(mysql_type_buf, (char *)p_in->type_buf, p_in->length);
	sprintf( this->sqlstr, " update %s set type_buf ='%s' \
		where  type = %u " ,
		this->get_table_name(), 
		mysql_type_buf, 
	    p_in->type);
	STD_SET_RETURN_EX(this->sqlstr, TYPE_NOFIND_ERR );	
}


