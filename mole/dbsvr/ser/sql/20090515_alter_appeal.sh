#!/bin/sh

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="APPEAL_DB"

create_table_sql() {
cat <<EOF >$tmp_file
alter table APPEAL_DB.t_appeal 
    add  why char(200) after userid,
	add  flag INT UNSIGNED  after userid;
EOF
}

create_table_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
