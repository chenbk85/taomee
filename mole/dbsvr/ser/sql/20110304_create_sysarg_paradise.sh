#!/bin/sh
#
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_sysarg_paradise_rank_db_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_sysarg_paradise_rank(
		userid INT   UNSIGNED NOT NULL DEFAULT 0,
		exp    INT   UNSIGNED NOT NULL DEFAULT 0,
		count  INT   UNSIGNED NOT NULL DEFAULT 0,
		PRIMARY KEY (userid)
	)ENGINE=innodb CHARSET=UTF8;
EOF
}

create_sysarg_paradise_rank_db_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host SYSARG_DB
