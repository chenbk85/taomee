#!/bin/sh
#
#LAMUCLASS拉姆教室
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_sysarg_love_contribute_db_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_sysarg_love_contribute(
		type			INT(10) UNSIGNED NOT NULL DEFAULT 0,
		userid			INT(10) UNSIGNED NOT NULL DEFAULT 0,
		nick			CHAR(16) NOT NULL, 
		contri_itemid 	INT(10) UNSIGNED NOT NULL DEFAULT 0,
		contri_count 	INT(10) UNSIGNED NOT NULL DEFAULT 0,
		date 			INT(10) UNSIGNED NOT NULL DEFAULT 0,
		PRIMARY KEY (type, userid, contri_itemid)
	)ENGINE=innodb CHARSET=UTF8;
EOF
}

create_sysarg_love_contribute_db_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host SYSARG_DB
