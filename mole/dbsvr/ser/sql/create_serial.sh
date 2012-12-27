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
    CREATE TABLE IF NOT EXISTS t_serial_18(
		        id INT UNSIGNED,
		        serialid  BIGINT UNSIGNED,
		        type INT UNSIGNED,
		        useflag INT UNSIGNED,
				PRIMARY KEY  (serialid)
				) ENGINE=innodb CHARSET=utf8;
	CREATE UNIQUE INDEX idx_serialid on t_serial_01(serialid );

    CREATE TABLE IF NOT EXISTS t_serial_price(
		        type INT UNSIGNED,
		        price INT UNSIGNED,
				PRIMARY KEY  (type)
				) ENGINE=innodb CHARSET=utf8;
	CREATE UNIQUE INDEX idx1 on t_serial_price(type);
	insert into t_serial_price values (0, 0 );
	insert into t_serial_price values (1, 100 );
	insert into t_serial_price values (2, 200 );
	insert into t_serial_price values (3, 300 );
	insert into t_serial_price values (4, 400 );
	insert into t_serial_price values (5, 500 );
	insert into t_serial_price values (6, 1000 );
	insert into t_serial_price values (7, 1500 );
	insert into t_serial_price values (8, 2000 );
	insert into t_serial_price values (9, 2500 );
	insert into t_serial_price values (10, 3000 );

	/*批次*/
	CREATE TABLE IF NOT EXISTS t_batch (
		        batchid 		INT UNSIGNED,
		        used  			TINYINT UNSIGNED,
		        serial_table_id TINYINT UNSIGNED, /*对应的t_serial 表的ID*/
		        gendate 		date,
		        stopdate 		date,
				PRIMARY KEY  (batchid)
	) ENGINE=innodb CHARSET=utf8;
	CREATE UNIQUE INDEX idx1 on t_batch(batchid);
EOF
}

	if test x$1 = x"drop" ; then
		mysqladmin -f -u $user --password="$password" -h $host DROP "SERIAL_DB"
	else
		mysqladmin -f -u $user --password="$password" -h $host CREATE "SERIAL_DB"

		create_other_table_sql 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "SERIAL_DB" 
	fi	

