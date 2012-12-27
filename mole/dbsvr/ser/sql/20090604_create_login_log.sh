#!/bin/sh
#
#客服程序
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="LOGIN_DB"


create_table_sql() {
cat <<EOF >$tmp_file
	drop table IF EXISTS t_login_log_$1;	
 	CREATE TABLE IF NOT EXISTS t_login_log_$1(
        userid 		INT UNSIGNED NOT NULL,
		loginflag	INT UNSIGNED NOT NULL,
		time   		INT UNSIGNED NOT NULL,
		onlineid 	INT UNSIGNED NOT NULL,
		ip 	INT UNSIGNED NOT NULL,
		key (userid)
	)ENGINE=innodb,CHARSET=utf8;

EOF
}

table_index=0
while [ $table_index -lt 100 ]; do
	tbx=`printf "%02d" $table_index`
	create_table_sql $tbx
	cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
	table_index=`expr $table_index + 1`
done
