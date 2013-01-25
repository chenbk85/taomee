#ifndef __MYSQL_WRAPPER_H__
#define __MYSQL_WRAPPER_H__


#include <stdint.h>
#include <mysql/mysql.h>
#include <mysql/errmsg.h>

#ifndef MAX_MYSQL_RES
#define MAX_MYSQL_RES 2
#endif

#define MAX_SQL_LEN		(10*1024)


struct dbconn_t {
	char		host[16];
	uint16_t	port;
	char		user[64];
	char 		passwd[64];
	char 		db[64];
};

struct dbmgr_t {
	MYSQL				my;
	MYSQL_RES			*my_res;
	MYSQL_RES			*res[MAX_MYSQL_RES];
	MYSQL_ROW			my_row;
	MYSQL_ROW			row[MAX_MYSQL_RES];
	struct dbconn_t		dbconn;
	char				sql[MAX_SQL_LEN];
	int					conned;
};

void my_init_db(struct dbmgr_t *dbmgr);
int my_connect_db(struct dbmgr_t *dbmgr);
int my_exec_sql(struct dbmgr_t *dbmgr, int res_idx);
int my_fetch_row(struct dbmgr_t *dbmgr, int res_idx);
void my_close_db(struct dbmgr_t *dbmgr);
int my_num_rows(struct dbmgr_t *dbmgr, int res_idx);


#endif /* __MYSQL_WRAPPER_H__ */
