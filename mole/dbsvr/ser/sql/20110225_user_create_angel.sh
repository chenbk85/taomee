#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
create_user_angel_table_sql() {
cat <<EOF >$tmp_file
CREATE TABLE IF NOT EXISTS t_user_angel_$1(
	id 				INT UNSIGNED NOT NULL AUTO_INCREMENT,
	userid          INT UNSIGNED NOT NULL DEFAULT 0,
	angelid         INT UNSIGNED NOT NULL DEFAULT 0,
	position 		INT UNSIGNED NOT NULL DEFAULT 0,
	mature          INT UNSIGNED NOT NULL DEFAULT 0,
	growth          INT UNSIGNED NOT NULL DEFAULT 0,
	state           INT UNSIGNED NOT NULL DEFAULT 0,
	variate_flag    INT UNSIGNED NOT NULL DEFAULT 0,
	variate_id      INT UNSIGNED NOT NULL DEFAULT 0,
	last_caltime     INT UNSIGNED NOT NULL DEFAULT 0,
	rate            INT UNSIGNED NOT NULL DEFAULT 0,
	layer           INT UNSIGNED NOT NULL DEFAULT 0,
	primary key (id),
	index (userid,angelid)
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
		create_user_angel_table_sql $tbx 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
		let "table_index+=1"
	done
	let "db_index+=1"
done

