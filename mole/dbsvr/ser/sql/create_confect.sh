#!/bin/bash

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="CONFECT_DB"


create_email_table_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_confect_$1(
        userid 		 INT UNSIGNED NOT NULL DEFAULT '0',
        flag  		 INT UNSIGNED NOT NULL DEFAULT '0',
        task		 INT UNSIGNED NOT NULL DEFAULT '0',
        logdate 	 INT UNSIGNED NOT NULL DEFAULT '0',
        count_today  INT UNSIGNED NOT NULL DEFAULT '0',
        count_all  	 INT UNSIGNED NOT NULL DEFAULT '0',
        PRIMARY KEY  (userid)
	) ENGINE=innodb, CHARSET=utf8;
EOF
}

mysqladmin -f -u $user --password="$password" -h $host DROP $dbname 
mysqladmin -f -u $user --password="$password" -h $host CREATE $dbname
table_index=0
while [ $table_index -lt 100 ] ; do
	tbx=`printf "%02d" $table_index`
	create_email_table_sql $tbx
	cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname 
	table_index=`expr $table_index + 1`
done
