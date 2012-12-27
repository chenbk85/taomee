#!/bin/bash

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_user_lamu_classroom_table_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_user_lamu_classroom_course_$1(
		userid      INT(10) UNSIGNED NOT NULL DEFAULT 0,
		course_id	INT(10) UNSIGNED NOT NULL DEFAULT 0,
		course_cnt	INT(10)	UNSIGNED NOT NULL DEFAULT 0,
		PRIMARY KEY (userid, course_id)
	) ENGINE=innodb, CHARSET=utf8;

	CREATE TABLE IF NOT EXISTS t_user_lamu_classroom_remembrance_$1(
		userid		INT(10) UNSIGNED NOT NULL DEFAULT 0,
		remem_id	INT(10) UNSIGNED NOT NULL DEFAULT 0,
		got_time	INT(10) UNSIGNED NOT NULL DEFAULT 0,
		remem_cnt	INT(10) UNSIGNED NOT NULL DEFAULT 0,
		PRIMARY KEY (userid, remem_id)
	)ENGINE=innodb, CHARSET=utf8;

EOF
}

db_index=0
table_index=0

while [ $db_index -lt 100 ] ; do
	dbx=`printf "%02d" $db_index`
	echo $dbx
	while [ $table_index -lt 10 ] ; do
		tbx=`printf "%01d" $table_index`
		echo $tbx
		create_user_lamu_classroom_table_sql $tbx
		cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx"
		table_index=`expr $table_index + 1`
	done
	table_index=0
	db_index=`expr $db_index + 1`
done
