#!/bin/sh
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_fire_cup_table() {
cat << EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_sysarg_team_medal( 
		date   	    INT UNSIGNED NOT NULL DEFAULT 0,
		team        INT UNSIGNED NOT NULL DEFAULT 0,
		gold        INT UNSIGNED NOT NULL DEFAULT 0,
		silver		INT UNSIGNED NOT NULL DEFAULT 0,
		copper		INT UNSIGNED NOT NULL DEFAULT 0,
		num			INT UNSIGNED NOT NULL DEFAULT 0,
		PRIMARY KEY (date, team)
		) ENGINE=innodb, CHARSET=utf8;
EOF
}


create_fire_cup_table
cat $tmp_file | mysql -u $user --password="$password" -h $host "SYSARG_DB"
