#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
alter_usermsg_db_sql() {
cat <<EOF >$tmp_file
	ALTER TABLE t_usermsg  add medal INT UNSIGNED NOT NULL DEFAULT 0;
EOF
}


alter_usermsg_db_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host USERMSG_DB
