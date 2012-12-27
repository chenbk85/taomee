#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
create_user_rank_table_sql() {
cat <<EOF >$tmp_file
CREATE TABLE IF NOT EXISTS t_sysarg_user_rank(
	userid INT UNSIGNED NOT NULL DEFAULT 0,
	type   INT UNSIGNED NOT NULL DEFAULT 0,
	rank   INT UNSIGNED NOT NULL DEFAULT 0,
	date   INT UNSIGNED NOT NULL DEFAULT 0,
	primary key (userid,type)
	)ENGINE=innodb,CHARSET=utf8;
EOF
}

create_user_rank_table_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host "SYSARG_DB" 

