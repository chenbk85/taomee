#!/bin/sh
#
#神奇密码
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"


create_other_table_sql() {
cat <<EOF >$tmp_file
    CREATE TABLE IF NOT EXISTS t_user_serial(
		userid INT UNSIGNED,
		type INT UNSIGNED,
		gentime INT UNSIGNED,
		serialid  char(8)  ,
		PRIMARY KEY  (userid,type)
	)ENGINE=innodb CHARSET=utf8;
EOF
}

	if test x$1 = x"drop" ; then
		mysqladmin -f -u $user --password="$password" -h $host DROP "USER_SERIAL_DB"
	else
		mysqladmin -f -u $user --password="$password" -h $host CREATE "USER_SERIAL_DB"

		create_other_table_sql 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_SERIAL_DB" 
	fi	

