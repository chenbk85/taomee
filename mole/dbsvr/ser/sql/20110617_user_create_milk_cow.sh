#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
create_user_milk_cow_table_sql() {
cat <<EOF >$tmp_file
CREATE TABLE IF NOT EXISTS t_user_milk_cow_$1(
	id 						 INT UNSIGNED NOT NULL AUTO_INCREMENT,
	userid                   INT UNSIGNED NOT NULL DEFAULT 0,
	cowid      		         INT UNSIGNED NOT NULL DEFAULT 0,
	hp			             INT UNSIGNED NOT NULL DEFAULT 0,
	mood                     INT UNSIGNED NOT NULL DEFAULT 0,
	clean                    INT UNSIGNED NOT NULL DEFAULT 0,
	milk_date				 INT UNSIGNED NOT NULL DEFAULT 0,
	eat_date				 INT UNSIGNED NOT NULL DEFAULT 0,
	primary key (id),
	index (userid, cowid)
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
		create_user_milk_cow_table_sql $tbx 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
		let "table_index+=1"
	done
	let "db_index+=1"
done

