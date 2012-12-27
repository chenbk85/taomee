#!/bin/bash

db_index=0
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_user_table_sql() {
    cat <<EOF >$tmp_file
	delete from t_user_attire_$1 where count=0;
EOF
}

if [ "$1" == ""  ] ; then
        echo need 0,1..16
        exit;
fi

db_index=0
end_index=99

if [ "$1" == "0" ]; then
    let "db_index=0"
    let "end_index=99"
else
    let "fix=($1-1)/4"
    let "add1flag=($1-1)%4"
    if [ "$add1flag" == "0" ]; then
        let "db_index=($1-1)*6+fix"
    else
        let "db_index=($1-1)*6+fix+1"
    fi
    let "end_index=($1)*6+fix"
fi

echo "do db:" $db_index  "-" $end_index

while [ $db_index -le $end_index ] ; do
	echo "deal" $db_index
	dbx=`printf "%02d" $db_index`
		table_index=0
		while [ $table_index -lt 100 ] ; do
			tbx=`printf "%02d" $table_index`
			create_user_table_sql  $tbx
			cat $tmp_file | mysql -u $user --password="$password" -h $host USER_$dbx
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done

