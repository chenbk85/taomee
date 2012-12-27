#!/bin/bash
#拉姆运动会
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="PET_SPORTS_DB"

create_cup_sql() {
cat <<EOF >$tmp_file
EOF
}

create_pet_temp_sql() {
cat <<EOF >$tmp_file
	alter table t_pet_sports_$1
        add score4 INT UNSIGNED NOT NULL,
        add score5 INT UNSIGNED NOT NULL;
EOF
}

table_index=0
while [ $table_index -lt 100 ] ; do
	tbx=`printf "%02d" $table_index`
	create_pet_temp_sql $tbx
	cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname 
	table_index=`expr $table_index + 1`
done
