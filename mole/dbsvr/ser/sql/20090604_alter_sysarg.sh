#!/bin/sh
#
#系统数
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="SYSARG_DB"

create_email_table_sql() {
cat <<EOF >$tmp_file
	/*每天只能做一次,每周只能做一次*/
   drop TABLE  t_month_task ;
   CREATE TABLE IF NOT EXISTS t_month_task (
        type   INT UNSIGNED NOT NULL DEFAULT '0',
        userid INT UNSIGNED NOT NULL DEFAULT '0',
        count INT UNSIGNED NOT NULL DEFAULT '0',
		key(userid),
        PRIMARY KEY  (type , userid)
   ) ENGINE=memory, CHARSET=utf8;

EOF
}

create_email_table_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
