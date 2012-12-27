#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

alter_user_ex_table_sql() {
cat <<EOF >$tmp_file
	select userid, attireid, count(*) from t_user_attire_$1 where attireid=190604 group by attireid; 
	select userid, attireid, count(*) from t_user_attire_$1 where attireid=1270040 group by attireid; 
EOF
}

db_index=0
end_index=100

while [ $db_index -lt $end_index ] ; do
	dbx=`printf "%02d" $db_index`
	echo $dbx
	table_index=0
	while [ $table_index -lt 100 ] ; do
		tbx=`printf "%02d" $table_index`
		alter_user_ex_table_sql $tbx 
		cat $tmp_file
		cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
		let "table_index+=1"
	done
	let "db_index+=1"
done

