#!/bin/sh
#
#客服程序
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="PASSWD_CHANGE_DB"


alter_table_sql() {
cat <<EOF >$tmp_file
 	ALTER TABLE t_passwd_change_$1 add type INT UNSIGNED NOT NULL AFTER userid
EOF
}

table_index=0
while [ $table_index -lt 100 ]; do
	echo $table_index
	tbx=`printf "%02d" $table_index`
	alter_table_sql $tbx
	cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
	table_index=`expr $table_index + 1`
done
