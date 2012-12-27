/*
 * =====================================================================================
 *
 *       Filename:  Cvalidationcode.cpp
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
#include "Cvalidationcode.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#define STD_QUERY_ONE_BEGIN_NOLOG( sqlstr, no_finderr ) {\
        int ret;\
        MYSQL_RES *result;\
        MYSQL_ROW row;\
        int rowcount;\
        ret =this->db->exec_query_sql(sqlstr,&result);\
        if (ret==DB_SUCC){\
            rowcount=mysql_num_rows(result);\
            if (rowcount!=1) { \
                mysql_free_result(result);       \
                return no_finderr;   \
            }else { \
                row= mysql_fetch_row(result); \
                int _fi =0;

Cvalidationcode::Cvalidationcode(mysql_interface * db ) 
	:Ctable( db,"VALIDATIONCODE_DB","t_validationcode")
{ 
 
}

int Cvalidationcode::add( char * code,   uint32_t *p_id)
{
	char code_mysql[mysql_str_len(VALIDATIONCODE_LEN )];
 	int dbret,acount ;
    set_mysql_string(code_mysql, code ,VALIDATIONCODE_LEN );
    sprintf( this->sqlstr, "insert into %s values (0,'%s')",
            this->get_table_name(),
            code_mysql);
     if ((dbret=this->db->exec_update_sql(sqlstr,&acount ))==DB_SUCC){
		*p_id=mysql_insert_id(&(this->db->handle)); //得到产生的ID
     }
     return dbret;
    
}

int Cvalidationcode::check( uint32_t id, char *code )
{
	char code_mysql[mysql_str_len(VALIDATIONCODE_LEN )];
    set_mysql_string(code_mysql, code ,VALIDATIONCODE_LEN );

	sprintf( this->sqlstr, "select id from %s  where id=%u  and code ='%s' ", 
		this->get_table_name(),id,code_mysql );
	STD_QUERY_ONE_BEGIN_NOLOG(this-> sqlstr,CHECK_VALIDATION_ERR );
	STD_QUERY_ONE_END();
}
