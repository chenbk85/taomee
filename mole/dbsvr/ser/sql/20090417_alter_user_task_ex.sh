#!/bin/bash

db_index=0
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

alter_user_task_ex_sql() {
    cat <<EOF >$tmp_file

	alter table  t_user_task_ex_$1 
       MODIFY	taskvalue binary(50);
EOF
}


db_index=0
end_index=99


while [ $db_index -le $end_index ] ; do
	echo $db_index
	dbx=`printf "%02d" $db_index`
		table_index=0
		while [ $table_index -lt 10 ] ; do
			tbx=`printf "%01d" $table_index`
			alter_user_task_ex_sql  $tbx
			cat $tmp_file | mysql -u $user --password="$password" -h $host USER_$dbx
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done

