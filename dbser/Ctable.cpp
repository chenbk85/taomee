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



Ctable::Ctable(mysql_interface * db, const char * dbname, const char * tablename ) 
{ 
	this->use_cache=true;
	this->db=db;
	strcpy (this->table_name,tablename );
	strcpy (this->db_name,dbname );
}

Ctable::Ctable(mysql_interface * db)
{
	this->db=db;
}


char* Ctable::get_table_name(){

	this->db->select_db(this->db_name);

	sprintf (this->db_table_name,"%s.%s", this->db_name, this->table_name);
	return this->db_table_name;
}

int Ctable::exec_insert_sql_get_auto_increment_id(char * sql_str, int existed_err,
				uint32_t *p_increment_id )
{
 	int dbret;
	int acount; 

	this->db->id=this->id;
	if ((dbret=this->db->exec_update_sql(sql_str,&acount ))==DB_SUCC)
	{
		*p_increment_id=mysql_insert_id(&(this->db->handle));
		return DB_SUCC;
	}else {
		if (dbret==ER_DUP_ENTRY)
			return  existed_err;
		else return DB_ERR;
	}

}

int Ctable::exec_delete_sql(char * sql_str, int nofind_err )
{
	return this->exec_update_sql(sql_str,nofind_err  );
}

int Ctable::exec_update_sql(char * sql_str, int nofind_err )
{
	int acount; 
	int dbret; 
	this->db->id=this->id;
	if ((dbret=this->db->exec_update_sql(sql_str,&acount ))==DB_SUCC)
	{
		if (acount ==1){
			return DB_SUCC;	
		}else{
			return nofind_err; 
		}
	}else {
		return DB_ERR;
	} 

}	

int  Ctable::record_is_existed(char * sql_str,  bool * p_existed)
{
	this->db->id=this->id;
	*p_existed=false;
	STD_QUERY_ONE_BEGIN(sql_str, SUCC);
		*p_existed=true;
	STD_QUERY_ONE_END();
}

int Ctable::exec_insert_sql(char * sql_str, int existed_err )
{
	this->db->id=this->id;
 	int dbret;
	int acount; 
	if ((dbret=this->db->exec_update_sql(sql_str,&acount ))==DB_SUCC)
	{
		return DB_SUCC;
	}else {
		if (dbret==ER_DUP_ENTRY)
			return  existed_err;
		else return DB_ERR;
	}
}

int Ctable::exec_update_list_sql(char * sql_str, int nofind_err )
{
	int dbret;
	int acount; 
	this->db->id=this->id;
	if ((dbret=this->db->exec_update_sql(sql_str,&acount ))==DB_SUCC)
	{
		return DB_SUCC;
	}else {
		return DB_ERR;
	}
}
int Ctable::select_data( char *&_p_result,	uint32_t &_pkg_len, 
		MYSQL_RES* &res , uint32_t * p_count)
{

	this->db->id=this->id;

	if (( this->db->exec_query_sql(sqlstr,&res))!=DB_SUCC){
		return DB_ERR;
	}

    *p_count=mysql_num_rows(res);
	return SUCC;
}


