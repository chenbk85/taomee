#!/bin/sh
#
#LAMUCLASS拉姆教室
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_sysarg_auction_db_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_sysarg_auction(
		userid INT(10) UNSIGNED NOT NULL DEFAULT 0,
		xiaomee INT(10) NOT NULL DEFAULT 0,
		PRIMARY KEY (userid),
		INDEX (xiaomee)
	)ENGINE=innodb CHARSET=UTF8;
EOF
}

create_sysarg_auction_db_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host SYSARG_DB
