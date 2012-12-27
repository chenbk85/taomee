#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
alter_user_ex_table_sql() {
cat <<EOF >$tmp_file
	alter table t_user_dining_dish_history_$1 add column dish_stars int(11) unsigned not NULL default '0';
EOF
}

db_begin_index=0
db_end_index=100

while [ $db_begin_index -lt $db_end_index ] ; do
	dbx=`printf "%02d" $db_begin_index`
	echo $dbx
	table_index=0
	while [ $table_index -lt 10 ] ; do
		tbx=`printf "%d" $table_index`
		alter_user_ex_table_sql $tbx 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
		let "table_index+=1"
	done
	let "db_begin_index+=1"
done

