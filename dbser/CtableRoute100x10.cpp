/*
 * =====================================================================================
 *
 *       Filename:  CtableRoute100x10.cpp
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
#include "CtableRoute100x10.h"

	// id_name : 用于id_is_existed方法中
CtableRoute100x10::CtableRoute100x10(mysql_interface * db, const char * db_name_pre,  
		 const char * table_name_pre,const char* id_name ,const char* key2_name  )
	: CtableRoute(db, db_name_pre, table_name_pre, id_name, key2_name  )
{

}

char * CtableRoute100x10::get_table_name(uint32_t id)
{

	this->id=id;
	sprintf (this->db_name,"%s_%02d", this->db_name_pre,id%100 );
	this->db->select_db(this->db_name);

	sprintf (this->db_table_name,"%s_%02d.%s_%01d",
	this->db_name_pre,id%100, this->table_name_pre,(id%10000)/1000);
	return this->db_table_name;
}

