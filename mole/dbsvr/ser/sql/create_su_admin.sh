#!/bin/sh
#
#客服程序
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="ADMIN_DB"


create_table_sql() {
cat <<EOF >$tmp_file
 	CREATE TABLE IF NOT EXISTS t_admin_all(
        adminid		INT UNSIGNED NOT NULL AUTO_INCREMENT,
        nick 		char(16),	
       	passwd 		BINARY(16),
      	flag 		INT UNSIGNED NOT NULL,
		PRIMARY KEY (adminid)
	)ENGINE=innodb,CHARSET=utf8;

 	CREATE TABLE IF NOT EXISTS t_admin_power(
        adminid 		INT UNSIGNED NOT NULL,
        powerid 		INT UNSIGNED NOT NULL,
		PRIMARY KEY (adminid,powerid)
	)ENGINE=innodb,CHARSET=utf8;
EOF
}

mysqladmin -f -u $user --password="$password" -h $host CREATE $dbname
create_table_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
