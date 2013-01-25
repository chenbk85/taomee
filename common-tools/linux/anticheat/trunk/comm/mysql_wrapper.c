#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "util.h"
#include "mysql_wrapper.h"

void my_free_result(struct dbmgr_t *dbmgr, int res_idx)
{
	if (res_idx < MAX_MYSQL_RES && dbmgr->res[res_idx]) {
		mysql_free_result(dbmgr->res[res_idx]);
		dbmgr->res[res_idx] = NULL;
	}
}

void my_init_db(struct dbmgr_t *dbmgr)
{
	int i;
	for (i = 0; i < MAX_MYSQL_RES; i++) {
		dbmgr->res[i] = NULL;
	}
	dbmgr->my_res = NULL;
	dbmgr->conned = 0;
}

void my_close_db(struct dbmgr_t *dbmgr)
{
	int i;
	for (i = 0; i < MAX_MYSQL_RES; i++) {
		my_free_result(dbmgr, i);
	}
	mysql_close(&(dbmgr->my));
	dbmgr->conned = 0;
}

int my_connect_db(struct dbmgr_t *dbmgr)
{
	MYSQL *my;

	my = &(dbmgr->my);
	if (dbmgr->conned) my_close_db(dbmgr);
	if (mysql_init(my) != my
		|| !mysql_real_connect(my, dbmgr->dbconn.host, dbmgr->dbconn.user,
			dbmgr->dbconn.passwd, dbmgr->dbconn.db, dbmgr->dbconn.port, NULL, 0)) {
		return -1;
	}
	dbmgr->conned = 1;

	return 0;
}

int my_exec_sql(struct dbmgr_t *dbmgr, int res_idx)
{
	int select = 0;

	MYSQL *my = &(dbmgr->my);

	if (!dbmgr->conned) return -1;
	if (!strncasecmp(skip_blank(dbmgr->sql), "select", 6)) select = 1;
	if (select) {
		if (res_idx < 0 || res_idx >= MAX_MYSQL_RES) {
			return -1;
		}
		my_free_result(dbmgr, res_idx);
	}

	if (mysql_real_query(my, dbmgr->sql, strlen(dbmgr->sql))) {
		if (mysql_errno(my) == CR_SERVER_GONE_ERROR) {
			my_close_db(dbmgr);
		}
		return -1;
	}
	if (select && !(dbmgr->my_res = dbmgr->res[res_idx] = mysql_store_result(my))) {
		return -1;
	}
	return 0;
}

int my_fetch_row(struct dbmgr_t *dbmgr, int res_idx)
{
	if (!dbmgr->conned) return -1;
	if (res_idx >= 0 && res_idx < MAX_MYSQL_RES) { 
		if ((dbmgr->my_row = dbmgr->row[res_idx] = mysql_fetch_row(dbmgr->res[res_idx]))) {
			return 0;
		} else {
			return -1;
		}
	} else if ((dbmgr->my_row = mysql_fetch_row(dbmgr->my_res))) {
		return 0; 
	}

	return -1;
}

int my_affected_rows(struct dbmgr_t *dbmgr)
{
	return mysql_affected_rows(&(dbmgr->my));
}

int my_num_rows(struct dbmgr_t *dbmgr, int res_idx)
{
	MYSQL_RES *res;

	if (res_idx >= 0 && res_idx < MAX_MYSQL_RES) {
		res = dbmgr->res[res_idx];
	} else {
		res = dbmgr->my_res;
	}
	return mysql_num_rows(res);
}

int my_num_fields(struct dbmgr_t *dbmgr, int res_idx)
{
	MYSQL_RES *res;

	if (res_idx >= 0 && res_idx < MAX_MYSQL_RES) {
		res = dbmgr->res[res_idx];
	} else {
		res = dbmgr->my_res;
	}
	return mysql_num_fields(res);
}

my_ulonglong my_insert_id(struct dbmgr_t *dbmgr)
{
	return mysql_insert_id(&(dbmgr->my));
}
