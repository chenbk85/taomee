/*
 * =====================================================================================
 *
 *       Filename:  CtableString.cpp
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
#include "CtableString.h"



CtableString::CtableString(mysql_interface * db,const  char * db_name_pre,  
	const 	char * table_name_pre) 
	:Ctable(db,"","")	
{ 
	strncpy (this->table_name_pre,table_name_pre,sizeof(this->table_name_pre ) );
    strncpy(this->db_name_pre, db_name_pre,sizeof(this->db_name_pre));
}

char * CtableString::get_table_name(const char *  str  )
{
	uint32_t id=hash_str(str);

	sprintf (this->db_name,"%s_%01d", this->db_name_pre,id%10);
	this->db->select_db(this->db_name);

	sprintf (this->db_table_name,"%s_%01d.%s_%01d",
	this->db_name_pre,id%10, this->table_name_pre,(id/10)%10);
	return this->db_table_name;
}
