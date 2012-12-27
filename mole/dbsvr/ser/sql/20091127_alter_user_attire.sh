#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_farm_table_sql() {
cat <<EOF >$tmp_file
	alter table t_user_attire_$1
#	add  mode SMALLINT UNSIGNED DEFAULT 0 AFTER chest,
	modify	 mode_index INT UNSIGNED;
#	modify	 mode_index INT UNSIGNED DEFAULT 0 AFTER mode;
#	create index mode_index on t_user_attire_$1(mode_index);  
EOF
}



db_index=0
end_index=100

while [ $db_index -lt $end_index ] ; do
	dbx=`printf "%02d" $db_index`
	echo $dbx
	table_index=0
	while [ $table_index -lt 100 ] ; do
	echo $table_index
		tbx=`printf "%02d" $table_index`
		create_farm_table_sql $tbx 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
		let "table_index+=1"
	done
	let "db_index+=1"
done

