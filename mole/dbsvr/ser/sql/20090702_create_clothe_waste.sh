#!/bin/sh
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_vip_table(){
cat << EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_roominfo_clothe_waste_$1 ( 
		userid        INT UNSIGNED NOT NULL,
		clothe_type   INT UNSIGNED NOT NULL,
		clothe_waste  INT UNSIGNED NOT NULL DEFAULT 0,
		PRIMARY KEY (userid, clothe_type)
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
		create_vip_table $table
		cat $tmp_file | mysql -u $user --password="$password" -h $host "ROOMINFO_$db"
		table_index=$(( $table_index+1 ))
	done
	table_index=0
	db_index=$(($db_index + 1))
done
