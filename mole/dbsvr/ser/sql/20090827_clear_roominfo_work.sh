#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

time=`date +%s`
((time=$time- 950400))
week_id=`date -d "1970-01-01 UTC $time seconds" "+%W"`

clear_roominfo_work_table_sql() {
cat <<EOF >$tmp_file
	delete from t_roominfo_work_$1 where week_id=$2;
EOF
}



db_index=0
table_index=0

while [ $db_index -lt 10 ]; do
	db=`printf "%01d" $db_index`
	echo $db
	while [ $table_index -lt 10 ];do
		table=`printf "%01d" $table_index`
		clear_roominfo_work_table_sql $table $week_id
		cat $tmp_file
		cat $tmp_file | mysql -u $user --password="$password" -h $host "ROOMINFO_$db"
		table_index=$(( $table_index+1 ))
	done
	table_index=0
	db_index=$(($db_index + 1))
done
