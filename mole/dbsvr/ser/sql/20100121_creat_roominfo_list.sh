#!/bin/bash

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
create_user_table_sql() {
cat <<EOF >$tmp_file

use ROOMINFO_$1;
drop table if exists t_roominfo_list_$2;
create table IF NOT EXISTS t_roominfo_list_$2 (
	room_id int(10) unsigned not null default 0,
	presentlist varbinary(3684) not null default 0x00000000,
	pk_list VARBINARY(4004) not null default 0x00000000,
	primary key (room_id)
)charset=utf8, engine=Innodb;

insert into t_roominfo_list_$2
	select room_id, presentlist, pk_list from t_roominfo_$2;

alter table t_roominfo_$2
	drop column presentlist,
	drop column pk_list;
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

