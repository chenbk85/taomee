/*
 * =====================================================================================
 * 
 *       Filename:  Ctable.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CTABLE_INCL
#define  CTABLE_INCL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysql_iface.h"
#include "benchapi.h"

#include <db_macro.h>
#include <proto_header.h>

#define  GEN_SQLSTR(sqlstr,...)  snprintf( sqlstr, sizeof(sqlstr),__VA_ARGS__ )
class Ctable {
	protected:
		char sqlstr[8192*2]; 
		mysql_interface * db;
		char db_name[50];
		char table_name[50];
		//: db.table 
		char db_table_name[100];
		char dbser_return_buf[PROTO_MAX_SIZE];
		//用于保存主键值，缓存中使用,一般在get_table_name 中同步设置
		uint32_t id;
		bool 	use_cache;
	public:
		virtual char*  get_table_name();

		 int select_data( char *&_p_result,	uint32_t &_pkg_len, MYSQL_RES* &res , uint32_t * p_count);

		int record_is_existed(char * sql_str,  bool * p_existed);
		//影响行数为1行的 接口
		int exec_update_sql(char * sql_str, int nofind_err );
		int exec_insert_sql(char * sql_str, int existed_err );
		int exec_delete_sql(char * sql_str, int nofind_err );
		int exec_insert_sql_get_auto_increment_id(char * sql_str, int existed_err,
				uint32_t *p_increment_id );


		//影响行数为多行的接口( insert ,update ,delete,都可以使用)
		int exec_update_list_sql(char * sql_str, int nofind_err );

		Ctable(mysql_interface * db,const char * dbname,const char * tablename  ); 
		Ctable(mysql_interface * db); 

		virtual ~Ctable(void){} 
};

#endif   /* ----- #ifndef CTABLE_INCL  ----- */

