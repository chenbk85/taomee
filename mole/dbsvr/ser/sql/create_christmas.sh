#!/bin/bash

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="CHRISTMAS_DB"


create_email_table_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_christmas_$1(
        userid 		INT UNSIGNED NOT NULL DEFAULT '0',
        msg 		char(255),
        flag1 		INT UNSIGNED NOT NULL DEFAULT '0',/*是否已经拿了祝福*/
        flag2 		INT UNSIGNED NOT NULL DEFAULT '0',/*祝福是否被拿了*/
        PRIMARY KEY  (userid)
	) ENGINE=innodb, CHARSET=utf8;
	CREATE INDEX idx_userid on t_christmas_$1(flag2);

EOF
}

mysqladmin -f -u $user --password="$password" -h $host DROP $dbname 
mysqladmin -f -u $user --password="$password" -h $host CREATE $dbname
table_index=0
while [ $table_index -lt 10 ] ; do
	tbx=`printf "%01d" $table_index`
	create_email_table_sql $tbx
	cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname 
	table_index=`expr $table_index + 1`
done
