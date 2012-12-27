#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
create_user_table_sql() {
cat <<EOF >$tmp_file
CREATE TABLE IF NOT EXISTS t_sysarg_vote_decorate(
	userid             INT UNSIGNED NOT NULL DEFAULT 0,
	nick        	   binary(16) NOT NULL DEFAULT 0x00000000, 
	vote               INT UNSIGNED NOT NULL DEFAULT 0,
	datetime               INT UNSIGNED NOT NULL DEFAULT 0,
	primary key (userid)
	)ENGINE=innodb,CHARSET=utf8;
EOF
}

create_user_table_sql
cat $tmp_file | mysql -u $user --password="$password" -h $host SYSARG_DB
