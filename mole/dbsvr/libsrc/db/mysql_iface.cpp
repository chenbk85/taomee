#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mysql_iface.h"
#include "common.h"
#include "proto.h"
#include "benchapi.h"

mysql_interface::mysql_interface (const char *h, const char *u, const char *p, unsigned int bind_port)
{
	mysql_init(&handle);
	safe_copy_string (host, h);
	safe_copy_string (user, u);
	safe_copy_string (pass, p);
	port = bind_port;
	this->is_log_debug_sql=0;
	DEBUG_LOG("DB conn msg [%s][%s][%s]",h,u,p);
	connect_server ();
}

void  mysql_interface::show_error_log(const char* cmd ) 
{
		DEBUG_LOG(" sqlerr:no[%d]:msg[%s]:sql[%s]",
				this->get_errno(),this->get_error(), cmd);
}

mysql_interface::~mysql_interface ()
{
	mysql_close (&handle);
}

int mysql_interface::connect_server ()
{
	if (!mysql_real_connect(&handle, host, user, pass, NULL, port, NULL, CLIENT_FOUND_ROWS ))
	{
		DEBUG_LOG("DB connect is err [%d]\n",this->get_errno());
		return DB_ERR;
	}
	DEBUG_LOG("DB connect is ok [%d]\n",this->get_errno());
	//mysql_autocommit(&handle,F);
	return SUCC;
}

int mysql_interface::exec_query_sql (const char *cmd, MYSQL_RES **result)
{
	if (!this->execsql(cmd)){
		if((*result = mysql_store_result (&handle))!=NULL){
			return  DB_SUCC;
		} else {
			this->show_error_log(cmd);
			return DB_ERR;
		}

	} else {
		return DB_ERR;
	}
}

int 
mysql_interface::exec_update_sql(const char *cmd ,int * affected_rows)
{
	if (!this->execsql(cmd)){
		 *affected_rows= mysql_affected_rows(&(this->handle));
		 INFO_LOG("[%d][%s;]",*affected_rows,cmd );
		 return DB_SUCC;
	}
	else {
		return this->get_errno();
	}

}

int mysql_interface::execsql(const char* cmd)
{
	int ret;

#ifdef LOG_SQL_DEBUG  
	if (this->is_log_debug_sql){
		DEBUG_LOG("SQL:[%s;]",cmd );
	}
#else
	INFO_LOG("SQL:[%s;]",cmd );
#endif
	ret=mysql_query (&handle, cmd );
	if (!ret){ //SUCC
		return ret;
	}
	if (this->get_errno()==CR_SERVER_GONE_ERROR )
	{ //server go away, must reconnect
	  if (this->connect_server()==DB_SUCC){
		return  mysql_query (&handle, cmd ); 	 
	  }else{
		return this->get_errno(); 
	  }

	}else{
		this->show_error_log(cmd);
		return this->get_errno();
	}	
}

void mysql_interface::set_is_log_debug_sql(int value )
{
	this->is_log_debug_sql=value;
}

int mysql_interface::select_db (char *db_name )
{
	int ret=mysql_select_db(&this->handle ,db_name);
	if (ret!=SUCC) {
		this->show_error_log(db_name );
	}
	return this->get_errno();
}

