#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
alter_table_sql() {
cat <<EOF >$tmp_file

alter TABLE  t_roominfo_day_limit_$1 add petid INT UNSIGNED NOT NULL default 0;
alter TABLE  t_roominfo_day_limit_$1 drop primary key;
alter TABLE  t_roominfo_day_limit_$1 add primary key(userid,petid);
EOF
}

db_index=0
end_index=10

while [ $db_index -lt $end_index ] ; do
	dbx=`printf "%01d" $db_index`
	echo $dbx
	table_index=0
	while [ $table_index -lt 10 ] ; do
		tbx=`printf "%d" $table_index`
		alter_table_sql $tbx 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "ROOMINFO_$dbx" 
		let "table_index+=1"
	done
	let "db_index+=1"
done

