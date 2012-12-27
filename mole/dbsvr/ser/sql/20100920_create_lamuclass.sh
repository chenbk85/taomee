#!/bin/sh
#
#LAMUCLASS拉姆教室
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

create_lamuclass_db_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_lamu_classroom(
		room_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,	/*拉姆教室id*/
		userid INT(10) UNSIGNED NOT NULL DEFAULT 0,			/*米米号*/
		room_name CHAR(16) NOT NULL,							/*教室名称*/
		style_id INT(10) UNSIGNED NOT NULL DEFAULT 0,		/*装饰*/
		PRIMARY KEY (room_id),
		UNIQUE KEY (userid)
	)ENGINE=innodb CHARSET=UTF8;
EOF
}

create_lamuclass_db_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host DININGROOM
