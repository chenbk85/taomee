#!/bin/bash

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"


alter_table_sql()
{
cat <<EOF >$tmp_file
	update t_user_$1 set iq = iq - 42949672 where iq > 42949672;
	update t_user_$1 set strong = strong - 42949672 where strong > 42949672;
EOF
}

db_index=0
end_index=100


while [ $db_index -lt $end_index ] ; do
	dbx=`printf "%02d" $db_index`
	echo $dbx
	table_index=0
		while [ $table_index -lt 10 ] ; do
			tbx=`printf "%d" $table_index`
			alter_table_sql $tbx 
			cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done

