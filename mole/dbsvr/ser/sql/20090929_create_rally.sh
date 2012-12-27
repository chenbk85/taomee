#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_animal_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_roominfo_rally_$1(
		userid			INT UNSIGNED NOT NULL DEFAULT '0',
		team			INT UNSIGNED NOT NULL DEFAULT '0',
		score			INT UNSIGNED NOT NULL DEFAULT '0',
		race1			INT UNSIGNED NOT NULL DEFAULT '0',
		race2			INT UNSIGNED NOT NULL DEFAULT '0',
		race3			INT UNSIGNED NOT NULL DEFAULT '0',
		race4			INT UNSIGNED NOT NULL DEFAULT '0',
		day_score		INT UNSIGNED NOT NULL DEFAULT '0',
		day_medal		INT UNSIGNED NOT NULL DEFAULT '0',
		PRIMARY KEY(userid)
	) ENGINE=innodb, CHARSET=utf8;
EOF
}


db_index=0
table_index=0
	while [ $db_index -lt 10 ]; do
		echo $db_index
		dbx=`printf "%01d" $db_index`
			while [ $table_index -lt 10 ];do
				tabx=`printf "%d" $table_index` 
        		create_animal_sql $tabx
        		cat $tmp_file | mysql -u $user --password="$password" -h $host "ROOMINFO_$dbx"
				table_index=$(($table_index + 1))
			done
			table_index=0
			db_index=$(($db_index+1))
		done

