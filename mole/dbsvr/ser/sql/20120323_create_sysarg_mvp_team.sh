#!/bin/sh
#
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_sysarg_db_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_sysarg_mvp_team(
		mvp_team	 	INT   UNSIGNED NOT NULL DEFAULT 0,
		nick    		char(16)  NOT NULL DEFAULT 0x00000000,
		logo    		INT   UNSIGNED NOT NULL DEFAULT 0,
		badge    		INT   UNSIGNED NOT NULL DEFAULT 0,
		teamid    		INT   UNSIGNED NOT NULL DEFAULT 0,
		count    		INT   UNSIGNED NOT NULL DEFAULT 0,
		primary key (mvp_team),
		index (badge),
		index (count)
	)ENGINE=innodb CHARSET=UTF8;
EOF
}

create_sysarg_db_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host SYSARG_DB
