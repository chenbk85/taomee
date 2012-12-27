#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_animal_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_user_animal_$1(
		id 				INT UNSIGNED NOT NULL AUTO_INCREMENT,
		userid			INT UNSIGNED NOT NULL DEFAULT '0',
		animalid		INT UNSIGNED NOT NULL DEFAULT '0',
		state			INT UNSIGNED NOT NULL DEFAULT '0',
		growth			INT UNSIGNED NOT NULL DEFAULT '0',
		eattime			INT UNSIGNED NOT NULL DEFAULT '0',
		drinktime		INT UNSIGNED NOT NULL DEFAULT '0',
		total_output	INT UNSIGNED NOT NULL DEFAULT '0',
		output_time		INT UNSIGNED NOT NULL,
		lastcal_time	INT	UNSIGNED NOT NULL DEFAULT '0',
		mature			INT UNSIGNED NOT NULL DEFAULT '0',
		animal_type     INT UNSIGNED NOT NULL,
		PRIMARY KEY (id ),
		KEY (userid , animal_type)
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
        		create_animal_sql $tabx
        		cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx"
				table_index=$(($table_index + 1))
			done
			table_index=0
			db_index=$(($db_index+1))
		done

