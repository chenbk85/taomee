#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

alter_roominfo_fire_cup_table_sql() {
	cat <<EOF >$tmp_file
		alter table t_roominfo_fire_cup_$1
		add flag TINYINT UNSIGNED NOT NULL DEFAULT 0;
EOF
}

db_index=0
end_index=10

while [ $db_index -lt $end_index ] ; do
    dbx=`printf "%01d" $db_index`
    echo $dbx
    table_index=0
    while [ $table_index -lt 10 ] ; do
		echo $table_index
        tbx=`printf "%01d" $table_index`
        alter_roominfo_fire_cup_table_sql $tbx
        cat $tmp_file | mysql -u $user --password="$password" -h $host "ROOMINFO_$dbx"
        let "table_index+=1"
    done
    let "db_index+=1"
done
