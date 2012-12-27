#!/bin/sh
#
#客服程序
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="PASSWD_CHANGE_DB"


create_table_sql() {
cat <<EOF >$tmp_file
 	CREATE TABLE IF NOT EXISTS t_passwd_change_$1(
        userid INT UNSIGNED NOT NULL,
		time   INT UNSIGNED NOT NULL,
       	passwd 		BINARY(16),
		key (userid)
	)ENGINE=innodb,CHARSET=utf8;

EOF
}

mysqladmin -f -u $user --password="$password" -h $host CREATE $dbname
table_index=0
while [ $table_index -lt 100 ]; do
	tbx=`printf "%02d" $table_index`
	create_table_sql $tbx
	cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
	table_index=`expr $table_index + 1`
done
