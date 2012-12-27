#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

alter_user_ex_table_sql() {
	cat <<EOF >$tmp_file
		insert into t_user_attire_00 values(50026, $1, 0, 0, 0, 0, 1);
EOF
}

db_index=0
end_index=1

while [ $db_index -lt $end_index ] ; do
dbx=`printf "%02d" $db_index`
echo $dbx
table_index=123003
while [ $table_index -lt 126003  ] ; do
		alter_user_ex_table_sql $table_index 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_26" 
		let "table_index+=1"
	done
	let "db_index+=1"
done

