#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_dragon_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_user_dragon_$1(
		userid			INT UNSIGNED NOT NULL DEFAULT '0',
		dragonid	    INT UNSIGNED NOT NULL DEFAULT '0',
		name			CHAR(16) NOT NULL,		
		growth			INT UNSIGNED NOT NULL DEFAULT '0',
		state			INT UNSIGNED NOT NULL DEFAULT '0',
		create_time		INT UNSIGNED NOT NULL DEFAULT '0',
		PRIMARY KEY(userid,dragonid,state,create_time)
	) ENGINE=innodb, CHARSET=utf8;
EOF
}


db_index=0
table_index=0
	while [ $db_index -lt 100 ]; do
		echo $db_index
		dbx=`printf "%02d" $db_index`
			while [ $table_index -lt 10 ];do
				tabx=`printf "%d" $table_index` 
        		create_dragon_sql $tabx
        		cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx"
				table_index=$(($table_index + 1))
			done
			table_index=0
			db_index=$(($db_index+1))
		done

