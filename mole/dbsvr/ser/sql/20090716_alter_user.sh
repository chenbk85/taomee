#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_farm_table_sql() {
cat <<EOF >$tmp_file
#	alter table t_user_$1
#	add  sale_xiaomee_max	INT UNSIGNED NOT NULL after xiaomee_max;
	select * from t_user_auto_$1 where oil > 10;
EOF
}



db_index=0
end_index=100

while [ $db_index -lt $end_index ] ; do
	dbx=`printf "%02d" $db_index`
	echo $dbx
	table_index=0
	while [ $table_index -lt 10 ] ; do
	echo $table_index
		tbx=`printf "%d" $table_index`
		create_farm_table_sql $tbx 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
		let "table_index+=1"
	done
	let "db_index+=1"
done

