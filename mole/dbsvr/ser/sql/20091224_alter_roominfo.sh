#!/bin/bash

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

alter_user_ex_table_sql() {
cat <<EOF >$tmp_file
	drop table t_roominfo_flower_$1;
	alter TABLE t_roominfo_$1  
	drop duck_statu,
	drop score,
	add gift_num INT UNSIGNED NULL DEFAULT 0,
	add gift_userid VARBINARY(2904) NOT NULL DEFAULT 0x00000000,
	add energy_star INT UNSIGNED NOT  NULL DEFAULT 0,
	add day_star TINYINT UNSIGNED NOT  NULL DEFAULT 0;
EOF
}

db_index=0
end_index=10

while [ $db_index -lt $end_index ] ; do
	dbx=`printf $db_index`
	echo $dbx
	table_index=0
	while [ $table_index -lt 10 ] ; do
		tbx=`printf "%d" $table_index`
		alter_user_ex_table_sql $tbx 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "ROOMINFO_$dbx" 
		let "table_index+=1"
	done
	let "db_index+=1"
done

