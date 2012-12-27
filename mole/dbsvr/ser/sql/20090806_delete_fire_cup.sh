#!/bin/sh
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

update_fire_cup_table(){
cat << EOF >$tmp_file
	update t_roominfo_fire_cup_$1 set day_gold = 0, day_game_gold = 0, day_silver = 0, day_copper = 0;
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
