#!/bin/sh

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"


create_other_table_sql() {
cat <<EOF >$tmp_file
 	CREATE TABLE IF NOT EXISTS t_user_opt_$1(
		logtime DATETIME NOT NULL,
		userid 	INT UNSIGNED NOT NULL DEFAULT '0', 
		flag 	INT UNSIGNED NOT NULL DEFAULT '0', 
		regpost INT UNSIGNED NOT NULL DEFAULT '0', 
		ip 		INT UNSIGNED NOT NULL DEFAULT '0', 
		PRIMARY KEY  (logtime,userid)					
	)ENGINE=innodb CHARSET=utf8;
	CREATE INDEX idx on t_user_opt_$1 (logtime,userid );
	CREATE INDEX idx2 on t_user_opt_$1 (userid );
EOF
}
create_other_table_sql $1 
cat $tmp_file | mysql -u $user --password="$password" -h $host "LOGDB" 

