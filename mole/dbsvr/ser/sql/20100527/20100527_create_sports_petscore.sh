#!/bin/bash

#----modify--------
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"



#-------------
create_sports_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_sysarg_db_sports_petscore(
		userid			INT UNSIGNED NOT NULL DEFAULT '0',
		gameid			INT UNSIGNED NOT NULL DEFAULT '0',
		petid			INT UNSIGNED NOT NULL DEFAULT '0',
		score			INT UNSIGNED NOT NULL DEFAULT '0',
		PRIMARY KEY(userid, gameid, petid)
	) ENGINE=innodb, CHARSET=utf8;
EOF
}


create_sports_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host "SYSARG_DB"

