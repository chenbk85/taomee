#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
alter_user_table_sql() {
cat <<EOF >$tmp_file
CREATE TABLE IF NOT EXISTS t_user_piglet_house_$1(
	userid          INT UNSIGNED NOT NULL DEFAULT 0,
	exp		        INT UNSIGNED NOT NULL DEFAULT 0,
	honor        	INT UNSIGNED NOT NULL DEFAULT 0,
	currency      	INT UNSIGNED NOT NULL DEFAULT 0,
	formation		INT UNSIGNED NOT NULL DEFAULT 0,
	feed_type     	INT UNSIGNED NOT NULL DEFAULT 0,
	feed_cnt	    INT UNSIGNED NOT NULL DEFAULT 0,
	bath_time		INT UNSIGNED NOT NULL DEFAULT 0,
	logintime		INT UNSIGNED NOT NULL DEFAULT 0,
	factory_level   INT UNSIGNED NOT NULL DEFAULT 0,
	process_cnt		INT UNSIGNED NOT NULL DEFAULT 0,
	perform_cnt		INT UNSIGNED NOT NULL DEFAULT 0,
	rob_cnt			INT UNSIGNED NOT NULL DEFAULT 0,
	prob_cnt		INT UNSIGNED NOT NULL DEFAULT 0,
	level		    INT UNSIGNED NOT NULL DEFAULT 0,
	description     char(56) NOT NULL;
	primary key (userid)
	)ENGINE=innodb,CHARSET=utf8;
EOF
}

db_index=0
end_index=100

while [ $db_index -lt $end_index ] ; do
	dbx=`printf "%02d" $db_index`
	echo $dbx
	table_index=0
	while [ $table_index -lt 10 ] ; do
		tbx=`printf "%d" $table_index`
		create_user_table_sql $tbx 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
		let "table_index+=1"
	done
	let "db_index+=1"
done

