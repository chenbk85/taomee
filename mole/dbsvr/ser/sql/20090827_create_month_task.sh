#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_month_task_table_sql() {
cat <<EOF >$tmp_file
 CREATE TABLE t_month_task_$1(
 	type    INT UNSIGNED NOT NULL DEFAULT 0,
 	userid 	INT UNSIGNED NOT NULL DEFAULT 0,
 	count 	INT UNSIGNED NOT NULL DEFAULT 0,
	PRIMARY KEY (userid, type)
  ) ENGINE=INNODB DEFAULT CHARSET=utf8 ;
EOF
}


for((i=0;i<100;i++)){
	tbx=`printf "%02d" $i`
	create_month_task_table_sql $tbx 
	cat $tmp_file | mysql -u $user --password="$password" -h $host "SYSARG_DB" 
}

