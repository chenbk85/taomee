#!/bin/sh
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

get_max_question_id(){
cat << EOF >$tmp_file
select room_id from t_roominfo_$1 where score > 0;

EOF
}


db_index=0
table_index=0

while [ $db_index -lt 10 ]; do
	db=`printf "%01d" $db_index`
	echo $db
	while [ $table_index -lt 10 ];do
		table=`printf "%01d" $table_index`
		get_max_question_id $table
		cat $tmp_file | mysql -u $user --password="$password" -h $host "ROOMINFO_$db"
		table_index=$(( $table_index+1 ))
	done
	table_index=0
	db_index=$(($db_index + 1))
done
