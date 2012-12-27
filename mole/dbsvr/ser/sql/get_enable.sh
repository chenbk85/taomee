#!/bin/sh

db_index=0
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_sql() {
cat <<EOF >$tmp_file
	select * from t_serial_05;
EOF
}

create_sql 
cat $tmp_file | mysql -u $user --password="$password" -h $host "SERIAL_DB" 
