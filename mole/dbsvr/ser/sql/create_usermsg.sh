#!/bin/sh
#
#举报和投稿
#
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="USERMSG_DB"


create_table_sql() {
cat <<EOF >$tmp_file
	CREATE TABLE IF NOT EXISTS t_writing(
		logtime 	INT UNSIGNED NOT NULL,
		serialid 	INT UNSIGNED NOT NULL,
        msgtype 	INT UNSIGNED NOT NULL,/*投稿渠道*/
        msgstore 	INT UNSIGNED NOT NULL,/*存放位置*/
        userid 		INT UNSIGNED NOT NULL,
		msg 		varchar(4004),
		reportid	INT UNSIGNED NOT NULL,
		reporttime	INT UNSIGNED NOT NULL,
        report 		varchar(1004),
		PRIMARY key(id),
		key(logtime),
		key(userid)
	)ENGINE=innodb, CHARSET=utf8;
EOF
}

create_table_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
