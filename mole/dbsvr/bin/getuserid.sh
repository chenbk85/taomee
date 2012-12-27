#!/bin/sh

db_index=0
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_user_table_sql() {
cat <<EOF >$tmp_file
	select userid , flag  from  t_user_$1 
	 	
EOF
}

while [ $db_index -lt 100 ] ; do
	dbx=`printf "%02d" $db_index`
		table_index=0
		while [ $table_index -lt 100 ] ; do
			tbx=`printf "%02d" $table_index`
			create_user_table_sql $tbx
			cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx"  >>idlist
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done

