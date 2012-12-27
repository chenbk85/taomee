#!/bin/sh
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_fire_cup_table() {
cat << EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_sysarg_msg( 
		date        INT UNSIGNED NOT NULL DEFAULT 0,
		start       INT UNSIGNED NOT NULL DEFAULT 0,
		end			INT UNSIGNED NOT NULL DEFAULT 0,
		content		VARCHAR(200),
		PRIMARY KEY (date, start)
		) ENGINE=innodb, CHARSET=utf8;
EOF
}


create_fire_cup_table
cat $tmp_file | mysql -u $user --password="$password" -h $host "SYSARG_DB"
