/*
 * =====================================================================================
 *
 *       Filename:  CtableDate.cpp
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
#include "CtableDate.h"

#include "common.h"
#include "proto.h"
#include "benchapi.h"


CtableDate::CtableDate(mysql_interface * db,const char * db_name_pre,  const char* table_name_pre, const char* id_name )
	:CtableRoute( db,db_name_pre,table_name_pre,id_name)
{ 
}

char*  CtableDate::get_table_name(time_t logtime )
{
	this->db->select_db(this->db_name_pre);
	sprintf(this->db_table_name,"%s.%s_%8.8d",
		this->db_name_pre,
		this->table_name_pre,
		get_date(logtime)
	);

	return this->db_table_name;
}

