#include <string.h>
#include <stdio.h>
#include "mysql_iface.h"

#define			T		 				1
#define		 	F				   		0
#define safe_copy_string(dst,src) { \
	if (src){\
		strncpy (dst, src, sizeof(dst) - 1); \
		dst[sizeof(dst) - 1] = '\0'; \
	}else{\
		dst[0] = '\0'; \
	}\
}

#include "proto_header.h"
stru_db_cache_state g_db_cache_state;//全局的状态


mysql_interface::mysql_interface (const char *h, const char *u, const char *p, unsigned int bind_port,const char * a_unix_socket,const char * char_set )//,const char * big_cache_hostport )
{
	DEBUG_LOG("MYSQL_INTERFACE::MYSQL_INTERFACE  7" );
	mysql_init(&handle);
	safe_copy_string (host, h);
	safe_copy_string (user, u);
	safe_copy_string (pass, p);
	if (a_unix_socket!=NULL) {
		safe_copy_string (this->unix_socket , a_unix_socket );
	}else{
		this->unix_socket[0]='\0';
	}
	port = bind_port;

	safe_copy_string (this->mysql_char_set,char_set );

	this->is_log_debug_sql=0;
	this->is_only_exec_select_sql=0;
	this->is_select_db_succ=true;



	DEBUG_LOG("DB conn msg [%s][%s][%s][%s]",h,u,p,this->unix_socket);

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
	//CLIENT_FOUND_ROWS		
	//让update返回的影响行数是查到的行数
	char * us;
	if (this->unix_socket[0]=='\0'){
		us=NULL;	
	}else{
		us=this->unix_socket;
	}
	if (this->mysql_char_set[0]!='\0' ){
		mysql_options(&handle,MYSQL_SET_CHARSET_NAME,this->mysql_char_set );	
	}
	
	if (!mysql_real_connect(&handle, host, user, pass, NULL, port,us , CLIENT_FOUND_ROWS ))
	{
		DEBUG_LOG("DB connect is err [%d]\n",this->get_errno());
		return DB_ERR;
	}
	DEBUG_LOG("DB connect is ok [%d]\n",this->get_errno());
	mysql_autocommit(&handle,F);



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
 	if (this->is_only_exec_select_sql ){
		return DB_CANNOT_UPDATE_ERR;
	};

	if (!this->execsql(cmd)){
		*affected_rows= mysql_affected_rows(&(this->handle));
		if (!this->is_log_debug_sql){
			INFO_LOG("D[%d][%s;]",*affected_rows,cmd );
		}
		 return DB_SUCC;
	}
	else {
		return this->get_errno();
	}

}

int mysql_interface::execsql(const char* cmd)
{
	int ret;

	if (this->is_log_debug_sql){
		KDEBUG_LOG(this->id ,"SQL:[%s;]",cmd );
	}

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
	if (this->is_test_env ) {
		if(strncmp(this->select_db_name_fix.c_str(),db_name,
					this->select_db_name_fix.length())!=0){
			DEBUG_LOG("select db ERROR :[%s], you set it is [%s] ",
				   	db_name,this->select_db_name_fix.c_str()  );	
			this->is_select_db_succ=false;
			return  NO_DEFINE_ERR;
		}
		if (this->select_db_name!="" && this->select_db_name !=db_name ){
			//两次select 的db名字不一致
			DEBUG_LOG("select db ERROR :select [%s], the prev select [%s] ",
				   	db_name,this->select_db_name.c_str());	
			this->is_select_db_succ=false;
			return  NO_DEFINE_ERR;
		}
	}

	int ret=mysql_select_db(&this->handle ,db_name);
	if (ret!=SUCC) {
		this->show_error_log(db_name );
	}
	return this->get_errno();
}
