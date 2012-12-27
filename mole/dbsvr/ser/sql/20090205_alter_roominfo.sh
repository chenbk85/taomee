#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
create_user_table_sql() {
cat <<EOF >$tmp_file
CREATE TABLE  ROOMINFO_$1.t_roominfo_$2 (
	room_id int(10) unsigned NOT NULL default '0',
	room_flag tinyint(3) unsigned NOT NULL default '0',
	room_hot int(10) unsigned NOT NULL default '0',
	room_flower int(10) unsigned NOT NULL default '0',
	room_bug int(10) unsigned NOT NULL default '0',
	room_puglist varbinary(1452) NOT NULL default '\0\0',
	room_neighbor varbinary(802) NOT NULL default '\0\0',
	PRIMARY KEY (room_id)
)ENGINE=InnoDB DEFAULT CHARSET=latin1;

insert into  ROOMINFO_$1.t_roominfo_$2 select * from  DB_ROOMINFO_0.t_roominfo_$2$1;

EOF
}



db_index=0
end_index=10

while [ $db_index -lt $end_index ] ; do
	echo $db_index
	dbx=`printf "%01d" $db_index`
		mysqladmin -f -u $user --password="$password" -h $host DROP "ROOMINFO_$dbx"
		mysqladmin -f -u $user --password="$password" -h $host CREATE "ROOMINFO_$dbx"
		table_index=0
		while [ $table_index -lt 10 ] ; do
			tbx=`printf "%01d" $table_index`
			create_user_table_sql $dbx $tbx
			cat $tmp_file | mysql -u $user --password="$password" -h $host 
			table_index=`expr $table_index + 1`
		done
		db_index=`expr $db_index + 1`
done

