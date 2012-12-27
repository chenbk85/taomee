#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
create_user_table_sql() {
cat <<EOF >$tmp_file
CREATE TABLE IF NOT EXISTS t_sysarg_mvp_day(
	day          	INT UNSIGNED NOT NULL DEFAULT 0,
    value_1         INT UNSIGNED NOT NULL DEFAULT 0,
	value_2         INT UNSIGNED NOT NULL DEFAULT 0,
	primary key (day)
	)ENGINE=innodb,CHARSET=utf8;
EOF
}

create_user_table_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host "SYSARG_DB"

