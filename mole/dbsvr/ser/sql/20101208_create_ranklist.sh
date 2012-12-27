#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
create_ranklist_table_sql() {
cat <<EOF >$tmp_file
CREATE TABLE IF NOT EXISTS t_sysarg_ranklist(
	userid INT UNSIGNED NOT NULL DEFAULT 0,
	type   INT UNSIGNED NOT NULL DEFAULT 0,
	count  INT UNSIGNED NOT NULL DEFAULT 0,
	dynamic_count INT UNSIGNED NOT NULL DEFAULT 0,
	datetime  INT UNSIGNED NOT NULL DEFAULT 0,
	primary key (userid,type),
	index (type,count,datetime)
	)ENGINE=innodb,CHARSET=utf8;
EOF
}

create_ranklist_table_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host "SYSARG_DB" 

