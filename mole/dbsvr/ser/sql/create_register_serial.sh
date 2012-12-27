#!/bin/sh

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"


create_other_table_sql() {
cat <<EOF >$tmp_file
    CREATE TABLE IF NOT EXISTS t_register_serial(
		        register_serialid char(8),
		        userid INT UNSIGNED,
		        useflag INT UNSIGNED,
				PRIMARY KEY  (register_serialid )
				) ENGINE=innodb CHARSET=utf8;
	CREATE UNIQUE INDEX idx_serialid on t_register_serial (register_serialid );
EOF
}

	if test x$1 = x"drop" ; then
		mysqladmin -f -u $user --password="$password" -h $host DROP "REGISTER_SERIAL_DB"
	else
		mysqladmin -f -u $user --password="$password" -h $host CREATE "REGISTER_SERIAL_DB"

		create_other_table_sql 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "REGISTER_SERIAL_DB" 
	fi	

