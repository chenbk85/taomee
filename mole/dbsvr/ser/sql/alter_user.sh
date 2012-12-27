#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
create_user_table_sql() {
cat <<EOF >$tmp_file
		alter table t_user_$1
			drop birthday,
			drop passwd,
			drop nick,
			drop mobile,
			drop email,
			drop addr_province,
			drop addr_city,
			drop addr,
			drop signature ;
EOF
}

db_index=0
end_index=100
if [ "$1" == ""  ] ; then
        echo need 0,1, 2, 3, 4 
        exit;
fi
if [ "$1" == "0" ]; then
	let "db_index=0"
	let "end_index=100"
else
	let "db_index=($1-1)*25"
	let "end_index=$1*25"
fi


while [ $db_index -lt $end_index ] ; do
	dbx=`printf "%02d" $db_index`
	echo $dbx
	table_index=0
		while [ $table_index -lt 100 ] ; do
			tbx=`printf "%02d" $table_index`
			create_user_table_sql $tbx $dbx
			cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done

