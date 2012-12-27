#!/bin/sh
#
#LAMUCLASS拉姆教室
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

alter_lamu_classroom_db_sql() {
cat <<EOF >$tmp_file
	ALTER TABLE t_lamu_classroom
		add exp INT(10) UNSIGNED NOT NULL DEFAULT 0,
		add outstand_sum INT(10) UNSIGNED NOT NULL DEFAULT 0,
		add KEY (userid, exp);
EOF
}

alter_lamu_classroom_db_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host DININGROOM
