#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_farm_table_sql() {
cat <<EOF >$tmp_file
CREATE TABLE IF NOT EXISTS t_user_farm_$1(
	userid						INT UNSIGNED NOT NULL,
	farm_state					INT UNSIGNED NOT NULL,
	water_time       			INT UNSIGNED NOT NULL,
	farmattirelist  			VARBINARY(3204) NOT NULL,
	noused_farmattirelist 		VARBINARY(1604) NOT NULL,
	noused_feedstuffattirelist 	VARBINARY(1604) NOT NULL,
	farm_accesslist 			VARBINARY(1654),
	thiever_time				INT UNSIGNED NOT NULL,
	thiever_list				VARBINARY(505),
	primary key(userid)
) ENGINE=innodb, CHARSET=utf8;

	alter table t_user_farm_$1 add thiever_time INT UNSIGNED NOT NULL AFTER farm_accesslist;
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
		tbx=`printf "%d" $table_index`
		create_farm_table_sql $tbx 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx" 
		table_index=`expr $table_index + 1`
	done
	let "db_index+=1"
done

