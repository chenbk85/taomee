#!/bin/sh

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="SU_CHANGE_DB"


create_table_sql() {
cat <<EOF >$tmp_file
	 alter table  t_msg_attime drop primary key;
EOF

}

create_table_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
