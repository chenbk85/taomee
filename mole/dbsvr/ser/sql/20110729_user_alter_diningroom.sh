#!/bin/sh
#
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

alter_diningroom_db_sql() {
cat <<EOF >$tmp_file
	ALTER TABLE  t_angel_battle add master INT(10) UNSIGNED NOT NULL DEFAULT 0 ;
EOF
}

alter_diningroom_db_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host DININGROOM
