#ifndef MYSQL_IFACE_H
#define MYSQL_IFACE_H

#include <mysql.h>
#include <mysqld_error.h>
#include <errmsg.h>
#include "inttypes.h" 
#include "common.h" 

class mysql_interface
{
private:
	char	host[16];
	char	user[16];
	char	pass[16];
	char 	select_db_str[20];
	unsigned int port;
	int  is_log_debug_sql;

	int execsql(const char* cmd);
	int connect_server ();
public:
	MYSQL	handle; 
	mysql_interface (const char *h, const char *u, const char *p, unsigned int port=3306);	
	~mysql_interface ();
	int get_errno() { return mysql_errno(&handle); }
	const char*  get_error() { return mysql_error(&handle); }
	void  show_error_log(const char *cmd); 
	void  set_is_log_debug_sql(int value ); 
	int  select_db (char *db_name ); 

//	bool select_db ( enum enum_db_type db_type, uint32_t id); 
	int exec_query_sql(const char *cmd, MYSQL_RES **result);
	int exec_update_sql(const char *cmd, int* affected_rows );

};

#endif //MYSQL_IFACE_H

