#!/bin/sh

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="APPEAL_DB"

create_table_sql() {
cat <<EOF >$tmp_file
 alter table APPEAL_DB.t_appeal 
	add adminid	INT UNSIGNED after dealtime ;
EOF
}

create_table_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
