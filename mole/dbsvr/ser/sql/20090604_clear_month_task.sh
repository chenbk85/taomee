#!/bin/bash

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
create_sql() {
cat <<EOF >$tmp_file
delete from  SYSARG_DB.t_month_task;
EOF
}

create_sql
cat $tmp_file | mysql -u $user --password="$password"  -h $host

