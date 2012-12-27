#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
create_user_table_sql() {
cat <<EOF >$tmp_file
CREATE TABLE IF NOT EXISTS t_roominfo_fire_cup_$1(
	userid          	INT UNSIGNED NOT NULL DEFAULT 0,
	team         		INT UNSIGNED NOT NULL DEFAULT 0,
	count         		INT UNSIGNED NOT NULL DEFAULT 0,
	today         		INT UNSIGNED NOT NULL DEFAULT 0,
	day_left         	INT UNSIGNED NOT NULL DEFAULT 0,
	flag			    tinyint(3) UNSIGNED NOT NULL DEFAULT 0,
	primary key (userid)
	)ENGINE=innodb,CHARSET=utf8;
EOF
}

db_index=0
end_index=10

while [ $db_index -lt $end_index ] ; do
	dbx=`printf "%d" $db_index`
	echo $dbx
	table_index=0
	while [ $table_index -lt 10 ] ; do
		tbx=`printf "%d" $table_index`
		create_user_table_sql $tbx 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "ROOMINFO_$dbx" 
		let "table_index+=1"
	done
	let "db_index+=1"
done

