#!/bin/sh
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

update_fire_cup_table(){
cat << EOF >$tmp_file
	update t_roominfo_point_$1 set water_list = 0x0000;
EOF
}


db_index=0
table_index=0

while [ $db_index -lt 10 ]; do
	db=`printf "%01d" $db_index`
	echo $db
	while [ $table_index -lt 10 ];do
		table=`printf "%01d" $table_index`
		update_fire_cup_table $table
		cat $tmp_file | mysql -u $user --password="$password" -h $host "ROOMINFO_$db"
		table_index=$(( $table_index+1 ))
	done
	table_index=0
	db_index=$(($db_index + 1))
done
