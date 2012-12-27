#!/bin/bash

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
create_user_table_sql() {
cat <<EOF >$tmp_file

use ROOMINFO_$1;
alter table t_roominfo_$2
	drop column socks,
	drop column gift_num,
	drop column gift_userid,
	drop column energy_star,
	drop column day_star;
EOF
}



db_index=0
end_index=10

while [ $db_index -lt $end_index ] ; do
	echo $db_index
	dbx=`printf "%01d" $db_index`
		table_index=0
		while [ $table_index -lt 10 ] ; do
			tbx=`printf "%01d" $table_index`
			create_user_table_sql $dbx $tbx
			cat $tmp_file | mysql -u $user --password="$password" -h $host 
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done

