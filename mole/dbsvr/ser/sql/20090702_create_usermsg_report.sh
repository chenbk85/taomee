#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_farm_table_sql() {
cat <<EOF >$tmp_file
CREATE TABLE IF NOT EXISTS t_user_report(
	logdate		INT UNSIGNED NOT NULL,
	userid		INT UNSIGNED NOT NULL,
	objuserid	INT UNSIGNED NOT NULL,
	reason		INT UNSIGNED NOT NULL,
	primary key(userid)
) ENGINE=innodb, CHARSET=utf8;
EOF
}


	create_farm_table_sql  
	cat $tmp_file | mysql -u $user --password="$password" -h $host "USERMSG_DB" 

