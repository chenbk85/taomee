#!/bin/sh
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_fire_cup_table() {
cat << EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_sysarg_rally( 
		team        INT UNSIGNED NOT NULL DEFAULT 0,
		score       INT UNSIGNED NOT NULL DEFAULT 0,
		day_score	INT UNSIGNED NOT NULL DEFAULT 0,
		race1		INT UNSIGNED NOT NULL DEFAULT 0,
		race2		INT UNSIGNED NOT NULL DEFAULT 0,
		race3		INT UNSIGNED NOT NULL DEFAULT 0,
		race4		INT UNSIGNED NOT NULL DEFAULT 0,
		PRIMARY KEY (team)
		) ENGINE=innodb, CHARSET=utf8;
EOF
}


create_fire_cup_table
cat $tmp_file | mysql -u $user --password="$password" -h $host "SYSARG_DB"
