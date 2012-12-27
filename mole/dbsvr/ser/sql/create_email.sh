#!/bin/sh
# 保存EMAIL－＞米米号的关连
# 用户用EMAIL登入时用
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="EMAIL_DB"


create_email_table_sql() {
cat <<EOF >$tmp_file
   CREATE TABLE IF NOT EXISTS t_email_id_$1(
        email CHAR(64),
        userid INT UNSIGNED NOT NULL DEFAULT '0',
        PRIMARY KEY  (email)
	) ENGINE=innodb, CHARSET=utf8;
EOF
}

db_index=0
while [ $db_index -lt 10 ] ; do
		dbx=`printf "%01d" $db_index`
		mysqladmin -f -u $user --password="$password" -h $host DROP "EMAIL_DB_$dbx"
		mysqladmin -f -u $user --password="$password" -h $host CREATE "EMAIL_DB_$dbx"
		table_index=0
		while [ $table_index -lt 10 ] ; do
			tbx=`printf "%01d" $table_index`
			create_email_table_sql $tbx
			cat $tmp_file | mysql -u $user --password="$password" -h $host "EMAIL_DB_$dbx" 
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done
