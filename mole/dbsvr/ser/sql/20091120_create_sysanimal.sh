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
   CREATE TABLE IF NOT EXISTS t_sys_animal(
        type     INT UNSIGNED NOT NULL DEFAULT '0',
        value  	 INT NOT NULL DEFAULT '0',
        PRIMARY KEY  (type)
   ) ENGINE=innodb, CHARSET=utf8;

EOF
}

        create_email_table_sql
        cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
