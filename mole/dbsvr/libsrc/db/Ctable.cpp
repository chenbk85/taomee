/*
 * =====================================================================================
 *
 *       Filename:  Ctable.cpp
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
#include "Ctable.h"
#include "common.h"
#include "proto.h"
#include "benchapi.h"



Ctable::Ctable(mysql_interface * db, const char * dbname, const char * tablename ) 
{ 
	this->db=db;
	strcpy (this->table_name,tablename );
	strcpy (this->db_name,dbname );
}

char* Ctable::get_table_name()
{
	this->db->select_db(this->db_name);
	sprintf (this->db_table_name,"%s.%s", this->db_name, this->table_name);
	return this->db_table_name;
}

