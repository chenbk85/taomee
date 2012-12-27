#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
create_user_table_sql() {
cat <<EOF >$tmp_file
	ALTER TABLE t_sysarg_christmas_socks  add type INT UNSIGNED NOT NULL DEFAULT 0;
	ALTER TABLE t_sysarg_christmas_socks drop  primary key;
	ALTER TABLE t_sysarg_christmas_socks add primary key(userid, type);
EOF
}

create_user_table_sql $tbx 
cat $tmp_file | mysql -u $user --password="$password" -h $host "SYSARG_DB" 

