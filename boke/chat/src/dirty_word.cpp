/*
 * =====================================================================================
 *
 *       Filename:  dirty_word.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/30/2011 03:17:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ian (Ian), ian@taomee
 *        Company:  taomee co.
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <libtaomee/log.h>
#include "dirty_word.h"
#include <string.h>
#include <openssl/md5.h>
#include <dbser/mysql_iface.h>
#include <libtaomee++/utils/strings.hpp>

Cdirty_word::Cdirty_word(int reload_time) : is_inited(false)
{
	db = new mysql_interface(config_get_strval("DB_IP"),
  		  	config_get_strval("DB_USER"),
    		config_get_strval("DB_PASSWD"),
    		config_get_intval("DB_PORT",3306 ),
    		config_get_strval("DB_UNIX_SOCKET"));

	db->set_is_log_debug_sql(config_get_intval("IS_LOG_DEBUG_SQL",0 ));
	db->set_is_only_exec_select_sql(config_get_intval("IS_ONLY_EXEC_SELECT_SQL",0 ));

	sprintf(this->sqlstr, "select word, level from %s.%s",
			config_get_strval("db_name"),
			config_get_strval("table_name"));

	this->reload_time = reload_time;
	this->reload();
}

int Cdirty_word::reload()
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	if (db->exec_query_sql(sqlstr, &res) == DB_SUCC) {
		this->total_len = 0;
    	while((row= mysql_fetch_row(res))) {
			mysql_fetch_lengths(res);
			memcpy(file_cache + total_len, row[0], res->lengths[0]);
			total_len += res->lengths[0];
			file_cache[total_len ++] = '\t';
			memcpy(file_cache + total_len, row[1], res->lengths[1]);
			total_len += res->lengths[1];
			file_cache[total_len ++] = '\n';
		}

		mysql_free_result(res);

		save_bak_file();
	} else if (!is_inited) {
		if (load_bak_file() == -1) {
			ERROR_LOG("db fail ==> load bak fail");
			return -1;
		}
	}

	is_inited = true;

	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx, this->file_cache, this->total_len);
	MD5_Final(this->file_md5, &ctx);

	char md5_str[64] = {0};
	bin2hex(md5_str, (char *)this->file_md5, dirty_file_md5_len, 64);
	md5_str[63] = '\0';
	DEBUG_LOG("reload md5 is\t[%u %s]", this->total_len, md5_str);
	return 0;
}

int Cdirty_word::save_bak_file()
{
	return 0;
}

int Cdirty_word::load_bak_file()
{
    int fd = open("../data/tm_dirty.dat", O_RDONLY);
    if (fd == -1) {
        ERROR_LOG("open ../data/tm_dirty.dat fail");
        return -1;
    }

    this->total_len = read(fd, this->file_cache, sizeof(this->file_cache));
    close(fd);

	return 0;
}
