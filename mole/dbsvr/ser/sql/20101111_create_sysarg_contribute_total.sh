#!/bin/sh
#
#LAMUCLASS拉姆教室
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_sysarg_contribute_total_db_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_sysarg_contribute_total(
		sum_user INT(10) UNSIGNED NOT NULL DEFAULT 0,
		sum_xiaomee INT(10) UNSIGNED NOT NULL DEFAULT 0,
		sum_attire INT(10) UNSIGNED NOT NULL DEFAULT 0
	)ENGINE=innodb CHARSET=UTF8;
	insert into t_sysarg_contribute_total values(0, 0, 0);
EOF
}

create_sysarg_contribute_total_db_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host SYSARG_DB
