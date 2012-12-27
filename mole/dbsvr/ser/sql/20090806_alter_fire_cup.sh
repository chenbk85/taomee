#!/bin/sh
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

add_fire_cup(){
cat << EOF >$tmp_file
	alter table t_roominfo_fire_cup_$1
	    add  day_game_gold INT UNSIGNED NOT NULL AFTER day_gold;
EOF
}


db_index=0
table_index=0

while [ $db_index -lt 10 ]; do
	db=`printf "%01d" $db_index`
	echo $db
	while [ $table_index -lt 10 ];do
		table=`printf "%01d" $table_index`
		add_fire_cup $table
		cat $tmp_file | mysql -u $user --password="$password" -h $host "ROOMINFO_$db"
		table_index=$(( $table_index+1 ))
	done
	table_index=0
	db_index=$(($db_index + 1))
done
