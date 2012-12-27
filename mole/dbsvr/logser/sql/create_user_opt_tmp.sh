#!/bin/sh

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"


create_other_table_sql() {
cat <<EOF >$tmp_file
 	CREATE TABLE IF NOT EXISTS t_user_opt_tmp(
		userid 			INT UNSIGNED NOT NULL DEFAULT '0', 
		regcount 		INT UNSIGNED NOT NULL DEFAULT '0', 
		logincount 		INT UNSIGNED NOT NULL DEFAULT '0', 
		PRIMARY KEY  (userid)					
	)ENGINE=innodb CHARSET=utf8;

 	CREATE TABLE IF NOT EXISTS t_user_reg_post(
		logdate 		INT UNSIGNED NOT NULL DEFAULT '0', 
		post 			INT UNSIGNED NOT NULL DEFAULT '0', 
		regcount 		INT UNSIGNED NOT NULL DEFAULT '0', 
		logincount 		INT UNSIGNED NOT NULL DEFAULT '0', 
		ipcount 		INT UNSIGNED NOT NULL DEFAULT '0', 
		realregcount 	INT UNSIGNED NOT NULL DEFAULT '0',
		PRIMARY KEY  (logdate,post)					
	)ENGINE=innodb CHARSET=utf8;

EOF
}
create_other_table_sql $1 
cat $tmp_file | mysql -u $user --password="$password" -h $host "LOGDB" 

