#!/bin/bash

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_user_lamu_classroom_exam_table_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_user_lamu_classroom_exam_$1(
		userid      INT(10) UNSIGNED NOT NULL DEFAULT 0,
		quality		TINYINT UNSIGNED NOT NULL DEFAULT 0,
		score		TINYINT UNSIGNED NOT NULL DEFAULT 0,
		difficulty	TINYINT UNSIGNED NOT NULL DEFAULT 0,
		level_s		TINYINT UNSIGNED NOT NULL DEFAULT 0,
		level_a		TINYINT UNSIGNED NOT NULL DEFAULT 0,
		level_b		TINYINT UNSIGNED NOT NULL DEFAULT 0,
		level_c		TINYINT UNSIGNED NOT NULL DEFAULT 0,
		level_d		TINYINT UNSIGNED NOT NULL DEFAULT 0,
		exp			INT(10) UNSIGNED NOT NULL DEFAULT 0,
		exam_times	TINYINT UNSIGNED NOT NULL DEFAULT 0,
		evaluate	TINYINT UNSIGNED NOT NULL DEFAULT 0,
		prize		varbinary(84) NOT NULL DEFAULT 0x00000000,
		PRIMARY KEY (userid, exam_times)
	) ENGINE=innodb, CHARSET=utf8;

	CREATE TABLE IF NOT EXISTS t_user_lamu_classroom_score_$1(
		userid		INT(10) UNSIGNED NOT NULL DEFAULT 0,
		lamu_id		INT(10) UNSIGNED NOT NULL DEFAULT 0,
		exam_times	TINYINT UNSIGNED NOT NULL DEFAULT 0,
		score		TINYINT UNSIGNED NOT NULL DEFAULT 0,
		course_info varbinary(84) NOT NULL DEFAULT 0x00000000,
		PRIMARY KEY (userid, exam_times, lamu_id)
	) ENGINE=innodb, CHARSET=utf8;
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
		create_user_lamu_classroom_exam_table_sql $tbx
		cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx"
		table_index=`expr $table_index + 1`
	done
	table_index=0
	db_index=`expr $db_index + 1`
done
