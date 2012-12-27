#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

alter_user_lamu_classroom_sql() {
cat <<EOF >$tmp_file
	ALTER TABLE t_user_lamu_classroom_$1
	    add networking binary(20) NOT NULL DEFAULT 0x00000000;
EOF
}

db_index=0
end_index=100

while [ $db_index -lt $end_index ] ; do
    dbx=`printf "%02d" $db_index`
    echo $dbx
    table_index=0
    while [ $table_index -lt 10 ] ; do
        echo $table_index
        tbx=`printf "%01d" $table_index`
        alter_user_lamu_classroom_sql $tbx
        cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx"
        let "table_index+=1"
    done
    let "db_index+=1"
done
