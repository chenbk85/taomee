#!/bin/sh

user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
dbname="MSGBOARD_DB"


create_table_sql() {
cat <<EOF >$tmp_file
   CREATE TABLE IF NOT EXISTS t_qa(
		msgid  	INT UNSIGNED NOT NULL AUTO_INCREMENT,	
		question  	varchar(241),
		answer  	varchar(361),
        PRIMARY KEY (msgid)
	) ENGINE=innodb, CHARSET=utf8;
EOF
}

create_table_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host $dbname
