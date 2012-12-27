#!/bin/sh
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

update_fire_cup_table(){
cat << EOF >$tmp_file
	update t_user_game_$1 set pkflag = 0;
EOF
}


db_index=0
table_index=0

while [ $db_index -lt 1 ]; do
	db=`printf "%01d" $db_index`
	echo $db
	while [ $table_index -lt 100 ];do
		table=`printf "%02d" $table_index`
		update_fire_cup_table $table
		cat $tmp_file | mysql -u $user --password="$password" -h $host "MOLE_GAME_DB"
		table_index=$(( $table_index+1 ))
	done
	table_index=0
	db_index=$(($db_index + 1))
done
