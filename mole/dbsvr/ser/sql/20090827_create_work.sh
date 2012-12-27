#!/bin/sh
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_fire_cup_table() {
cat << EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_roominfo_work_$1 ( 
		userid      	    	INT UNSIGNED NOT NULL DEFAULT 0,
		week_id             	TINYINT UNSIGNED NOT NULL DEFAULT 0,
		time_tom                TINYINT UNSIGNED NOT NULL DEFAULT 0,
		time_nick        		TINYINT UNSIGNED NOT NULL DEFAULT 0,
		PRIMARY KEY (userid, week_id)
	) ENGINE=innodb, CHARSET=utf8;
EOF
}


db_index=0
table_index=0

while [ $db_index -lt 10 ]; do
	db=`printf "%01d" $db_index`
	echo $db
	while [ $table_index -lt 10 ];do
		table=`printf "%01d" $table_index`
		create_fire_cup_table $table
		cat $tmp_file | mysql -u $user --password="$password" -h $host "ROOMINFO_$db"
		table_index=$(( $table_index+1 ))
	done
	table_index=0
	db_index=$(($db_index + 1))
done
