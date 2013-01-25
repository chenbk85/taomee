/*
 * =====================================================================================
 *
 *       Filename:  CtableDate_100.cpp
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
#include "CtableDate_100.h"

CtableDate_100::CtableDate_100(mysql_interface * db,const char * db_name_pre,  
	const	char * table_name_pre,const char* id_name ) :CtableRoute(
	db,db_name_pre,table_name_pre,id_name)
{ 

}

char*  CtableDate_100::get_table_name(uint32_t id, time_t logtime )
{
	this->id=id;
	this->db->select_db(this->db_name);


	sprintf (this->db_table_name,"%s.%s_%02d_%8.8d",
		this->db_name_pre,  this->table_name_pre,id%100,get_date(logtime) );
	return this->db_table_name;
}

