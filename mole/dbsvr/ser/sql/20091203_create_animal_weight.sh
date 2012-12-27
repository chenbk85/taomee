#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_sql() {
cat <<EOF >$tmp_file
create table t_roominfo_animal_weight_$1
(
	user_id integer unsigned not null,
	weight integer unsigned not null,
	primary key (user_id)
) engine=innoDB, default charset=utf8;
EOF
}


db_index=0
table_index=0
	while [ $db_index -lt 10 ]; do
		echo $db_index
		dbx=`printf "%d" $db_index`
			while [ $table_index -lt 10 ];do
				tabx=`printf "%d" $table_index` 
        		create_sql $tabx
        		cat $tmp_file | mysql -u $user --password="$password" -h $host "ROOMINFO_$dbx"
				table_index=$(($table_index + 1))
			done
			table_index=0
			db_index=$(($db_index+1))
		done

