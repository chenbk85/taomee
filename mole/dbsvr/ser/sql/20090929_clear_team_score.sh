#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
alter_user_ex_table_sql() {
cat <<EOF >$tmp_file
	update t_sysarg_rally set day_score=0;
EOF
}

alter_user_ex_table_sql 
cat $tmp_file | mysql -u $user --password="$password" -h $host "SYSARG_DB" 

